from dataclasses import dataclass
from pprint import pprint
import sys


KEYWORDS = {
    "TYPE", "VAR", "RECORD", "END", "POINTER", "TO", "BEGIN",
    "WHILE", "DO", "IF", "THEN", "ELSE", "NEW",
    "NOT", "AND", "OR", "DIV", "MOD",
}


@dataclass
class Token:
    kind: str
    value: str
    line: int
    col: int


class Lexer:
    def __init__(self, text):
        self.text = text
        self.pos = 0
        self.line = 1
        self.col = 1

    def error(self, msg):
        raise SyntaxError(f"{self.line}:{self.col}: {msg}")

    def peek(self):
        if self.pos >= len(self.text):
            return ""
        return self.text[self.pos]

    def peek2(self):
        return self.text[self.pos:self.pos + 2]

    def advance(self):
        ch = self.peek()
        self.pos += 1
        if ch == "\n":
            self.line += 1
            self.col = 1
        else:
            self.col += 1
        return ch

    def skip_ws_comments(self):
        while True:
            while self.peek().isspace():
                self.advance()

            if self.peek2() == "(*":
                self.advance()
                self.advance()
                while self.peek2() != "*)":
                    if self.peek() == "":
                        self.error("не закрыт комментарий")
                    self.advance()
                self.advance()
                self.advance()
            else:
                break

    def next_token(self):
        self.skip_ws_comments()

        line, col = self.line, self.col
        ch = self.peek()

        if ch == "":
            return Token("EOF", "", line, col)

        if ch.isalpha() or ch == "_":
            s = ""
            while self.peek().isalnum() or self.peek() == "_":
                s += self.advance()
            if s in KEYWORDS:
                return Token(s, s, line, col)
            return Token("IDENT", s, line, col)

        if ch.isdigit():
            s = ""
            while self.peek().isdigit():
                s += self.advance()
            if self.peek() == "." and self.text[self.pos + 1:self.pos + 2].isdigit():
                s += self.advance()
                while self.peek().isdigit():
                    s += self.advance()
                return Token("REAL", s, line, col)
            return Token("INT", s, line, col)

        two = self.peek2()
        if two in {":=", "<=", ">="}:
            self.advance()
            self.advance()
            return Token(two, two, line, col)

        if ch in "+-*/<>=#.,;:()^":
            self.advance()
            return Token(ch, ch, line, col)

        self.error(f"неизвестный символ {ch!r}")


@dataclass
class Program:
    type_decls: list
    var_decls: list
    body: list


@dataclass
class TypeDecl:
    name: str
    type_node: object


@dataclass
class VarDecl:
    names: list
    type_node: object


@dataclass
class NamedType:
    name: str


@dataclass
class PointerType:
    base: object


@dataclass
class RecordType:
    parent: str | None
    fields: list


@dataclass
class Assign:
    target: object
    expr: object


@dataclass
class New:
    target: object


@dataclass
class While:
    cond: object
    body: list


@dataclass
class If:
    cond: object
    then_body: list
    else_body: list


@dataclass
class Designator:
    name: str
    suffixes: list


@dataclass
class Number:
    value: str


@dataclass
class UnaryOp:
    op: str
    expr: object


@dataclass
class BinOp:
    op: str
    left: object
    right: object


