package shapify

import (
	"errors"
	"fmt"
	"log"
	"math"

	"github.com/arl/evolve"
	"github.com/arl/evolve/condition"
	"github.com/arl/evolve/engine"
	"github.com/arl/evolve/operator"
	"github.com/arl/evolve/operator/mutation"
	"github.com/arl/evolve/operator/xover"
	"github.com/arl/evolve/selection"
)

// Config is the configuration for shapify
type Config struct {
	Ntris     uint64 // NTris is the number of triangles
	BaseImage string // Base is the path to the base image

	OrgW, OrgH   int  // base image original dimensions
	W, H         int  // base image scaled down dimensions
	wbits, hbits uint // number of bits in image width and height
}

// pow2roundup rounds n up to the next higher power of 2, or n if it is already a power of 2.
func pow2roundup(x int) int {
	if x <= 1 {
		return 1
	}
	x--
	x |= x >> 1
	x |= x >> 2
	x |= x >> 4
	x |= x >> 8
	x |= x >> 16
	return x + 1
}

// Setup vets the values in the config structure and pre-compute some necessary configuration fields.
// It returns an error in case something is either not right or not supported.
func (c *Config) Setup() error {
	if pow2roundup(int(c.W)) != int(c.W) || pow2roundup(int(c.H)) != int(c.H) {
		return errors.New("scaled width and height must be power of 2")
	}
	c.wbits = uint(math.Log2(float64(c.W)))
	c.hbits = uint(math.Log2(float64(c.H)))
	return nil
}

// Shapify ...
func Shapify(cfg Config) error {
	// define the crossover
	xover := xover.New(xover.BitstringMater{})
	err := xover.SetPoints(1)
	if err != nil {
		return err
	}
	err = xover.SetProb(0.7)
	if err != nil {
		return err
	}

	// define the mutation
	mut := mutation.NewBitstring()
	err = mut.SetProb(0.01)
	if err != nil {
		return err
	}

	evaluator, err := newEval(cfg)
	if err != nil {
		return err
	}

	epocher := engine.Generational{
		Op:   operator.Pipeline{xover, mut},
		Eval: evaluator,
		Sel:  selection.RouletteWheel,
	}
	gen := &gen{cfg: cfg}
	eng, err := engine.New(gen, evaluator, &epocher)
	if err != nil {
		return err
	}

	eng.AddObserver(
		engine.ObserverFunc(func(data *evolve.PopulationData) {
			log.Printf("Generation %d: %s (%v)\n",
				data.GenNumber,
				data.BestCand,
				data.BestFitness)
		}))

	bests, _, err := eng.Evolve(
		1,                // number of individuals per generation
		engine.Elites(0), // nth best are put directly put into next population
		engine.EndOn(condition.TargetFitness{
			Fitness: 0,
			Natural: evaluator.IsNatural(),
		}),
	)

	if err != nil {
		return err
	}
	fmt.Println(bests[0])
	return nil
}
