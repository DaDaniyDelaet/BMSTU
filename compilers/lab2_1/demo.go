package main

import "fmt"

func f() {
	fmt.Println("function f")
}

func g() {
	fmt.Println("function g")
	f()
}

func h() {
	fmt.Println("function h")
}

func main() {
	f()
	f()
	g()
	h()
}
