// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"gopl.io/ch5/links"
)

//!+sema
// tokens is a counting semaphore used to
// enforce a limit of 20 concurrent requests.
var tokens = make(chan struct{}, 20)

type Link struct {
	url   string
	depth int
}

func crawl(link Link, fileName string, limit int) []Link {

	f, err := os.OpenFile(fileName,
		os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Println(err)
	}
	if _, err := f.WriteString(link.url + "\n"); err != nil {
		log.Println(err)
	}
	f.Close()
	if link.depth < limit {
		tokens <- struct{}{} // acquire a token
		list, err := links.Extract(link.url)
		temp := make([]Link, len(list))
		for i, url := range list {
			temp[i] = Link{url: url, depth: link.depth + 1}
		}

		<-tokens // release the token

		if err != nil {
			log.Print(err)
		}
		return temp
	}
	return []Link{}
}

//!-sema

//!+
func main() {
	if len(os.Args) < 4 {
		fmt.Printf("Error wrong number of parameters\n")
		return
	}
	worklist := make(chan []Link)
	var n int // number of pending sends to worklist
	var depth = flag.Int("depth", 1, "crawling depth")
	var fileName = flag.String("results", "results.txt", "output file name")
	flag.Parse()
	fmt.Printf("depth = %d and filename = %s\n", *depth, *fileName)

	startingInput := os.Args[3:]

	//Creating the graph to limit depth
	currentLinks := make([]Link, len(startingInput))
	for i, url := range startingInput {
		currentLinks[i] = Link{url: url, depth: 0}
	}

	// Start with the command-line arguments.
	n++
	go func() { worklist <- currentLinks }()

	// Crawl the web concurrently.
	seen := make(map[string]bool)
	for ; n > 0; n-- {
		list := <-worklist
		for _, link := range list {
			if !seen[link.url] {
				seen[link.url] = true
				n++
				go func(link Link) {
					worklist <- crawl(link, *fileName, *depth)
				}(link)
			}
		}
	}
}

//!-
