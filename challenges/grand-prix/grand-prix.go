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
	id           int
	positionX    int
	positionY    int
	maxSpeed     int
	rank         int
	laps         int
	raceTime     string
	finished     bool
	currentSpeed int
	currentTime  float64
}

var laps int

var players []Car

var board [5][90]int

var wg sync.WaitGroup

var startTime time.Time

var mu sync.RWMutex

func main() {
	// usage: ./grand-prix -players 20 -laps 78

	var lapsFlag = flag.Int("laps", 1, "Laps that game is going to run")
	var numberPlayers = flag.Int("players", 3, "Players needed to run the game")
	flag.Parse()
	laps = *lapsFlag
	fmt.Println(" LAPS FLAG = ", *lapsFlag)
	ranks := make(chan Car, *numberPlayers)
	if *numberPlayers > 400 {
		fmt.Printf("There are too many players for the map we are setting to 400 players")
		*numberPlayers = 400
	}
	fmt.Printf(" Laps = %d, Racers = %d \n", laps, *numberPlayers)
	addPlayers(*numberPlayers)
	play(ranks)

	var winners []int
	close(ranks)
	// place := 1
	for car := range ranks {
		// fmt.Printf(" El %d° puesto es %d\n", place, car.id)
		winners = append(winners, car.id)
		// place++
	}

	printPodium(winners)

}

func addPlayers(numberPlayers int) {
	var row, column int = -1, 0
	min := 6
	max := 12
	for i := 0; i < numberPlayers; i++ {
		rand.Seed(time.Now().UnixNano())

		if (i % len(board)) == 0 {
			row++
			column = 0
		}
		var newCar Car
		newCar.id = i + 1
		newCar.laps = 0
		newCar.maxSpeed = rand.Intn(max-min) + min
		newCar.rank = 0
		newCar.positionX = row
		newCar.positionY = column
		newCar.finished = false
		newCar.raceTime = ""
		newCar.currentSpeed = 0
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
			wg.Add(1)
			go carMovement(&players[i], ranks)
		}
		time.Sleep(time.Second * 1)

		wg.Wait()
		currentTime := time.Since(startTime)
		for i := 0; i < len(players); i++ {
			if players[i].laps < laps {
				players[i].raceTime = currentTime.String()
				players[i].currentTime = currentTime.Seconds()
			}
		}
		printGame()
	}

}

func carMovement(car *Car, ranks chan Car) {
	defer wg.Done()

	// fmt.Printf("PLAYER %d -> LAPS: %d/%d POSITION: %d %d  SPEED:  TIME: %s \n", car.id, car.laps, laps, car.positionX, car.positionY, transitTime)
	if !car.finished {
		currentPositionX := car.positionX
		currentPositionY := car.positionY
		newPositionY := car.positionY
		car.currentSpeed = carAcceleration(car.currentSpeed, car.maxSpeed)
		newPositionX := currentPositionX + car.currentSpeed

		mu.Lock()
		newPositionX, newPositionY = carCollision(currentPositionX, currentPositionY, newPositionX)
		car.currentSpeed = newPositionX - car.positionX

		// if board[car.positionY][(newPositionX+1)%(len(board[0])-1)] != 0 {
		// 	newPositionY = changeLane(currentPositionX, currentPositionY)
		// }

		mu.Unlock()
		if newPositionX > (len(board[0]) - 1) {
			if car.laps < laps {
				car.laps++
			}
			newPositionX = newPositionX % (len(board[0]) - 1)
			if car.laps == laps {
				car.finished = true
				if car.finished {
					mu.Lock()
					board[car.positionY][car.positionX] = 0
					mu.Unlock()
					car.positionX = -1
					car.positionY = -1
					car.rank = (car.laps * len(board[0]))
					ranks <- *car
					return

				}

			}
		}
		mu.Lock()
		board[currentPositionY][currentPositionX] = 0
		board[newPositionY][newPositionX] = car.id
		mu.Unlock()

		car.positionX = newPositionX
		car.positionY = newPositionY

		car.rank = (car.laps * len(board[0])) + newPositionX

	}

}

func carAcceleration(currentSpeed int, maxSpeed int) int {
	if currentSpeed >= 0 && currentSpeed < maxSpeed-2 {
		return currentSpeed + 1
	}
	min := maxSpeed - 2
	max := maxSpeed
	rand.Seed(time.Now().UnixNano())
	return (rand.Intn(max-min) + min)

}

func carCollision(currentPositionX int, currentPositionY int, newPositionX int) (int, int) {
	newPositionY := currentPositionY
	for i := currentPositionX; i < newPositionX; i++ {

		if board[currentPositionY][(i+1)%(len(board[0])-1)] != 0 {

			return i, changeLane(i%(len(board[0])-1), currentPositionY)
		}
	}
	return newPositionX, newPositionY

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
	var positionsById []float64
	actualTime := time.Since(startTime).Seconds()
	for i := 0; i < len(players); i++ {
		if players[i].finished {
			positionsById = append(positionsById, float64(players[i].rank)*(actualTime-players[i].currentTime))
		} else {
			positionsById = append(positionsById, float64(players[i].rank))
		}

		// 3 * 70 = 210 * 4.56 -> 957.6
		// 3 * 70 = 210 * 8.56 -> 1797.6

		// 30 segundos
		// 3 * 70 = 210 * (30 - 4.56) -> 5342.4
		// 3 * 70 = 210 * (30 - 8.56) -> 4502.4

		// 2 * 56 = 112
	}
	sort.Float64s(positionsById)
	var realPositions []int
	for i := 0; i < len(positionsById); i++ {
		for j := 0; j < len(players); j++ {

			if players[j].finished {
				if positionsById[i] == (float64(players[j].rank) * (actualTime - players[j].currentTime)) {
					realPositions = append(realPositions, j)
				}
			} else {
				if positionsById[i] == (float64(players[j].rank)) {
					realPositions = append(realPositions, j)
				}
			}

		}
	}
	realPositions = removeMultiple(realPositions)
	var positionCounter int = 1
	for i := len(players) - 1; i >= 0; i-- {
		//fmt.Println(positionCounter, "° place, Player: ", realPositions[i])

		transitTime := players[realPositions[i]].raceTime
		speed := players[realPositions[i]].currentSpeed
		playerID := realPositions[i]
		playerLaps := players[realPositions[i]].laps

		if (playerLaps + 1) > laps {
			playerLaps = laps
		} else {
			playerLaps++
		}
		fmt.Printf("%d° PLACE | PLAYER %d -> LAPS: %d/%d SPEED: %d0Km/h  TIME: %s\n", positionCounter, playerID+1, playerLaps, laps, speed, transitTime)
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
