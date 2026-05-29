package main

import (
	"fmt"
	"go/ast"
	"go/parser"
	"go/token"
	"os"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Println("usage: astprint <filename.go>")
		return
	}

	fset := token.NewFileSet()

	file, err := parser.ParseFile(fset, os.Args[1], nil, parser.ParseComments)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
		return
	}

	ast.Fprint(os.Stdout, fset, file, nil)
}
