package main

import (
	"fmt"
	"time"
)

var waitingChannel = make(chan struct{})

var startingTime = time.Now()

func pipeline(in chan struct{}, out chan struct{}, current int, limit int) {
	if current <= limit {
		fmt.Println("Maximum number of pipeline stages   : ", current)
		fmt.Println("Time to transit trough the pipeline : ", time.Since(startingTime))
		go pipeline(out, in, current+1, limit)
		out <- <-in
	} else {
		waitingChannel <- struct{}{}
	}
}

func main() {
	var in chan (struct{})
	var out chan (struct{})
	go pipeline(in, out, 0, 10000000)
	<-waitingChannel

}
