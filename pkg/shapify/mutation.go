package shapify

import (
	"math/rand"

	"github.com/arl/evolve/pkg/bitstring"
)

type mutation struct {
	pwholecolor float32 // probability to mutate a whole triangle
	ptriangle   float32 // probability to mutate triangle vertices
	pcolor      float32 // probability to a triangle color
	ranges      *trianglesRanges
}

func (m *mutation) Apply(sel []interface{}, rng *rand.Rand) []interface{} {
	mutpop := make([]interface{}, len(sel))
	copy(mutpop, sel)
	for i := range mutpop {
		bs := mutpop[i].(*bitstring.Bitstring)
		if rng.Float32() < m.pwholecolor {
			cut := m.ranges.wholetris[rng.Intn(len(m.ranges.wholetris))]
			bs.FlipBit(cut.index + uint(rng.Intn(int(cut.length-1))))
		}
		if rng.Float32() < m.ptriangle {
			cut := m.ranges.tris[rng.Intn(len(m.ranges.tris))]
			bs.FlipBit(cut.index + uint(rng.Intn(int(cut.length-1))))
		}
		if rng.Float32() < m.pcolor {
			cut := m.ranges.colors[rng.Intn(len(m.ranges.colors))]
			bs.FlipBit(cut.index + uint(rng.Intn(int(cut.length-1))))
		}
	}
	return mutpop
}
