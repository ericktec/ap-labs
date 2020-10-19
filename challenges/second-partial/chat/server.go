// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 254.
//!+

// Chat is a server that lets clients chat with each other.
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

//!+broadcaster
type client chan<- string // an outgoing message channel

type user struct {
	username  string
	connected bool
	admin     bool
	conection net.Conn
	channel   chan string
	ip        string
	register  string
}

var (
	entering = make(chan string)
	leaving  = make(chan string)
	messages = make(chan string) // all incoming client messages
	userInfo = make(chan user)
	users    = make(map[string]*user)
	first    = true
)

func broadcaster() {

	for {
		select {
		case msg := <-messages:
			// Broadcast incoming message to all
			// clients' outgoing message channels.
			for cli := range users {
				users[cli].channel <- msg
			}

		case cli := <-entering:

			users[cli].connected = true

		case cli := <-leaving:
			close(users[cli].channel)
			delete(users, cli)
		}
	}
}

//!-broadcaster

func getUsers() string {
	var temp string
	for i := range users {
		temp += "irc-server > " + users[i].username + "- connected since " + users[i].register + ",		"
	}
	return temp
}

//!+handleConn
func handleConn(conn net.Conn) {
	ch := make(chan string) // outgoing client messages
	go clientWriter(conn, ch)

	who := conn.RemoteAddr().String()

	input := bufio.NewScanner(conn)
	input.Scan()
	currentUser := input.Text()
	if users[currentUser] != nil {
		ch <- "Error that username is already registered"
		conn.Close()
		return
	} else if first {
		users[currentUser] = &user{
			username:  currentUser,
			connected: true,
			admin:     true,
			conection: conn,
			channel:   ch,
			ip:        who,
			register:  time.Now().Format(time.RFC850),
		}
		fmt.Printf("irc-server > [%s] was promoted as the channel ADMIN\n", users[currentUser].username)
		first = false
	} else {
		users[currentUser] = &user{
			username:  currentUser,
			connected: true,
			admin:     false,
			conection: conn,
			channel:   ch,
			ip:        who,
			register:  time.Now().Format(time.RFC850),
		}
	}

	fmt.Printf("irc-server > New connected user [%s]\n", users[currentUser].username)
	ch <- "You are " + users[currentUser].username
	messages <- users[currentUser].username + " has arrived"
	entering <- currentUser

	for input.Scan() {
		if users[currentUser] == nil {
			return
		}

		subCommand := strings.Split(input.Text(), " ")

		switch subCommand[0] {
		case "/users":
			ch <- getUsers()

		case "/time":
			ch <- "irc-server > Local Time: " + time.Now().Format(time.RFC850)
		case "/user":
			if len(subCommand) == 2 {
				temp := users[subCommand[1]]
				if temp != nil {
					ch <- "irc-server > username: " + temp.username + ", IP: " + temp.ip + " Connected since: " + temp.register
				} else {
					ch <- "irc-server > Error that user does not exist"
				}
			} else {
				ch <- "irc-server > Error wrong parameters"
			}

		case "/msg":
			temp := users[subCommand[1]]
			output := users[currentUser].username + " (private):"
			if temp != nil {
				if len(subCommand) > 2 {
					for i := 2; i < len(subCommand); i++ {
						output += " " + subCommand[i]
					}
					temp.channel <- output
				} else {
					ch <- "irc-server > Error missing parameters"
				}

			} else {
				ch <- "irc-server > Error that user does not exist"
			}

		case "/kick":
			if users[currentUser].admin {
				if len(subCommand) == 2 {
					temp := users[subCommand[1]]
					if temp != nil {
						temp.channel <- "irc-server > You're kicked from this channel"
						fmt.Printf("irc-server > [%s] was kicked\n", temp.username)
						leaving <- temp.username
						messages <- "irc-server > [" + temp.username + "] was kicked from channel"

					} else {
						ch <- "irc-server > Error this user does not exist"
					}
				}

			} else {
				ch <- "irc-server > Error you don't have the permissions required"
			}
		default:
			messages <- users[currentUser].username + ": " + input.Text()
		}

	}
	// NOTE: ignoring potential errors from input.Err()
	if users[currentUser] != nil {
		messages <- users[currentUser].username + " has left"
		fmt.Printf("irc-server > [%s] left\n", users[currentUser].username)
		leaving <- currentUser
		conn.Close()
	}

}

func clientWriter(conn net.Conn, ch <-chan string) {
	for msg := range ch {
		fmt.Fprintln(conn, msg) // NOTE: ignoring network errors
	}
}

//!-handleConn

//!+main
func main() {
	var port string
	var host string
	if len(os.Args) == 5 {
		if os.Args[1] == "-host" {
			host = os.Args[2]
		} else if os.Args[1] == "-port" {
			port = os.Args[2]
		} else {
			fmt.Println("Wrong flag")
			return
		}

		if os.Args[3] == "-host" {
			host = os.Args[4]
		} else if os.Args[3] == "-port" {
			port = os.Args[4]
		} else {
			fmt.Println("Wrong flag")
			return
		}
	} else {
		fmt.Println("Error wrong arguments")
		return
	}
	listener, err := net.Listen("tcp", host+":"+port)
	if err != nil {
		log.Fatal(err)
	}

	fmt.Printf("irc-server > Simple IRC Server started at %s:%s \nirc-server > Ready for receiving new clients\n", host, port)

	go broadcaster()
	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Print(err)
			continue
		}
		go handleConn(conn)
	}
}

//!-main
