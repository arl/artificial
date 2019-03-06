package shapify

import (
	"math/rand"

	"github.com/arl/evolve/pkg/bitstring"
)

type mater struct {
	cutset []cut // only set of points where cuts can happen
}

func newMater(cs *cutset) *mater {
	// place all cuts of various types in a same slice, effectively
	// giving each of them the same probability to 'happen'.
	return &mater{
		cutset: append(append(cs.wholetris, cs.tris...), cs.colors...),
	}
}

func (m *mater) Mate(parent1, parent2 interface{}, nxpts int64, rng *rand.Rand) []interface{} {
	p1, p2 := parent1.(*bitstring.Bitstring), parent2.(*bitstring.Bitstring)
	off1 := bitstring.Copy(p1)
	off2 := bitstring.Copy(p2)

	for i := int64(0); i < nxpts; i++ {
		cut := m.cutset[rng.Intn(len(m.cutset))] // select a cut
		bitstring.SwapRange(off1, off2, cut.index, cut.length)
	}
	return []interface{}{off1, off2}
}
