package shapify

import (
	"bytes"
	"fmt"
	"image"
	"image/png"
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

type imageOutput struct {
	every    int // save the best image every nth generation
	renderer *renderer
	imagecb  func([]byte)
	format   string
}

func (imo *imageOutput) Observe(stats *evolve.PopulationStats) {
	if stats.GenNumber%imo.every != 0 {
		return
	}
	cand := stats.BestCand.(*bitstring.Bitstring)
	img := imo.renderer.pool.Get().(*image.RGBA)
	defer imo.renderer.pool.Put(img)
	imo.renderer.draw(img, cand)

	var (
		buf []byte
		err error
	)
	switch imo.format {
	case "png":
		buf, err = encodePNG(img)
		if err != nil {
			log.Printf("imageOutput.Observer: can't encode to png: %v", err)
			return
		}
	default:
		log.Fatalf("imageOutput.Observer: unsupported format %s", imo.format)
	}
	imo.imagecb(buf)
}

func encodePNG(img *image.RGBA) ([]byte, error) {
	buf := bytes.Buffer{}
	err := png.Encode(&buf, img)
	if err != nil {
		return nil, err
	}
	return buf.Bytes(), nil
}
