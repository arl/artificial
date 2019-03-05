package shapify

import (
	"image/color"
	"math/rand"

	"github.com/arl/evolve/generator"
	"github.com/arl/evolve/pkg/bitstring"
)

type gen struct {
	cfg Config
}

/*
image:
- header
- triangles (cfg.Ntris)

header:
 - color: image background color

color (32 bits):
 - 8bits R
 - 8bits G
 - 8bits B
 - 8bits A

triangle:
 - color: triangle color
 - point * 3

point:
 - x: n bits with 2ⁿ = cfg.W
 - y: n bits with 2ⁿ = cfg.H
*/

const (
	nbpchannel = 8        // number of bits per color channel
	nbpcolor   = 8 * 4    // number of bits per color
	nbheader   = nbpcolor // number of bits in header
)

func totalBits(cfg Config) uint {
	var (
		nbpoint = cfg.hbits + cfg.wbits // number of bits per point
		nbtri   = 3*nbpoint + nbpcolor  // number of bits per triangle
	)
	return nbheader + uint(cfg.Ntris)*nbtri // number total of bits per image
}

func (g *gen) Generate(rnd *rand.Rand) interface{} {
	return generator.Bitstring(totalBits(g.cfg)).Generate(rnd)
}

func extractColorNRGBA(bs *bitstring.Bitstring, i, bitsPerChannel uint) color.NRGBA {
	return color.NRGBA{
		R: uint8(bs.Grayn(bitsPerChannel, i+bitsPerChannel*0)),
		G: uint8(bs.Grayn(bitsPerChannel, i+bitsPerChannel*1)),
		B: uint8(bs.Grayn(bitsPerChannel, i+bitsPerChannel*2)),
		A: uint8(bs.Grayn(bitsPerChannel, i+bitsPerChannel*3)),
	}
}
