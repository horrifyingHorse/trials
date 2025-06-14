package main

import (
	"fmt"
	"runtime"
)

// basic recursion factorial
func factorial(x int) int {
	if x == 0 || x == 1 {
		return 1
	}
	return x * factorial(x-1)
}

// factorial with tail recursion
func fact(x, a int) int {
	if x == 0 || x == 1 {
		return a
	}
	return fact(x-1, a*x)
}

// Jackit Phuncshion
func facto(x int) int {
	return fact(x, 1)
}

// Multiple return values
func division(num, den int) (int, int) {
	// Explicit variable declaration
	// var quo, rem int
	quo := num / den
	rem := num % den

	return quo, rem
}

func main() {
	// Any exported function or anything exported is
	// implicit if its name begins with an uppercase
	// letter
	fmt.Println("Hello noobs")
	fmt.Println(factorial(5))
	fmt.Printf("Using tail recursion %d\n", facto(5))

	quo, rem := division(17, 3)
	fmt.Printf("num=17, den=3, quotient=%d, remainder=%d\n", quo, rem)
	fmt.Println(quo, &quo)

	type Student struct {
		Name string
		Id   int
	}
	var student Student
	student.Name = "Noob"
	student.Id = 1e5
	std := Student{Name: "Noob"}
	fmt.Println(student, std)

	var studs [10]Student
	// Mordernized(?) for loop using range over int(?)
	for i := range 10 {
		studs[i] = Student{"Oh no, Riyaal!", i}
	}
	fmt.Println(studs)

	// slices
	// A slice does not store any data, in fact its a ref
	// to the underlying array!
	var ids0to4 []Student = studs[:5]
	fmt.Println(ids0to4)

	var m map[string]Student
	m = make(map[string]Student)
	m["Idiot"] = studs[9]
	item, ok := m["ok is false here"]
	if ok {
		fmt.Println(item)
	} else {
		fmt.Println("Did not find `item`")
	}

	fib := Fibonacci()
	for _ = range 10 {
		fmt.Println(fib())
	}

	fmt.Println("Os = " + runtime.GOOS)

}
