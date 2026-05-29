package main

import (
	"fmt"
	"go/ast"
	"go/format"
	"go/parser"
	"go/token"
	"os"
)

func addCounterVar(file *ast.File) {
	counterDecl := &ast.GenDecl{
		Tok: token.VAR,
		Specs: []ast.Spec{
			&ast.ValueSpec{
				Names: []*ast.Ident{ast.NewIdent("functionCalls")},
				Values: []ast.Expr{
					&ast.CompositeLit{
						Type: &ast.MapType{
							Key:   ast.NewIdent("string"),
							Value: ast.NewIdent("int"),
						},
					},
				},
			},
		},
	}

	var newDecls []ast.Decl
	inserted := false

	for _, decl := range file.Decls {
		newDecls = append(newDecls, decl)

		if !inserted {
			if genDecl, ok := decl.(*ast.GenDecl); ok && genDecl.Tok == token.IMPORT {
				newDecls = append(newDecls, counterDecl)
				inserted = true
			}
		}
	}

	if !inserted {
		newDecls = append([]ast.Decl{counterDecl}, newDecls...)
	}

	file.Decls = newDecls
}

func counterStatement(functionName string) ast.Stmt {
	return &ast.IncDecStmt{
		X: &ast.IndexExpr{
			X: ast.NewIdent("functionCalls"),
			Index: &ast.BasicLit{
				Kind:  token.STRING,
				Value: fmt.Sprintf("%q", functionName),
			},
		},
		Tok: token.INC,
	}
}

func addCountersToFunctions(file *ast.File) {
	for _, decl := range file.Decls {
		if funcDecl, ok := decl.(*ast.FuncDecl); ok {

			funcName := funcDecl.Name.Name

			if funcName != "main" {
				funcDecl.Body.List = append(
					[]ast.Stmt{counterStatement(funcName)},
					funcDecl.Body.List...,
				)
			}
		}
	}
}

func printCounterStatement() ast.Stmt {
	return &ast.RangeStmt{
		Key:   ast.NewIdent("name"),
		Value: ast.NewIdent("count"),
		Tok:   token.DEFINE,
		X:     ast.NewIdent("functionCalls"),
		Body: &ast.BlockStmt{
			List: []ast.Stmt{
				&ast.ExprStmt{
					X: &ast.CallExpr{
						Fun: &ast.SelectorExpr{
							X:   ast.NewIdent("fmt"),
							Sel: ast.NewIdent("Printf"),
						},
						Args: []ast.Expr{
							&ast.BasicLit{
								Kind:  token.STRING,
								Value: "\"%s: %d\\n\"",
							},
							ast.NewIdent("name"),
							ast.NewIdent("count"),
						},
					},
				},
			},
		},
	}
}

func addPrintToMain(file *ast.File) {
	for _, decl := range file.Decls {
		if funcDecl, ok := decl.(*ast.FuncDecl); ok {
			if funcDecl.Name.Name == "main" {
				funcDecl.Body.List = append(
					funcDecl.Body.List,
					printCounterStatement(),
				)
			}
		}
	}
}

func main() {
	if len(os.Args) != 2 {
		fmt.Println("usage: transform <filename.go>")
		return
	}

	fset := token.NewFileSet()

	file, err := parser.ParseFile(
		fset,
		os.Args[1],
		nil,
		parser.ParseComments,
	)

	if err != nil {
		fmt.Printf("Errors in %s\n", os.Args[1])
		return
	}

	addCounterVar(file)
	addCountersToFunctions(file)
	addPrintToMain(file)

	if err := format.Node(os.Stdout, fset, file); err != nil {
		fmt.Printf("Formatter error: %v\n", err)
	}
}
