package main

import (
	"bufio"
	"fmt"
	// "reflect"

	// "log"
	"os"
	"os/exec"

	"slices"
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
		cwd, err := os.Getwd()
		if err != nil {
			fmt.Println("Failed to get current dir")
			os.Exit(1)
		}
		fmt.Print(cwd + "> ")
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
		line := scanner.Text()
		line = strings.Trim(line, " \t")
		fmt.Println(Tokenize(&line), len(Tokenize(&line)))
		if line == "exit" {
			break
		}

		// ls|wc
		// echo "ls|wc"

		arr := strings.Split(line, " ")

		if arr[0] == "cd" {
			if len(arr) > 2 {
				fmt.Println("Too many arguments for cd")
			} else {
				if err := os.Chdir(arr[1]); err != nil {
					fmt.Println(err.Error())
				}
			}
			continue
		}

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

func Tokenize(s *string) []string {
	var tokens []string
	var toParse bool
	var from int

	toParse = true
	from = 0

	ignore := strings.Split(" \t", "")
	d := strings.Split("|><", "")
	for i, c := range *s {
		if c == '"' {
			toParse = !toParse
			continue
		}

		if !toParse {
			continue
		}

		if slices.Contains(ignore, string(c)) {
			if i == from {
				from++
				continue
			}
			tokens = append(tokens, (*s)[from:i])
			from = i + 1
			tokens[len(tokens)-1] = os.ExpandEnv(tokens[len(tokens)-1])
		} else if slices.Contains(d, string(c)) {
			if i == from {
				tokens = append(tokens, string(c))
			} else {
				tokens = append(tokens, (*s)[from:i], string(c))
				tokens[len(tokens)-2] = os.ExpandEnv(tokens[len(tokens)-2])
			}
			from = i + 1
		}

	}

	if from != len(*s) {
		tokens = append(tokens, (*s)[from:])
		tokens[len(tokens)-1] = os.ExpandEnv(tokens[len(tokens)-1])
	}

	return tokens
}