class Parser:
    def __init__(self, lexer):
        self.lexer = lexer
        self.cur = self.lexer.next_token()

    def error(self, msg):
        raise SyntaxError(f"{self.cur.line}:{self.cur.col}: {msg}")

    def eat(self, kind):
        if self.cur.kind != kind:
            self.error(f"ожидалось {kind}, получено {self.cur.kind}")
        value = self.cur.value
        self.cur = self.lexer.next_token()
        return value

    def parse(self):
        type_decls = []
        var_decls = []

        if self.cur.kind == "TYPE":
            self.eat("TYPE")
            while self.cur.kind == "IDENT":
                name = self.eat("IDENT")
                self.eat("=")
                type_decls.append(TypeDecl(name, self.parse_type()))
                self.eat(";")

        if self.cur.kind == "VAR":
            self.eat("VAR")
            while self.cur.kind == "IDENT":
                names = self.parse_ident_list()
                self.eat(":")
                var_decls.append(VarDecl(names, self.parse_type()))
                self.eat(";")

        self.eat("BEGIN")
        body = self.parse_stmt_seq({"END"})
        self.eat("END")
        self.eat(".")
        self.eat("EOF")

        return Program(type_decls, var_decls, body)

    def parse_ident_list(self):
        names = [self.eat("IDENT")]
        while self.cur.kind == ",":
            self.eat(",")
            names.append(self.eat("IDENT"))
        return names

    def parse_type(self):
        if self.cur.kind == "IDENT":
            return NamedType(self.eat("IDENT"))

        if self.cur.kind == "POINTER":
            self.eat("POINTER")
            self.eat("TO")
            return PointerType(self.parse_type())

        if self.cur.kind == "RECORD":
            self.eat("RECORD")
            parent = None
            if self.cur.kind == "(":
                self.eat("(")
                parent = self.eat("IDENT")
                self.eat(")")

            fields = []
            while self.cur.kind == "IDENT":
                names = self.parse_ident_list()
                self.eat(":")
                fields.append(VarDecl(names, self.parse_type()))
                self.eat(";")

            self.eat("END")
            return RecordType(parent, fields)

        self.error("ожидался тип")

    def parse_stmt_seq(self, stop):
        result = []

        while self.cur.kind not in stop:
            result.append(self.parse_stmt())

            if self.cur.kind == ";":
                self.eat(";")
            elif self.cur.kind not in stop:
                self.error("ожидалась ;")

        return result

    def parse_stmt(self):
        if self.cur.kind == "IDENT":
            target = self.parse_designator()
            self.eat(":=")
            return Assign(target, self.parse_expr())

        if self.cur.kind == "NEW":
            self.eat("NEW")
            self.eat("(")
            target = self.parse_designator()
            self.eat(")")
            return New(target)

        if self.cur.kind == "WHILE":
            self.eat("WHILE")
            cond = self.parse_expr()
            self.eat("DO")
            body = self.parse_stmt_seq({"END"})
            self.eat("END")
            return While(cond, body)

        if self.cur.kind == "IF":
            self.eat("IF")
            cond = self.parse_expr()
            self.eat("THEN")
            then_body = self.parse_stmt_seq({"ELSE", "END"})

            else_body = []
            if self.cur.kind == "ELSE":
                self.eat("ELSE")
                else_body = self.parse_stmt_seq({"END"})

            self.eat("END")
            return If(cond, then_body, else_body)

        self.error("ожидался оператор")

    def parse_designator(self):
        name = self.eat("IDENT")
        suffixes = []

        while self.cur.kind in {".", "^"}:
            if self.cur.kind == ".":
                self.eat(".")
                suffixes.append(("field", self.eat("IDENT")))
            else:
                self.eat("^")
                suffixes.append(("deref",))

        return Designator(name, suffixes)

    def parse_expr(self):
        left = self.parse_simple_expr()

        if self.cur.kind in {"<", ">", "<=", ">=", "#", "="}:
            op = self.cur.kind
            self.eat(op)
            right = self.parse_simple_expr()

            if self.cur.kind in {"<", ">", "<=", ">=", "#", "="}:
                self.error("операции сравнения неассоциативны")

            return BinOp(op, left, right)

        return left

    def parse_simple_expr(self):
        left = self.parse_term()

        while self.cur.kind in {"+", "-", "OR"}:
            op = self.cur.kind
            self.eat(op)
            right = self.parse_term()
            left = BinOp(op, left, right)

        return left

    def parse_term(self):
        left = self.parse_factor()

        while self.cur.kind in {"*", "/", "AND", "DIV", "MOD"}:
            op = self.cur.kind
            self.eat(op)
            right = self.parse_factor()
            left = BinOp(op, left, right)

        return left

    def parse_factor(self):
        if self.cur.kind == "NOT":
            self.eat("NOT")
            return UnaryOp("NOT", self.parse_factor())

        if self.cur.kind in {"+", "-"}:
            op = self.cur.kind
            self.eat(op)
            return UnaryOp(op, self.parse_factor())

        if self.cur.kind in {"INT", "REAL"}:
            value = self.cur.value
            self.eat(self.cur.kind)
            return Number(value)

        if self.cur.kind == "IDENT":
            return self.parse_designator()

        if self.cur.kind == "(":
            self.eat("(")
            expr = self.parse_expr()
            self.eat(")")
            return expr

        self.error("ожидалось выражение")


def main():
    if len(sys.argv) != 2:
        print("usage: python3 main.py test.ob")
        return

    with open(sys.argv[1], encoding="utf-8") as f:
        text = f.read()

    ast = Parser(Lexer(text)).parse()
    pprint(ast, width=120)


if __name__ == "__main__":
    main()