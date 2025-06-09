package main

func Fibonacci() func() int {
	arr := [2]int{0, 1}
	index := 0
	inc := func() {
		index++
	}

	return func() int {
		defer inc()
		if index <= 1 {
			return arr[index]
		}

		sum := arr[0] + arr[1]
		arr[0] = arr[1]
		arr[1] = sum
		return sum
	}
}
