package shapify

import (
	"fmt"
	"image"

	"github.com/arl/evolve/pkg/bitstring"
	"github.com/fogleman/gg"
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
	cfg Config
}

func (r *renderer) draw(bs *bitstring.Bitstring) *image.RGBA {
	// Initialize the graphic context on an RGBA image
	// dst := image.NewRGBA(image.Rect(0, 0, e.cfg.W, e.cfg.H))
	// dc := gg.NewContextForRGBA(dst)
	dc := gg.NewContext(r.cfg.W, r.cfg.H)

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
			xs[j] = float64(bs.Uintn(r.cfg.wbits, ibit))
			ibit += r.cfg.wbits
			ys[j] = float64(bs.Uintn(r.cfg.hbits, ibit))
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

	return dc.Image().(*image.RGBA)
}
