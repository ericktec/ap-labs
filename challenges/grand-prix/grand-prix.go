package main

import (
	"flag"
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"sort"
	"sync"
	"time"
)

/*

 ██████╗ ██████╗  █████╗ ███╗   ██╗██████╗     ██████╗ ██████╗ ██╗██╗  ██╗
██╔════╝ ██╔══██╗██╔══██╗████╗  ██║██╔══██╗    ██╔══██╗██╔══██╗██║╚██╗██╔╝
██║  ███╗██████╔╝███████║██╔██╗ ██║██║  ██║    ██████╔╝██████╔╝██║ ╚███╔╝
██║   ██║██╔══██╗██╔══██║██║╚██╗██║██║  ██║    ██╔═══╝ ██╔══██╗██║ ██╔██╗
╚██████╔╝██║  ██║██║  ██║██║ ╚████║██████╔╝    ██║     ██║  ██║██║██╔╝ ██╗
 ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝     ╚═╝     ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝


*/
type Car struct {
	id        int
	positionX int
	positionY int
	maxSpeed  int
	rank      int
	laps      int
	raceTime  string
	finished  bool
	//emoji     string
	// "\U0001f3ce\ufe0f"
}

var laps int

var players []Car

var board [6][90]int

var wg sync.WaitGroup

var startTime time.Time

func main() {
	// usage: ./grand-prix -players 20 -laps 78
	rand.Seed(time.Now().UnixNano())
	var lapsFlag = flag.Int("laps", 1, "Laps that game is going to run")
	var numberPlayers = flag.Int("players", 3, "Players needed to run the game")
	flag.Parse()
	laps = *lapsFlag
	fmt.Println(" LAPS FLAG = ", *lapsFlag)
	ranks := make(chan Car, *numberPlayers)
	fmt.Printf(" Laps = %d, Racers = %d \n", laps, *numberPlayers)
	addPlayers(*numberPlayers)
	play(ranks)

	var winners []int
	close(ranks)
	place := 1
	for car := range ranks {
		fmt.Printf(" El %d° puesto es %d\n", place, car.id)
		winners = append(winners, car.id)
		place++
	}

	printPodium(winners)

}

func addPlayers(numberPlayers int) {
	var row, column int = -1, 0
	for i := 0; i < numberPlayers; i++ {
		if (i % len(board)) == 0 {
			row++
			column = 0
		}
		var newCar Car
		newCar.id = i + 1
		newCar.laps = 0
		newCar.maxSpeed = rand.Intn(50) + 1
		newCar.rank = 0
		newCar.positionX = row
		newCar.positionY = column
		newCar.finished = false
		newCar.raceTime = ""
		fmt.Println("Player ", i, "coordinates y:", column, "x: ", row)
		board[column][row] = newCar.id
		players = append(players, newCar)
		column++
	}
}

func play(ranks chan Car) {
	startTime = time.Now()
	for !gameOver(ranks) {
		for i := 0; i < len(players); i++ {
			go carMovement(&players[i], ranks)
			wg.Add(1)
		}
		time.Sleep(time.Second * 1)
		wg.Wait()

		printGame()

	}

}

func carMovement(car *Car, ranks chan Car) {

	// fmt.Printf("PLAYER %d -> LAPS: %d/%d POSITION: %d %d  SPEED:  TIME: %s \n", car.id, car.laps, laps, car.positionX, car.positionY, transitTime)
	if !car.finished {
		currentPositionX := car.positionX
		currentPositionY := car.positionY
		newPositionY := car.positionY
		if board[car.positionY][(car.positionX+1)%(len(board[0])-1)] != 0 {
			newPositionY = changeLane(currentPositionX, currentPositionY)
		}
		newPositionX := currentPositionX + rand.Intn(car.maxSpeed) + 1

		newPositionX = carCollision(currentPositionX, newPositionY, newPositionX)

		if newPositionX > (len(board[0]) - 1) {
			if car.laps < laps {
				car.laps++
			}
			newPositionX = newPositionX % (len(board[0]) - 1)
			if car.laps == laps {
				car.finished = true
				if car.finished {
					board[car.positionY][car.positionX] = 0
					car.positionX = -1
					car.positionY = -1
					ranks <- *car
					wg.Done()
					return

				}

			}
		}

		board[currentPositionY][currentPositionX] = 0
		car.positionX = newPositionX
		car.positionY = newPositionY
		if car.laps < laps {
			car.raceTime = time.Since(startTime).String()
		}

		board[newPositionY][newPositionX] = car.id

	}
	wg.Done()

}

func carCollision(currentPositionX int, currentPositionY int, newPositionX int) int {
	for i := currentPositionX; i < newPositionX; i++ {

		if board[currentPositionY][(i+1)%(len(board[0])-1)] != 0 {

			return i
		}
	}
	return newPositionX
}

func changeLane(currentPositionX int, currentPositionY int) int {
	if currentPositionY > 0 && currentPositionY < (len(board)-1) {
		if board[currentPositionY+1][currentPositionX] == 0 {
			return (currentPositionY + 1)
		} else if board[currentPositionY-1][currentPositionX] == 0 {
			return (currentPositionY - 1)
		}
	} else if currentPositionY == 0 {
		if board[currentPositionY+1][currentPositionX] == 0 {
			return (currentPositionY + 1)
		}
	} else if currentPositionY == (len((board)) - 1) {
		if board[currentPositionY-1][currentPositionX] == 0 {
			return (currentPositionY - 1)
		}
	}
	return currentPositionY
}

