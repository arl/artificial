package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	_ "net/http/pprof"
	"os"
	"os/signal"
	"syscall"

	"gopkg.in/yaml.v3"

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

func readConfig(name string) shapify.Config {
	buf, err := ioutil.ReadFile(name)
	exitIfErr(err, "can't read configuration file '%s'", name)

	var cfg shapify.Config
	err = yaml.Unmarshal(buf, &cfg)
	exitIfErr(err, "can't decode configuration")

	log.Printf("config: %+v", cfg)
	exitIfErr(cfg.Setup(), "invalid shapify configuration file")
	return cfg
}

func main() {
	addr := "localhost:6061"
	cfgf := "./shapify.cfg"
	flag.StringVar(&addr, "addr", addr, "web gui http address")
	flag.StringVar(&cfgf, "cfg", cfgf, "shapify configuration file")
	flag.Parse()

	// start web gui server
	go func() {
		fmt.Println("listening on", addr)
		log.Fatalln(http.ListenAndServe(addr, nil))
	}()

	stop, err := shapify.Shapify(readConfig(cfgf))
	exitIfErr(err, "shapify error")

	var sig = make(chan os.Signal)
	signal.Notify(sig, syscall.SIGTERM)
	signal.Notify(sig, syscall.SIGINT)
	fmt.Printf("caught signal: %+v\n", <-sig)
	exitIfErr(stop(), "shapify error")
}
