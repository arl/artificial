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
		diff int64
	)

	var off int
	for y := 0; y < h; y++ {
		off = y * img1.Stride
		for x := 0; x < w; x++ {
			s1 := img1.Pix[off : off+4 : off+4] // Small cap improves performance, see https://golang.org/issue/27857
			s2 := img2.Pix[off : off+4 : off+4]

			rd := abs(int64(s1[0]) - int64(s2[0]))
			gd := abs(int64(s1[1]) - int64(s2[1]))
			bd := abs(int64(s1[2]) - int64(s2[2]))

			// sum of squared differences
			diff += rd*rd + gd*gd + bd*bd
			off += 4
		}
	}
	return float64(diff)
}
