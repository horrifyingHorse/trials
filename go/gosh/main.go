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

type CmdType int

const (
	BASE CmdType = iota
	PIPE
)

type CmdMetaData struct {
	cmdType CmdType
	cmd     *exec.Cmd
	r, w    *os.File
}

type Shell struct {
	r, w *os.File
	err  error
	cmds []*CmdMetaData
}

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
			fmt.Println("Failed to scan")
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
		tokens := Tokenize(&line)
		fmt.Println(tokens, len(tokens))
		if line == "exit" {
			break
		}

		// ls|wc
		// echo "ls|wc"

		// arr := strings.Split(line, " ")
		// if arr[0] == "cd" {
		// 	if len(arr) > 2 {
		// 		fmt.Println("Too many arguments for cd")
		// 	} else {
		// 		if err := os.Chdir(arr[1]); err != nil {
		// 			fmt.Println(err.Error())
		// 		}
		// 	}
		// 	continue
		// }

		// Explicitly Handle: | < > 2> 1> 2>&1 & && ~ $?
		shell := new(Shell)
		shell.r = os.Stdin
		shell.w = os.Stdout
		shell.cmds = Parse(tokens)

		for _, execCmd := range shell.cmds {
			if execCmd.cmdType == PIPE {
				execCmd.cmd.Stdin = shell.r
				execCmd.r = shell.r
				shell.r, shell.w, shell.err = os.Pipe()
				if shell.err != nil {
					fmt.Println(err.Error())
					os.Exit(1)
				}
				execCmd.cmd.Stdout = shell.w
				execCmd.w = shell.w
			} else {
				execCmd.cmd.Stdin = shell.r
				execCmd.r = shell.r
				if shell.w != os.Stdout {
					shell.r = os.Stdin
					shell.w = os.Stdout
				}
				execCmd.cmd.Stdout = shell.w
				execCmd.w = shell.w
			}
			if err := execCmd.cmd.Start(); err != nil {
				fmt.Println(err.Error())
			}
		}
		for i, cmd := range shell.cmds {
			if err := cmd.cmd.Wait(); err != nil {
				fmt.Println(err.Error())
				return
			}
			if i > 0 && shell.cmds[i-1].cmdType == PIPE {
				cmd.r.Close()
			}
			if cmd.cmdType == PIPE {
				cmd.w.Close()
			}
		}

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

func Parse(tokens []string) []*CmdMetaData {
	var cmds []*CmdMetaData
	var cmd *CmdMetaData
	var execCmd *exec.Cmd
	// var r, w *os.File
	// var err error

	begin := 0
	// r = os.Stdin
	// w = os.Stdout
	for i, t := range tokens {
		if t == "|" {
			execCmd = exec.Command(tokens[begin], tokens[begin+1:i]...)

			cmd = new(CmdMetaData)
			cmd.cmdType = PIPE
			cmd.cmd = execCmd
			// cmd.Stdin = r
			// r, w, err = os.Pipe()
			// if err != nil {
			// 	fmt.Println(err.Error())
			// 	os.Exit(1)
			// }
			// cmd.Stdout = w

			cmds = append(cmds, cmd)
			begin = i + 1
		}
	}
	execCmd = exec.Command(tokens[begin], tokens[begin+1:]...)
	cmd = new(CmdMetaData)
	cmd.cmd = execCmd
	cmd.cmdType = BASE
	// cmd.Stdin = r
	// cmd.Stdout = os.Stdout
	cmds = append(cmds, cmd)

	return cmds
}
