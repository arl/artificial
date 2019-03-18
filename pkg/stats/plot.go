package stats

import (
	"bytes"
	"fmt"
	"io"
	"log"

	"gonum.org/v1/plot"
	"gonum.org/v1/plot/plotter"
	"gonum.org/v1/plot/plotutil"
	"gonum.org/v1/plot/vg"

	"github.com/arl/evolve"
)

// A Plotter provides basic plotting of evolution statistics.
type Plotter struct {
	Title  string
	W, H   vg.Length
	Format string
}

// DefaultPlotter is a plotter predefined with some sensible values.
var DefaultPlotter = Plotter{
	Title:  "Shapify Evolution Plot",
	W:      16 / 2 * vg.Inch,
	H:      9 / 2 * vg.Inch,
	Format: "png",
}

func (pl Plotter) draw(samples []evolve.PopulationStats) (*plot.Plot, error) {
	p, err := plot.New()
	if err != nil {
		return nil, err
	}

	fitness := make(plotter.XYs, len(samples))
	mean := make(plotter.XYs, len(samples))
	stddev := make(plotter.XYs, len(samples))

	for i := range samples {
		sample := &samples[i]
		x := float64(sample.GenNumber)
		fitness[i].X = x
		mean[i].X = x
		stddev[i].X = x
		fitness[i].Y = sample.BestFitness
		mean[i].Y = sample.Mean
		stddev[i].Y = sample.StdDev
	}
	fmt.Println("len samplmes", len(samples))

	p.Title.Text = pl.Title
	p.X.Label.Text = "Generation"
	p.Y.Label.Text = "Fitness"

	err = plotutil.AddLinePoints(p,
		"Best fitness", fitness,
		"Mean fitness", mean,
		"Stddev", stddev)
	if err != nil {
		return nil, err
	}
	return p, nil
}

// WritePlot writes to w a plot with the provided samples.
func (pl Plotter) WritePlot(w io.Writer, samples []evolve.PopulationStats) error {
	p, err := pl.draw(samples)
	if err != nil {
		return fmt.Errorf("stats.Plotter: can't draw plot: %v", err)
	}
	wt, err := p.WriterTo(pl.W, pl.H, pl.Format)
	if err != nil {
		return fmt.Errorf("stats.Plotter: can't generate plot: %v", err)
	}
	_, err = wt.WriteTo(w)
	if err != nil {
		return fmt.Errorf("stats.Plotter: can't write plot: %v", err)
	}
	return nil
}

// PlotUpdateFunc receives the buffer containing a plot of the nth generation.
type PlotUpdateFunc func(buf bytes.Buffer, generation int)

// A PlotUpdater provides updated plots representing population evolution.
type PlotUpdater struct {
	every    int
	plotter  Plotter
	dsampler *Downsampler
	popbuf   []evolve.PopulationStats
	plotbuf  bytes.Buffer
	f        PlotUpdateFunc
}

// NewPlotUpdater returns a new plot updater that plots with p.
//
// The plotted populations are provided by the specified downsampler.
// Every 'every' new generation, f is called with a new plot.
func NewPlotUpdater(p Plotter, dsampler *Downsampler, every int, f PlotUpdateFunc) (*PlotUpdater, error) {
	return &PlotUpdater{
		dsampler: dsampler,
		plotter:  p,
		popbuf:   make([]evolve.PopulationStats, dsampler.maxsamples),
		every:    every,
		f:        f,
	}, nil
}

// Observe is called when a new population is generated.
func (pu *PlotUpdater) Observe(stats *evolve.PopulationStats) {
	if stats.GenNumber%pu.every != 0 {
		return
	}
	n := pu.dsampler.Samples(pu.popbuf)
	pu.plotbuf.Reset()
	err := pu.plotter.WritePlot(&pu.plotbuf, pu.popbuf[:n])
	if err != nil {
		log.Printf("stats.PlotUpdater: couldn't write plot: %v", err)
		return
	}
	pu.f(pu.plotbuf, stats.GenNumber)
}
