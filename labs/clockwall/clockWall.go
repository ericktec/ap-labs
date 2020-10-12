// Netcat1 is a read-only TCP client.
package main

import (
	"fmt"
	"log"
	"net"
	"os"
	"strings"
)

func main() {
	if len(os.Args) == 1 {
		fmt.Println("Wrong number of parameters")
		os.Exit(1)
	}
	done := make(chan int)
	for i := 1; i < len(os.Args); i++ {
		currentInput := strings.Split(os.Args[i], "=")
		timeZone := currentInput[0]
		port := currentInput[1]
		conn, err := net.Dial("tcp", port)
		if err != nil {
			log.Fatal(err)
		}
		go handler(conn, timeZone, done)

	}
	<-done

}

func handler(conn net.Conn, timeZone string, done chan int) {
	for {
		temp := make([]byte, 12)

		if _, err := conn.Read(temp); err != nil {
			log.Fatal(err)
			conn.Close()
			return
		}
		fmt.Printf("%s: %s \n", timeZone, temp)

	}
	done <- 1

}