func printGame() {

	c := exec.Command("clear")
	c.Stdout = os.Stdout
	c.Run()
	printBanner()

	for i := 0; i < len(board); i++ {
		for j := 0; j < len(board[i]); j++ {
			if board[i][j] == 0 {
				fmt.Printf("- ")
			} else {
				fmt.Printf("%d ", board[i][j])
			}
		}
		fmt.Printf("\n")
	}
	calculatePositions()
	printCar()

}

func gameOver(ranks chan Car) bool {
	maxLaps := 0
	for i := 0; i < len(players); i++ {
		if players[i].laps == laps {
			maxLaps++

		}
	}
	if maxLaps == len(players) {
		return true
	}
	return false

}

func printBanner() {
	var banner string = "" +
		"	 ██████╗ ██████╗  █████╗ ███╗   ██╗██████╗     ██████╗ ██████╗ ██╗██╗  ██╗\n" +
		"	██╔════╝ ██╔══██╗██╔══██╗████╗  ██║██╔══██╗    ██╔══██╗██╔══██╗██║╚██╗██╔╝\n" +
		"	██║  ███╗██████╔╝███████║██╔██╗ ██║██║  ██║    ██████╔╝██████╔╝██║ ╚███╔╝\n" +
		"	██║   ██║██╔══██╗██╔══██║██║╚██╗██║██║  ██║    ██╔═══╝ ██╔══██╗██║ ██╔██╗\n" +
		"	╚██████╔╝██║  ██║██║  ██║██║ ╚████║██████╔╝    ██║     ██║  ██║██║██╔╝ ██╗\n" +
		"	 ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚═════╝     ╚═╝     ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝"

	fmt.Println(banner)

}

func printCar() {
	var car string = "\n" +
		"                                     d88b\n" +
		"                     _______________|8888|_______________\n" +
		"                    |_____________ ,~~~~~~. _____________|\n" +
		"  _________         |_____________: mmmmmm :_____________|         _________\n" +
		" / _______ \\   ,----|~~~~~~~~~~~,'\\ _...._ /`.~~~~~~~~~~~|----,   / _______ \\ \n" +
		"| /       \\ |  |    |       |____|,d~    ~b.|____|       |    |  | /       \\ |\n" +
		"||         |-------------------\\-d.-~~~~~~-.b-/-------------------|         ||\n" +
		"||         | |8888 ....... _,===~/......... \\~===._         8888| |         ||\n" +
		"||         |=========_,===~~======._.=~~=._.======~~===._=========|         ||\n" +
		"||         | |888===~~ ...... //,, .`~~~~'. .,\\         ~~===888| |         ||\n" +
		"||        |===================,P'.::::::::.. `?,===================|        ||\n" +
		"||        |_________________,P'_::----------.._`?,_________________|        ||\n" +
		"`|        |-------------------~~~~~~~~~~~~~~~~~~-------------------|        |'\n" +
		"  \\_______/                                                        \\_______/"
	/*
			                         __
		                   _.--""  |
		    .----.     _.-'   |/\| |.--.
		    |jrei|__.-'   _________|  |_)  _______________
		    |  .-""-.""""" ___,    `----'"))   __   .-""-.""""--._
		    '-' ,--. `    |tic|   .---.       |:.| ' ,--. `      _`.
		     ( (    ) ) __|tac|__ \\|// _..--  \/ ( (    ) )--._".-.
		      . `--' ;\__________________..--------. `--' ;--------'
		       `-..-'                               `-..-'
	*/

	fmt.Println(car)
}

func printPodium(winners []int) {
	printBanner()

	fmt.Printf("                                Player %d\n", winners[0])
	fmt.Println("                         @-----------------------@")
	fmt.Printf("        Player %d         |           1           |\n", winners[1])
	fmt.Println("@-----------------------@|           |           |")
	fmt.Printf("|           2           ||           |           |         Player %d      \n", winners[2])
	fmt.Println("|           |           ||           |           |@-----------------------@")
	fmt.Println("|           |           ||           |           ||           3           |")
}

func calculatePositions() {
	var positionsById []int
	for i := 0; i < len(players); i++ {
		positionsById = append(positionsById, (players[i].laps*len(board[0]))+players[i].positionX)
	}
	sort.Ints(positionsById)
	var realPositions []int
	for i := 0; i < len(positionsById); i++ {
		for j := 0; j < len(players); j++ {
			if positionsById[i] == ((players[j].laps * len(board[0])) + players[j].positionX) {
				realPositions = append(realPositions, j)
			}
		}
	}
	realPositions = removeMultiple(realPositions)
	var positionCounter int = 1
	for i := len(players) - 1; i >= 0; i-- {
		//fmt.Println(positionCounter, "° place, Player: ", realPositions[i])
		transitTime := players[realPositions[i]].raceTime
		playerID := realPositions[i]
		playerLaps := players[realPositions[i]].laps
		if (playerLaps + 1) > laps {
			playerLaps = laps
		} else {
			playerLaps++
		}
		fmt.Printf("%d° place | PLAYER %d -> LAPS: %d/%d SPEED:  TIME: %s \n", positionCounter, playerID+1, playerLaps, laps, transitTime)
		positionCounter++

	}

}

func removeMultiple(intSlice []int) []int {
	keys := make(map[int]bool)
	list := []int{}
	for _, entry := range intSlice {
		if _, value := keys[entry]; !value {
			keys[entry] = true
			list = append(list, entry)
		}
	}
	return list
}
