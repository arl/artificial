package shapify

import (
	"flag"
	"fmt"
	"image"
	"image/png"
	"os"
	"path/filepath"
	"reflect"
	"strings"
	"testing"

	"github.com/stretchr/testify/require"

	"github.com/arl/evolve/pkg/bitstring"
)

var update = flag.Bool("update", false, "update .golden files")

func Test_draw(t *testing.T) {
	tests := []struct {
		name  string
		bs    string
		ntris uint64
		want  image.Image
	}{
		{
			name: "one big green triangle", // gray code
			//      [                            1st triangle                           [          image  header        [
			//      [   y2  x2  [  y1  x1   [  y0  x0   [ triangle color (opaque green) [ bgcolor (opaque black)        [
			bs:    "1000000000000000001000000000000000001000000000000000100000000000000010000000000000000000000000000000",
			ntris: 1,
		},
		{
			name: "red and green alpha triangles", // gray-code,
			//     [                            2nd triangle                            [                            1st triangle                           [          image  header        [
			//     [    y2  x2  [  y1  x1   [  y0  x0   [            color              [   y2  x2  [   y1  x1  [   y0  x0  [             color             [ bgcolor (opaque black)        [
			bs:    "100000100000000000100000000000000000010000000000000000000000100000001000000000000000001000000000000000000100000000000000100000010000000010000000100000001000000010000000",
			ntris: 2,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			ind, err := bitstring.MakeFromString(tt.bs)
			require.NoError(t, err)

			e := eval{
				renderer: &renderer{
					cfg: Config{
						W:     64,
						H:     64,
						wbits: 6,
						hbits: 6,
						Ntris: tt.ntris,
					}}}

			img := image.NewRGBA(image.Rect(0, 0, 64, 64))
			e.draw(img, ind)

			// update and/or read golden file
			gf := filepath.Join("testdata", strings.ReplaceAll(tt.name, " ", "_")+".golden")
			if *update {
				f, err := os.Create(gf)
				require.NoErrorf(t, err, "can't create %s", gf)
				defer f.Close()
				t.Log("update .golden file", gf)

				err = png.Encode(f, img)
				require.NoErrorf(t, err, "failed to update .golden file %s", gf)
			}

			f, err := os.Open(gf)
			require.NoError(t, err, "failed reading .golden file")

			want, err := png.Decode(f)
			f.Close()
			require.NoError(t, err, "failed to decode .golden file")

			if !reflect.DeepEqual(img, want) {
				t.Errorf("draw() = %v, want %v", img, tt.want)
			}
		})
	}
}

func Test_diff(t *testing.T) {
	tests := []struct {
		ref, cand string  // path to images to diff
		want      float64 // expected diff
	}{
		{
			ref:  "one_big_green_triangle.golden",
			cand: "one_big_green_triangle.golden",
			want: 0,
		},
	}
	for i, tt := range tests {
		t.Run(fmt.Sprintf("%d", i), func(t *testing.T) {
			refimg, err := loadPNGRGBA(filepath.Join("testdata", tt.ref))
			require.NoError(t, err)
			candimg, err := loadPNGRGBA(filepath.Join("testdata", tt.cand))
			require.NoError(t, err)
			got := diff(refimg, candimg)
			require.Equal(t, tt.want, got)
		})
	}
}
