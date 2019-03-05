package shapify

import (
	"math/rand"

	"github.com/arl/evolve/pkg/bitstring"
)

type bitrange struct{ index, length uint }

// ranges of bitranges when the bitstring codes for:
// a background color + colored triangles
type trianglesRanges struct {
	wholetris []bitrange // whole triangles (color + vertices)
	tris      []bitrange // triangles (vertices only)
	colors    []bitrange // colors (background + triangles)
}

func (tr *trianglesRanges) set(cfg Config) {
	nbpoint := cfg.hbits + cfg.wbits // number of bits per point
	nbtri := 3*nbpoint + nbpcolor    // number of bits per triangle

	// whole triangle cuts (color included)
	for i := uint(0); i < uint(cfg.Ntris); i++ {
		tr.wholetris = append(tr.wholetris, bitrange{nbheader + i*nbtri, nbtri})
	}

	// triangle cuts (3 vertices only)
	for i := uint(0); i < uint(cfg.Ntris); i++ {
		tr.tris = append(tr.tris, bitrange{nbheader + i*nbtri + nbpcolor, 3 * nbpoint})
	}

	// color cuts
	tr.colors = append(tr.colors, bitrange{0, nbheader}) // background color (whole header)
	for i := uint(0); i < uint(cfg.Ntris); i++ {
		tr.colors = append(tr.colors, bitrange{nbheader + i*nbtri, nbpcolor})
	}
}

type mater struct {
	cutset []bitrange // only set of points where cuts can happen
}

func newMater(tr *trianglesRanges) *mater {
	cutset := append(tr.wholetris, tr.tris...)
	cutset = append(cutset, tr.colors...)
	return &mater{
		cutset: cutset,
	}
}

func (m *mater) Mate(parent1, parent2 interface{}, nxpts int64,
	rng *rand.Rand) []interface{} {

	p1, p2 := parent1.(*bitstring.Bitstring), parent2.(*bitstring.Bitstring)
	off1 := bitstring.Copy(p1)
	off2 := bitstring.Copy(p2)

	for i := int64(0); i < nxpts; i++ {
		cut := m.cutset[rng.Intn(len(m.cutset))] // select a cut
		bitstring.SwapRange(off1, off2, cut.index, cut.length)
	}
	return []interface{}{off1, off2}
}
