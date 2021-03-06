// Package shapify implements a genetic algorithm evolving a population of images
// made of colored shapes in order to match an user-provided reference image.
package shapify

import (
	"bytes"
	"errors"
	"fmt"
	"math"
	"os"
	"path/filepath"
	"sync"

	"github.com/arl/artificial/pkg/stats"

	"github.com/arl/evolve"
	"github.com/arl/evolve/condition"
	"github.com/arl/evolve/engine"
	"github.com/arl/evolve/operator"
	"github.com/arl/evolve/operator/xover"
	"github.com/arl/evolve/pkg/bitstring"
	"github.com/arl/evolve/selection"
)

// Config is the configuration for shapify
type Config struct {
	Ntris     uint64 // NTris is the number of triangles
	BaseImage string // Base is the path to the base image

	OrgW, OrgH   int  // base image original dimensions
	W, H         int  // base image scaled down dimensions
	wbits, hbits uint // number of bits in image width and height
}

// pow2roundup rounds n up to the next higher power of 2, or n if it is already a power of 2.
func pow2roundup(x int) int {
	if x <= 1 {
		return 1
	}
	x--
	x |= x >> 1
	x |= x >> 2
	x |= x >> 4
	x |= x >> 8
	x |= x >> 16
	return x + 1
}

// Setup vets the values in the config structure and pre-compute some necessary configuration fields.
// It returns an error in case something is either not right or not supported.
func (c *Config) Setup() error {
	if pow2roundup(int(c.W)) != int(c.W) || pow2roundup(int(c.H)) != int(c.H) {
		return errors.New("scaled width and height must be power of 2")
	}
	c.wbits = uint(math.Log2(float64(c.W)))
	c.hbits = uint(math.Log2(float64(c.H)))
	return nil
}

const statsSamplerSize = 200

// Shapify implements a genetic algorithm evolving a population of images
// made of colored shapes in order to match an user-provided reference image.
func Shapify(cfg Config) (stop func() error, err error) {
	stop = func() error { return nil }

	// construct the cutset for crossover and mutation
	cuts := cutset{}
	cuts.set(cfg)

	// define the crossover
	xover := xover.New(&mater{
		cuts: cuts,
	})
	err = xover.SetPoints(3)
	if err != nil {
		return stop, err
	}
	err = xover.SetProb(0.7)
	if err != nil {
		return stop, err
	}

	// define the mutation operator
	mut := &mutation{
		pwholetri: 0.01,
		ptriangle: 0.02,
		pcolor:    0.02,
		palpha:    0.01,
		cuts:      cuts,
	}

	renderer := newRenderer(cfg)
	evaluator, err := newEval(renderer, cfg)
	if err != nil {
		return stop, err
	}

	epocher := engine.Generational{
		Op:   operator.Pipeline{xover, mut},
		Eval: evaluator,
		Sel:  selection.RouletteWheel,
	}
	gen := &gen{cfg: cfg}
	eng, err := engine.New(gen, evaluator, &epocher)
	if err != nil {
		return stop, err
	}

	// add evolution observers
	dsampler := stats.NewDownsampler(statsSamplerSize)
	eng.AddObserver(dsampler)

	plotsc := make(chan []byte)
	imagesc := make(chan []byte)

	plotUpdater, err := stats.NewPlotUpdater(stats.DefaultPlotter, dsampler, 100,
		func(buf bytes.Buffer, gen int) {
			select {
			case plotsc <- buf.Bytes():
			default:
			}
		})
	if err != nil {
		return stop, err
	}
	eng.AddObserver(plotUpdater)

	imgout := &imageOutput{
		every:    50,
		renderer: renderer,
		format:   "png",
		imagecb: func(buf []byte) {
			select {
			case imagesc <- buf:
			default:
			}
		},
	}
	eng.AddObserver(imgout)

	gui, err := newGUI()
	if err != nil {
		return stop, err
	}
	gui.start(plotsc, imagesc)

	// start evolution in a goroutine, stoppable from the caller
	var (
		lastpop   evolve.Population
		userabort = condition.NewUserAbort()
		wg        sync.WaitGroup
	)
	wg.Add(1)
	go func() {
		defer wg.Done()
		lastpop, _, err = eng.Evolve(
			50,               // number of individuals per generation
			engine.Elites(2), // the nth best are directly put into next population
			engine.EndOn(userabort),
		)
	}()
	return func() error {
		userabort.Abort()
		wg.Wait()
		var samples [statsSamplerSize]evolve.PopulationStats
		dsampler.Samples(samples[:])
		return summary(samples[:], lastpop[0], "./work/_tmp")
	}, nil
}

func summary(samples []evolve.PopulationStats, best *evolve.Individual, dir string) error {
	// print information about best individual
	fmt.Println("best individual fitness ", best.Fitness)
	fmt.Println("best individual as big int string ", best.Candidate.(*bitstring.Bitstring).BigInt().String())

	// save evolution stats as csv
	csvpath := filepath.Join(dir, "stats.csv")
	err := stats.SaveAsCSVFile(csvpath, samples)
	if err != nil {
		return fmt.Errorf("summary: %v", err)
	}
	fmt.Println("summary: saved evolution stats to", csvpath)

	// save plot evolution stats
	plotpath := fmt.Sprintf("./work/_tmp/stats.%s", stats.DefaultPlotter.Format)
	f, err := os.Create(plotpath)
	if err != nil {
		return fmt.Errorf("summary: %v", err)
	}
	defer f.Close()
	err = stats.DefaultPlotter.WritePlot(f, samples)
	if err != nil {
		return fmt.Errorf("summary: %v", err)
	}
	fmt.Println("summary: saved evolution stats plot to", plotpath)
	return nil
}
