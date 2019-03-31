package shapify

import (
	"encoding/base64"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"golang.org/x/net/websocket"
)

type gui struct {
	html []byte
}

func newGUI() (*gui, error) {
	buf, err := ioutil.ReadFile("./pkg/shapify/index.html")
	if err != nil {
		return nil, err
	}
	g := &gui{html: buf}
	return g, nil
}

func (g *gui) start(plotsc, imagesc <-chan []byte) {
	eh := eventHandler{
		plotsc:  plotsc,
		imagesc: imagesc,
	}
	http.HandleFunc("/", g.home)
	http.Handle("/data", websocket.Handler(eh.handle))
}

// home handles the shapify gui page.
func (g *gui) home(w http.ResponseWriter, r *http.Request) {
	w.Write(g.html)
}

type plotHandler struct {
	plotsc chan []byte
}

func newPlotHandler() *plotHandler {
	return &plotHandler{
		plotsc: make(chan []byte),
	}
}

type PlotUpdate struct {
	EventType
	Plot string `json:"plot"`
}

type eventHandler struct {
	plotsc, imagesc <-chan []byte
}

func (h *eventHandler) handle(ws *websocket.Conn) {
	fmt.Println("plotHandler")

	// TODO: how to exit this loop
	for {
		var (
			evt interface{}
			buf []byte
		)
		select {
		case buf = <-h.plotsc:
			evt = PlotUpdate{
				EventType: EventType{
					Type: plotEventType,
				},
				Plot: string(buf),
			}
		case buf = <-h.imagesc:
			evt = ImageUpdate{
				EventType: EventType{
					Type: imageEventType,
				},
				Image: base64.StdEncoding.EncodeToString(buf),
			}
		}
		err := websocket.JSON.Send(ws, evt)
		if err != nil {
			log.Printf("eventHandler.handler: error sending data: %v", err)
			return
		}
	}
}

type imageHandler struct {
	imgsc chan []byte
}

func newImageHandler() *imageHandler {
	return &imageHandler{
		imgsc: make(chan []byte),
	}
}

const (
	plotEventType  = "plot"
	imageEventType = "image"
)

type EventType struct {
	Type string `json:"type"`
}

type ImageUpdate struct {
	EventType
	Image string `json:"image"`
}
