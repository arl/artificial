package shapify

import (
	"flag"
	"image"
	"image/png"
	"os"
	"path/filepath"
	"reflect"
	"strings"
	"testing"

	"github.com/arl/evolve/pkg/bitstring"
	"github.com/stretchr/testify/require"
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
			name: "one big green triangle",
			//      [                            1st triangle                           [          image  header        [
			//      [   y2  x2  [  y1  x1   [  y0  x0   [ triangle color (opaque green) [ bgcolor (opaque black)        [
			bs:    "1111110000000000001111110000000000001111111100000000111111110000000011111111000000000000000000000000",
			ntris: 1,
		},
		{
			name: "red and green alpha triangles",
			//     [                            2nd triangle                            [                            1st triangle                           [          image  header        [
			//     [ y2  x2     [  y1  x1   [  y0  x0   [            color              [   y2  x2  [   y1  x1  [   y0  x0  [             color             [ bgcolor (opaque black)        [
			bs:    "111111111111000000111111000000000000011111110000000000000000111111111111110000000000001111110000000000000111111100000000111111110000000011111111111111111111111111111111",
			ntris: 2,
		},
	}
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			ind, err := bitstring.MakeFromString(tt.bs)
			require.NoError(t, err)

			e := eval{
				cfg: Config{
					W:     64,
					H:     64,
					wbits: 6,
					hbits: 6,
					Ntris: tt.ntris,
				},
			}

			got := e.draw(ind)

			// update and/or read golden file
			gf := filepath.Join("testdata", strings.ReplaceAll(tt.name, " ", "_")+".golden")
			if *update {
				f, err := os.Create(gf)
				require.NoErrorf(t, err, "can't create %s", gf)
				defer f.Close()
				t.Log("update .golden file", gf)

				err = png.Encode(f, got)
				require.NoErrorf(t, err, "failed to update .golden file %s", gf)
			}

			f, err := os.Open(gf)
			require.NoError(t, err, "failed reading .golden file")
			defer f.Close()
			want, err := png.Decode(f)
			require.NoError(t, err, "failed to decode .golden file")

			if !reflect.DeepEqual(got, want) {
				t.Errorf("draw() = %v, want %v", got, tt.want)
			}
		})
	}
}
