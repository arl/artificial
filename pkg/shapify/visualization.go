package shapify

import (
	"fmt"
	"image"
	"log"
	"path/filepath"

	"github.com/arl/evolve"
	"github.com/arl/evolve/pkg/bitstring"
	"github.com/fogleman/gg"
)

type folderOutput struct {
	folder   string // output folder
	every    int    // save the best image every nth generation
	renderer *renderer
}

func (fo *folderOutput) PopulationUpdate(data *evolve.PopulationData) {
	if data.GenNumber%fo.every == 0 {
		cand := data.BestCand.(*bitstring.Bitstring)

		img := fo.renderer.pool.Get().(*image.RGBA)
		fo.renderer.draw(img, cand)
		err := gg.SavePNG(filepath.Join(fo.folder, fmt.Sprintf("%d.png", data.GenNumber)), img)
		if err != nil {
			panic(err)
		}
		fo.renderer.pool.Put(img)
		log.Printf("Generation %d: fitness %f stddev: %f", data.GenNumber, data.BestFitness, data.StdDev)
	}
}
