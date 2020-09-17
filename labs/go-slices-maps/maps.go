package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	words := strings.Split(s, " ")
	lenOfWords := make(map[string]int)

	for i:=0; i<len(words);i++{
		lenOfWords[words[i]] += 1 
	}
	return lenOfWords
}

func main() {
	wc.Test(WordCount)
}