package shapify

import (
	"math/rand"

	"github.com/arl/evolve/pkg/bitstring"
)

type mater struct {
	cuts cutset
}

func (m *mater) Mate(parent1, parent2 interface{}, nxpts int64, rng *rand.Rand) []interface{} {
	p1, p2 := parent1.(*bitstring.Bitstring), parent2.(*bitstring.Bitstring)
	off1 := bitstring.Copy(p1)
	off2 := bitstring.Copy(p2)

	for i := int64(0); i < nxpts; i++ {
		cut := cut{}
		switch rng.Intn(3) {
		case 0: // whole triangles
			cut = m.cuts.wholetris[rng.Intn(len(m.cuts.wholetris))]
		case 1: // triangle vertices
			cut = m.cuts.tris[rng.Intn(len(m.cuts.tris))]
		case 2: // colors (background or triangle)
			cut = m.cuts.colors[rng.Intn(len(m.cuts.colors))]
		}
		bitstring.SwapRange(off1, off2, cut.index, cut.length)
	}
	return []interface{}{off1, off2}
}
