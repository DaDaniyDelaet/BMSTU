package main

import "fmt"

var functionCalls = map[string]int{}

func f() {
	functionCalls["f"]++
	fmt.Println("function f")
}

func g() {
	functionCalls["g"]++
	fmt.Println("function g")
	f()
}

func h() {
	functionCalls["h"]++
	fmt.Println("function h")
}

func main() {
	f()
	f()
	g()
	h()
	for name, count := range functionCalls {
		fmt.Printf("%s: %d\n", name, count)
	}
}
