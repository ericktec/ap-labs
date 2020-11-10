package main

import (
	"fmt"
	"time"
)

func main() {
	waitingChannel := make(chan struct{})
	q := make(chan int)
	var i int64
	start := time.Now()
	go func() {
		q <- 1
		for {
			i++
			q <- <-q
		}
		waitingChannel <- struct{}{}
	}()
	go func() {
		for {
			i++
			q <- <-q
		}
		waitingChannel <- struct{}{}
	}()
	for {
		select {
		case <-time.After(1 * time.Second):
			fmt.Println("Communications Per Second : ", float64(i)/time.Since(start).Seconds())
		}

	}

	<-waitingChannel

}
