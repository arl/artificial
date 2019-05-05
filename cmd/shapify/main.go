package main

import (
	"fmt"
	"log"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"

	"github.com/spf13/pflag"
	"github.com/spf13/viper"

	"github.com/arl/artificial/pkg/shapify"
)

func exitIfErr(err error, format string, args ...string) {
	if err == nil {
		return
	}
	if len(args) == 0 {
		log.Fatalf("%s: %s", format, err)
	}
	log.Fatalf("%s: %s", fmt.Sprintf(format, args), err)
}

func main() {
	go func() {
		fmt.Println("listening on localhost:6060")
		log.Println(http.ListenAndServe("localhost:6060", nil))
	}()

	// port := pflag.String("port", "8080", "web gui server port")
	pflag.String("baseimage", "", "base image")
	pflag.Parse()
	viper.SetConfigName("config")
	viper.AddConfigPath(".")
	exitIfErr(viper.ReadInConfig(), "Error reading config file")
	viper.BindPFlags(pflag.CommandLine)

	var cfg shapify.Config
	exitIfErr(viper.Unmarshal(&cfg), "Unable to unmarshal config")

	cfg.W = 64
	cfg.H = 64
	exitIfErr(cfg.Setup(), "Invalid config file")
	log.Printf("config: %+v", cfg)
	stop, err := shapify.Shapify(cfg)
	exitIfErr(err, "Shapify error")

	var sig = make(chan os.Signal)
	signal.Notify(sig, syscall.SIGTERM)
	signal.Notify(sig, syscall.SIGINT)
	fmt.Printf("caught sig: %+v\n", <-sig)
	exitIfErr(stop(), "Shapify error")
}
