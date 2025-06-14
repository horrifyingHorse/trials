package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
	"os/exec"
	"strings"
)

func main() {
	fmt.Println("Hello noobs")
	scanner := bufio.NewScanner(os.Stdin)

	for {
		fmt.Print("gosh> ")
		if !scanner.Scan() {
			break
		}

		line := scanner.Text()
		if line == "exit" {
			break
		}

		arr := strings.Split(line, " ")

		// Explicitly Handle: | < > 2> 1> 2>&1 & &&
		cmd := exec.Command(arr[0], arr[1:]...)
		cmd.Stdout = os.Stdout
		cmd.Stdin = os.Stdin
		cmd.Stderr = os.Stderr
		if err := cmd.Run(); err != nil {
			log.Fatal(err)
		}

		// or
		//
		// if resp, err := cmd.CombinedOutput(); err != nil {
		// 	log.Fatal(err)
		// } else {
		// 	fmt.Printf("%s", resp)
		// }
	}
}
