package shapify

import (
	"fmt"
	"image"
	"image/png"
	"os"

	"github.com/arl/evolve/pkg/bitstring"
	"github.com/fogleman/gg"
)

type eval struct {
	cfg    Config
	img    *image.RGBA
	ncands int
}

func newEval(cfg Config) (*eval, error) {
	e := &eval{cfg: cfg}
	f, err := os.Open(cfg.BaseImage)
	if err != nil {
		return e, fmt.Errorf("can't open base image %s: %s", cfg.BaseImage, err)
	}
	defer f.Close()

	img, err := png.Decode(f)
	if err != nil {
		return e, fmt.Errorf("can't decode image %s: %s", cfg.BaseImage, err)
	}
	var ok bool
	e.img, ok = img.(*image.RGBA)
	if !ok {
		return e, fmt.Errorf("wrong image format %T", img)
	}

	return e, nil
}

func (e *eval) IsNatural() bool { return false }

func (e *eval) Fitness(icand interface{}, pop []interface{}) float64 {
	cand := icand.(*bitstring.Bitstring)
	candimg := e.draw(cand)
	return diff(e.img, candimg)
}

func (e *eval) draw(bs *bitstring.Bitstring) *image.RGBA {
	// Initialize the graphic context on an RGBA image
	dc := gg.NewContext(e.cfg.W, e.cfg.H)

	var ibit uint

	// fill background
	bgcol := extractColorNRGBA(bs, 0, nbpchannel)
	ibit += nbpcolor
	dc.SetColor(bgcol)
	dc.Fill()
	dc.Clear()

	for i := 0; i < int(e.cfg.Ntris); i++ {
		col := extractColorNRGBA(bs, ibit, nbpchannel)
		ibit += nbpcolor

		dc.SetColor(col)

		var (
			xs [3]float64
			ys [3]float64
		)
		for j := 0; j < 3; j++ {
			xs[j] = float64(bs.Uintn(e.cfg.wbits, ibit))
			ibit += e.cfg.wbits
			ys[j] = float64(bs.Uintn(e.cfg.hbits, ibit))
			ibit += e.cfg.hbits
		}
		dc.MoveTo(float64(xs[0]), float64(ys[0]))
		dc.LineTo(float64(xs[1]), float64(ys[1]))
		dc.LineTo(float64(xs[2]), float64(ys[2]))
		dc.Fill()
	}
	if ibit != uint(bs.Len()) {
		panic(fmt.Sprintf("only %d/%d bits have been used", ibit, bs.Len()))
	}

	//dc.SavePNG(fmt.Sprintf("test-%d.png", e.ncands))
	e.ncands++
	return dc.Image().(*image.RGBA)
}

func diff(img1, img2 *image.RGBA) float64 {
	return 1
}
