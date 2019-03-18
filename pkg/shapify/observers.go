package shapify

import (
	"fmt"
	"image"
	"log"
	"path/filepath"

	"github.com/fogleman/gg"

	"github.com/arl/evolve"
	"github.com/arl/evolve/pkg/bitstring"
)

// folderOutput is a population observer that saves the best candidate image
// as a PNG file in the specified folder, every given number of generation.
type folderOutput struct {
	folder   string // output folder
	every    int    // save the best image every nth generation
	print    bool   // print fast population stats on standard output
	renderer *renderer
}

func (fo *folderOutput) Observe(stats *evolve.PopulationStats) {
	if stats.GenNumber%fo.every != 0 {
		return
	}
	cand := stats.BestCand.(*bitstring.Bitstring)
	img := fo.renderer.pool.Get().(*image.RGBA)
	fo.renderer.draw(img, cand)
	err := gg.SavePNG(filepath.Join(fo.folder, fmt.Sprintf("%d.png", stats.GenNumber)), img)
	if err != nil {
		panic(err)
	}
	fo.renderer.pool.Put(img)
	if fo.print {
		log.Printf("Generation %d: fitness %f stddev: %f", stats.GenNumber, stats.BestFitness, stats.StdDev)
	}
}
