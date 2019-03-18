package stats

import "github.com/arl/evolve"

// A Downsampler keeps track of the evolution of population statistics
// over time by keeping a maximum number of samples in its underlying
// buffer. When the number of samples exceeds maxsamples, a minimum
// number of evenly spaced samples are decimated.
// It implements the evolve.Observer interface.
type Downsampler struct {
	buf        []evolve.PopulationStats
	n          int // total number of downsampled item that buf represents
	invres     int // current inverse sampling resolution
	maxsamples int // maximum number of samples
}

// NewDownsampler creates a downsampler that keeps tracks of the specified
// number of samples.
func NewDownsampler(maxsamples int) *Downsampler {
	return &Downsampler{
		// use twice the size to smooth decimation
		buf:        make([]evolve.PopulationStats, 0, maxsamples*2),
		invres:     1,
		maxsamples: maxsamples,
	}
}

// Observe observes evolution of population statistics, by being notified with
// population statistics after each completed epoch.
func (sb *Downsampler) Observe(stats *evolve.PopulationStats) {
	sb.addSample(stats)
}

func (sb *Downsampler) addSample(stats *evolve.PopulationStats) {
	if sb.n == sb.invres*cap(sb.buf) {
		sb.decimate()
	}
	// only keep 1 sample out of 'invres', discard the rest.
	if sb.n%sb.invres == 0 {
		sb.buf = append(sb.buf, *stats)
	}
	sb.n++
}

// decimate divides the number of samples by 2, thus doubling the sampling
// resolution.
func (sb *Downsampler) decimate() {
	for i := 0; i < len(sb.buf); i++ {
		if i&1 != 1 {
			sb.buf[i/2] = sb.buf[i]
		}
	}
	sb.buf = sb.buf[:len(sb.buf)/2] // reslice
	sb.invres *= 2
}

// Samples fills the provided buffer with a maximum of maxsamples samples.
// buf must be big enough or samples will panic. Returns the number n of
// samples that were filled in buf, 0 <= n <= maxsamples.
func (sb *Downsampler) Samples(buf []evolve.PopulationStats) (n int) {
	if sb.n < sb.maxsamples {
		copy(buf, sb.buf[:sb.n])
		return sb.n
	}
	for i := 0; i < sb.maxsamples; i++ {
		buf[i] = sb.buf[i*len(sb.buf)/sb.maxsamples]
	}
	return sb.maxsamples
}
