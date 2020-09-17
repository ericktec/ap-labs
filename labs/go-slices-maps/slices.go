package main


import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	slice := make([][]uint8, dy)
	for i:= range slice{
		slice[i] = make([]uint8,dx)
		for j:= range slice[i] {
			slice[i][j] = uint8(i*j)
		}
	}
	return slice
}

func main() {
	pic.Show(Pic)
}

