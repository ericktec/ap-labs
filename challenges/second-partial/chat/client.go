// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"os"
)

var (
	user string
)

//!+
func main() {

	var server string
	if len(os.Args) == 5 {
		if os.Args[1] == "-user" {
			user = os.Args[2]
		} else if os.Args[1] == "-server" {
			server = os.Args[2]
		} else {
			fmt.Println("Wrong flag")
			return
		}

		if os.Args[3] == "-user" {
			user = os.Args[4]
		} else if os.Args[3] == "-server" {
			server = os.Args[4]
		} else {
			fmt.Println("Wrong flag")
			return
		}
	} else {
		fmt.Println("Error wrong arguments")
		return
	}

	conn, err := net.Dial("tcp", server)
	if err != nil {
		log.Fatal(err)
	}
	_, e := io.WriteString(conn, user+"\n")
	if e != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		input := bufio.NewScanner(conn)
		for input.Scan() {
			fmt.Print("\n" + input.Text())
			fmt.Print("\n" + user + " > ")
		}
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-

func mustCopy(dst io.Writer, src io.Reader) {
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}

}
