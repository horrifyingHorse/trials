package main

import (
	"bufio"
	"fmt"
	// "log"
	"os"
	"os/exec"
	"strings"
)

func main() {
	// ENV := make(map[string]string)
	// inheritedEnv := os.Environ()
	// for _, envVar := range inheritedEnv {
	// 	if index := strings.Index(envVar, "="); index == -1 {
	// 		log.Fatal("Invalid Environment Variable Inherited from parent. got:", envVar)
	// 	} else {
	// 		ENV[envVar[:index]] = envVar[index+1:]
	// 	}
	// }
	fmt.Println("$HOME=" + os.Getenv("HOME"))

	scanner := bufio.NewScanner(os.Stdin)

	for {
		fmt.Print("gosh> ")
		if !scanner.Scan() {
			break
		}

		// Expand Env -> Tokenize
		// [ O R ]
		// Tokenize		-> Expand Env
		// That is the Question
		//
		// ans} To implement $status or $?
		// We need to Tokenize -> Expand Env
		line := os.ExpandEnv(scanner.Text())
		if line == "exit" {
			break
		}

		arr := strings.Split(line, " ")

		// Explicitly Handle: | < > 2> 1> 2>&1 & && ~ $?
		cmd := exec.Command(arr[0], arr[1:]...)
		cmd.Stdout = os.Stdout
		cmd.Stdin = os.Stdin
		cmd.Stderr = os.Stderr
		if err := cmd.Run(); err != nil {
			fmt.Println(err.Error())
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
