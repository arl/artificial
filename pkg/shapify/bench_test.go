package shapify

import (
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/require"
)

var sink interface{}

func Benchmark_diff(b *testing.B) {
	path := filepath.Join("testdata", "one_big_green_triangle.golden")

	img1, err := loadPNGRGBA(path)
	require.NoError(b, err)
	img2, err := loadPNGRGBA(path)
	require.NoError(b, err)

	var val float64

	b.ResetTimer()
	b.ReportAllocs()
	for i := 0; i < b.N; i++ {
		val = diff(img1, img2)
	}
	b.StopTimer()
	sink = val
}
