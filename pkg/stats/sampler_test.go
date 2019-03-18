package stats

import (
	"fmt"
	"testing"

	"github.com/stretchr/testify/assert"

	"github.com/arl/evolve"
)

func TestDownsampler_decimate(t *testing.T) {
	tests := []struct {
		nsamples int
		want     []int
	}{
		{nsamples: 2, want: []int{0}},
		{nsamples: 3, want: []int{0}},
		{nsamples: 4, want: []int{0, 2}},
		{nsamples: 10, want: []int{0, 2, 4, 6, 8}},
	}
	for _, tt := range tests {
		t.Run(fmt.Sprintf("nsamples=%d", tt.nsamples), func(t *testing.T) {
			sb := Downsampler{buf: genSamples(tt.nsamples)}
			sb.decimate()
			assert.EqualValues(t, tt.want, intSlice(sb.buf))
		})
	}
}

func TestDownsamplerSampling(t *testing.T) {
	tests := []struct {
		nsamples int
		maxpts   int
		want     []int
	}{
		{
			maxpts:   1,
			nsamples: 2,
			want:     []int{0},
		},
		{
			maxpts:   10,
			nsamples: 19,
			want:     []int{0, 1, 3, 5, 7, 9, 11, 13, 15, 17},
		},
		{
			maxpts:   10,
			nsamples: 20,
			want:     []int{0, 2, 4, 6, 8, 10, 12, 14, 16, 18},
		},
		{
			maxpts:   10,
			nsamples: 21,
			want:     []int{0, 2, 4, 6, 8, 10, 12, 14, 16, 18},
		},
		{
			maxpts:   10,
			nsamples: 100,
			want:     []int{0, 8, 16, 24, 40, 48, 56, 72, 80, 88},
		},
	}
	for _, tt := range tests {
		name := fmt.Sprintf("nsamples=%d,maxpts=%d", tt.nsamples, tt.maxpts)
		t.Run(name, func(t *testing.T) {
			sb := NewDownsampler(tt.maxpts)
			for i := 0; i < tt.nsamples; i++ {
				sb.addSample(dummySample(i))
			}
			got := make([]evolve.PopulationStats, tt.maxpts)
			n := sb.Samples(got)
			assert.EqualValues(t, tt.want, intSlice(got[:n]))
		})
	}
}

func genSamples(n int) []evolve.PopulationStats {
	buf := make([]evolve.PopulationStats, n)
	for i := 0; i < n; i++ {
		buf[i] = *dummySample(i)
	}
	return buf
}

func dummySample(gen int) *evolve.PopulationStats {
	return &evolve.PopulationStats{GenNumber: gen}
}

func intSlice(buf []evolve.PopulationStats) []int {
	ints := make([]int, len(buf))
	for i := range buf {
		ints[i] = buf[i].GenNumber
	}
	return ints
}
