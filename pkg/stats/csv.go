package stats

import (
	"encoding/csv"
	"fmt"
	"io"
	"os"
	"strconv"
	"time"

	"github.com/arl/evolve"
)

// SaveAsCSV saves the population samples in CSV format into the provided writer.
func SaveAsCSV(w io.Writer, samples []evolve.PopulationStats) error {
	csvw := csv.NewWriter(w)
	err := csvheader(csvw)
	if err != nil {
		return fmt.Errorf("stats.SaveAsCSV: %v", err)
	}

	var row [7]string
	for i := range samples {
		csvrow(row[:], &samples[i])
		err = csvw.Write(row[:])
		if err != nil {
			return fmt.Errorf("stats.SaveAsCSV: %v", err)
		}
	}

	csvw.Flush()
	err = csvw.Error()
	if err != nil {
		return fmt.Errorf("stats.SaveAsCSV: %v", err)
	}
	return nil
}

// SaveAsCSVFile saves the population samples in CSV format to the specified file.
func SaveAsCSVFile(path string, samples []evolve.PopulationStats) error {
	f, err := os.Create(path)
	if err != nil {
		return fmt.Errorf("stats.SaveAsCSVFile: can't create %s: %v", path, err)
	}
	defer f.Close()
	return SaveAsCSV(f, samples)
}

func csvheader(csvw *csv.Writer) error {
	return csvw.Write([]string{"elapsed (ms)", "best", "mean", "stddev", "size", "num_elites", "gen_number"})
}

func csvrow(row []string, stats *evolve.PopulationStats) {
	row[0] = strconv.FormatInt(int64(stats.Elapsed/time.Millisecond), 10)
	row[1] = strconv.FormatFloat(stats.BestFitness, 'f', -1, 64)
	row[2] = strconv.FormatFloat(stats.Mean, 'f', -1, 64)
	row[3] = strconv.FormatFloat(stats.StdDev, 'f', -1, 64)
	row[4] = strconv.FormatInt(int64(stats.Size), 10)
	row[5] = strconv.FormatInt(int64(stats.NumElites), 10)
	row[6] = strconv.FormatInt(int64(stats.GenNumber), 10)
}
