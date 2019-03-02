package shapify

import (
	"fmt"
	"image"

	"github.com/arl/evolve/pkg/bitstring"
	"github.com/fogleman/gg"
)

type eval struct {
	*renderer
	img *image.RGBA // reference image
}

func newEval(renderer *renderer, cfg Config) (*eval, error) {
	e := &eval{renderer: renderer}
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
	img := e.renderer.pool.Get().(*image.RGBA)
	e.draw(img, cand)
	d := diff(e.img, img)
	e.renderer.pool.Put(img)
	return d
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
			c1 := img1.RGBAAt(x, y)
			c2 := img2.RGBAAt(x, y)

			rd := abs(int64(c1.R) - int64(c2.R))
			gd := abs(int64(c1.G) - int64(c2.G))
			bd := abs(int64(c1.B) - int64(c2.B))
			diff += rd + gd + bd
		}
	}
	return float64(diff)
}
