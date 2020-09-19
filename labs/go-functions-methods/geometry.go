// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"fmt"
	"math"
	"math/rand"
	"os"
	"strconv"
	"time"
)

type Point struct{ x, y float64 }

func (p *Point) SetX(x float64) {
	p.x = x
}

func (p *Point) SetY(y float64) {
	p.y = y
}

func (p Point) GetX() float64 {
	return p.x
}

func (p Point) GetY() float64 {
	return p.y
}

func newPoint() Point {
	rand.Seed(time.Now().UnixNano())
	newPoint := Point{}
	newPoint.SetX(rand.Float64()*(101+101) - 101)
	newPoint.SetY(rand.Float64()*(101+101) - 101)
	return newPoint
}

func onSegment(p, q, r Point) bool {
	if q.x <= math.Max(p.x, r.x) && q.x >= math.Min(p.x, r.x) && q.y <= math.Max(p.y, r.y) && q.y >= math.Min(p.y, r.y) {
		return true
	}
	return false
}

func orientation(p, q, r Point) int {
	val := (q.y-p.y)*(r.x-q.x) - (q.x-p.x)*(r.y-q.y)

	if val == 0 {
		return 0
	}
	if val > 0 {
		return 1
	}
	return 2

}

func doIntersect(p1, q1, p2, q2 Point) bool {
	// Find the four orientations needed for general and
	// special cases
	o1 := orientation(p1, q1, p2)
	o2 := orientation(p1, q1, q2)
	o3 := orientation(p2, q2, p1)
	o4 := orientation(p2, q2, q1)

	// General case
	if o1 != o2 && o3 != o4 {
		return true
	}
	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if o1 == 0 && onSegment(p1, p2, q1) {
		return true
	}

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1
	if o2 == 0 && onSegment(p1, q2, q1) {
		return true
	}

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if o3 == 0 && onSegment(p2, p1, q2) {
		return true
	}

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if o4 == 0 && onSegment(p2, q1, q2) {
		return true
	}

	return false // Doesn't fall in any of the above cases
}

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.GetX()-p.GetX(), q.GetY()-p.GetY())
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.GetX()-p.GetX(), q.GetY()-p.GetY())
}

//!-point

//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	sum += path[0].Distance(path[len(path)-1])
	return sum
}

func validateFigure(path Path) bool {
	//Return true if the figure is valid
	for i := 0; i < len(path); i++ {
		if doIntersect(path[i%len(path)], path[(i+1)%len(path)], path[(i+2)%len(path)], path[(i+3)%len(path)]) == true {
			return false
		}
	}
	for i := 1; i < len(path)-3; i++ {
		if doIntersect(path[len(path)-1], path[0], path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	for i := 0; i < len(path)-4; i++ {
		if doIntersect(path[len(path)-1], path[len(path)-2], path[(i)], path[(i+1)]) == true {
			return false
		}
	}
	return true
}

func createFigure(nPoints int) Path {
	var path Path

	for i := 0; i < nPoints; i++ {
		path = append(path, newPoint())
		if i == 2 {
			for orientation(path[0], path[1], path[2]) == 0 {
				path[2] = newPoint()
			}
		} else if i > 2 {
			for validateFigure(path) == false {
				path[i] = newPoint()
			}
		}

	}
	return path
}

func printFigure(path Path) {
	for i := 0; i < len(path); i++ {
		fmt.Printf("   - (  %f,   %f)\n", path[i].GetX(), path[i].GetY())
	}
}

func printDistance(path Path) {
	for i := range path {
		if i > 0 && i+1 < len(path) {
			fmt.Printf("+ %f", path[i].Distance(path[i+1]))
		} else if i == 0 {
			fmt.Printf("%f", path[i].Distance(path[i+1]))
		}
	}
	fmt.Printf("+ %f = %f\n", path[0].Distance(path[len(path)-1]), path.Distance())
}

func main() {
	args := os.Args[1:]
	fmt.Println(args)

	if len(args) < 1 || len(args) > 1 {
		fmt.Println("Bad arguments")
		return
	}
	numberOfRandoms, err := strconv.Atoi(args[0])
	if err != nil {
		fmt.Println("Error converting the string to number")
		return
	} else if numberOfRandoms < 3 {
		fmt.Println("It's not possible to create a figure with just two points")
		return
	}

	fmt.Printf("- Generating a [%d] sides figure\n", numberOfRandoms)
	fmt.Println("- Figure's vertices")
	figure := createFigure(numberOfRandoms)
	printFigure(figure)
	fmt.Println("- Figure's Perimeter")
	printDistance(figure)

	//   - 5.38 + 3.60 + 3.92 + 3.80 = 16.70

}

//!-path
