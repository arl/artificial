package shapify

import (
	"math/rand"

	"github.com/arl/evolve/pkg/bitstring"
)

type mutation struct {
	pwholecolor float32 // probability to mutate a whole triangle
	ptriangle   float32 // probability to mutate triangle vertices
	pcolor      float32 // probability to a triangle color
	cuts        cutset
}

func (m *mutation) Apply(sel []interface{}, rng *rand.Rand) []interface{} {
	mutpop := make([]interface{}, len(sel))
	copy(mutpop, sel)
	for i := range mutpop {
		bs := mutpop[i].(*bitstring.Bitstring)
		if rng.Float32() < m.pwholecolor {
			cut := m.cuts.wholetris[rng.Intn(len(m.cuts.wholetris))]
			bs.FlipBit(cut.index + uint(rng.Intn(int(cut.length-1))))
		}
		if rng.Float32() < m.ptriangle {
			cut := m.cuts.tris[rng.Intn(len(m.cuts.tris))]
			bs.FlipBit(cut.index + uint(rng.Intn(int(cut.length-1))))
		}
		if rng.Float32() < m.pcolor {
			cut := m.cuts.colors[rng.Intn(len(m.cuts.colors))]
			bs.FlipBit(cut.index + uint(rng.Intn(int(cut.length-1))))
		}
	}
	return mutpop
}
