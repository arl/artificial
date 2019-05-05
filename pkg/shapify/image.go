package shapify

import (
	"fmt"
	"image"
	"sync"

	"github.com/fogleman/gg"

	"github.com/arl/evolve/pkg/bitstring"
)

func loadPNGRGBA(path string) (*image.RGBA, error) {
	img, err := gg.LoadPNG(path)
	if err != nil {
		return nil, err
	}
	rgba, ok := img.(*image.RGBA)
	if !ok {
		return nil, fmt.Errorf("loadPNGRGBA: image is not RGBA but %T", img)
	}
	return rgba, nil
}

type renderer struct {
	cfg  Config
	pool sync.Pool
}

func newRenderer(cfg Config) *renderer {
	return &renderer{
		cfg: cfg,
		pool: sync.Pool{
			New: func() interface{} {
				return image.NewRGBA(image.Rect(0, 0, cfg.W, cfg.H))
			},
		},
	}
}

func (r *renderer) draw(dst *image.RGBA, bs *bitstring.Bitstring) {
	dc := gg.NewContextForRGBA(dst)

	var ibit uint

	// fill background
	bgcol := extractColorNRGBA(bs, 0, nbpchannel)
	ibit += nbpcolor
	dc.SetColor(bgcol)
	dc.Fill()
	dc.Clear()

	for i := 0; i < int(r.cfg.Ntris); i++ {
		col := extractColorNRGBA(bs, ibit, nbpchannel)
		ibit += nbpcolor

		dc.SetColor(col)

		var (
			xs [3]float64
			ys [3]float64
		)
		for j := 0; j < 3; j++ {
			xs[j] = float64(bs.Grayn(r.cfg.wbits, ibit))
			ibit += r.cfg.wbits
			ys[j] = float64(bs.Grayn(r.cfg.hbits, ibit))
			ibit += r.cfg.hbits
		}
		dc.MoveTo(float64(xs[0]), float64(ys[0]))
		dc.LineTo(float64(xs[1]), float64(ys[1]))
		dc.LineTo(float64(xs[2]), float64(ys[2]))
		dc.Fill()
	}
	if ibit != uint(bs.Len()) {
		panic(fmt.Sprintf("only %d/%d bits have been used", ibit, bs.Len()))
	}
}
