Build Requirements
------------------
```
Golang
More details for installation : https://golang.org/doc/install
```


How to build
------------
```
make build

OR

use this commands to introduce the binary name you might want:
go build -o <binary> grand-prix.go
```


How to Run
----------
```
make run (include 20 players with 3 laps)

OR

use this commands to introduce the players and laps you might want:
./<binary> -players=<number> -laps=<number>
```

How to Clean
----------
```
make clean
```