package shapify

import (
	"fmt"
	"image"

	"github.com/arl/evolve/pkg/bitstring"
	"github.com/fogleman/gg"
)

type eval struct {
	renderer
	img *image.RGBA // reference image
}

func newEval(cfg Config) (*eval, error) {
	e := &eval{renderer: renderer{cfg: cfg}}
	img, err := gg.LoadPNG(cfg.BaseImage)
	if err != nil {
		return nil, fmt.Errorf("can't load base image: %v", err)
	}
	imgrgba, ok := img.(*image.RGBA)
	if !ok {
		return nil, fmt.Errorf("base image is not image.RGBA but %T", img)
	}
	e.img = imgrgba
	return e, nil
}

func (e *eval) IsNatural() bool { return false }

func (e *eval) Fitness(icand interface{}, pop []interface{}) float64 {
	cand := icand.(*bitstring.Bitstring)
	candimg := e.draw(cand)
	return diff(e.img, candimg)
}

func abs(x int64) int64 {
	if x < 0 {
		return -x
	}
	return x
}

func diff(img1, img2 *image.RGBA) float64 {
	var (
		b    = img1.Bounds()
		w, h = b.Dx(), b.Dy()
		//off  int
		diff int64
	)

	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			r1, g1, b1, _ := img1.At(x, y).RGBA()
			r2, g2, b2, _ := img2.At(x, y).RGBA()

			rd := abs(int64(r1) - int64(r2))
			gd := abs(int64(g1) - int64(g2))
			bd := abs(int64(b1) - int64(b2))
			diff += rd + gd + bd

			//off = y*img1.Stride + x*4
			//diff += abs(int64(img1.Pix[off+0]) + int64(img1.Pix[off+1]) + int64(img1.Pix[off+2]) -
			//int64(img2.Pix[off+0]) + int64(img2.Pix[off+1]) + int64(img2.Pix[off+2]))
		}
	}
	return float64(diff)
}
