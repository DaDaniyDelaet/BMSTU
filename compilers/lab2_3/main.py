from dataclasses import dataclass

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

    def current(self):
        if self.pos >= len(self.text):
            return None
        return self.text[self.pos]

    def advance(self):
        ch = self.current()
        self.pos += 1
        if ch == "\n":
            self.line += 1
            self.col = 1
        else:
            self.col += 1
        return ch

    def tokenize(self):
        tokens = []

        while self.current() is not None:
            ch = self.current()

            if ch.isspace():
                self.advance()
                continue

            line, col = self.line, self.col

            if ch == "`":
                self.advance()
                word = ""
                while self.current() is not None and self.current().isalpha():
                    word += self.advance()

                keywords = {
                    "is": "IS",
                    "or": "OR",
                    "end": "END",
                    "axiom": "AXIOM",
                    "epsilon": "EPSILON"
                }

                if word not in keywords:
                    raise SyntaxError(f"Unknown keyword at {line}:{col}")

                tokens.append(Token(keywords[word], "`" + word, line, col))
                continue

            if ch == '"':
                self.advance()
                value = ""
                while self.current() is not None and self.current() != '"':
                    value += self.advance()

                if self.current() != '"':
                    raise SyntaxError(f"Unclosed string at {line}:{col}")

                self.advance()
                tokens.append(Token("STRING", '"' + value + '"', line, col))
                continue

            if ch.isalpha():
                value = ""
                while self.current() is not None and self.current().isalnum():
                    value += self.advance()
                tokens.append(Token("IDENT", value, line, col))
                continue

            raise SyntaxError(f"Unexpected symbol at {line}:{col}")

        tokens.append(Token("EOF", "", self.line, self.col))
        return tokens


class Node:
    counter = 0

    def __init__(self, label):
        self.id = Node.counter
        Node.counter += 1
        self.children = []
        self.token = None

    @property
    def label(self):
        return self.token.value if self.token.value else "EOF"


class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.pos = 0

        self.table = {
            ("S", "AXIOM"): ["Rules", "EOF"],
            ("S", "IDENT"): ["Rules", "EOF"],
            ("S", "EOF"): ["Rules", "EOF"],

            ("Rules", "AXIOM"): ["Rule", "Rules"],
            ("Rules", "IDENT"): ["Rule", "Rules"],
            ("Rules", "EOF"): [],

            ("Rule", "AXIOM"): ["AxiomOpt", "IDENT", "IS", "Alts", "END"],
            ("Rule", "IDENT"): ["AxiomOpt", "IDENT", "IS", "Alts", "END"],

            ("AxiomOpt", "AXIOM"): ["AXIOM"],
            ("AxiomOpt", "IDENT"): [],

            ("Alts", "IDENT"): ["Symbols", "AltsTail"],
            ("Alts", "STRING"): ["Symbols", "AltsTail"],
            ("Alts", "EPSILON"): ["Symbols", "AltsTail"],

            ("AltsTail", "OR"): ["OR", "Symbols", "AltsTail"],
            ("AltsTail", "END"): [],

            ("Symbols", "IDENT"): ["Symbol", "SymbolsTail"],
            ("Symbols", "STRING"): ["Symbol", "SymbolsTail"],
            ("Symbols", "EPSILON"): ["EPSILON"],

            ("SymbolsTail", "IDENT"): ["Symbol", "SymbolsTail"],
            ("SymbolsTail", "STRING"): ["Symbol", "SymbolsTail"],
            ("SymbolsTail", "OR"): [],
            ("SymbolsTail", "END"): [],

            ("Symbol", "IDENT"): ["IDENT"],
            ("Symbol", "STRING"): ["STRING"],
        }

        self.nonterms = {
            "S", "Rules", "Rule", "AxiomOpt", "Alts",
            "AltsTail", "Symbols", "SymbolsTail", "Symbol"
        }

    def current(self):
        return self.tokens[self.pos]

    def parse(self):
        Node.counter = 0
        root = Node("S")
        stack = [("S", root)]

        while stack:
            symbol, node = stack.pop()
            token = self.current()

            if symbol in self.nonterms:
                key = (symbol, token.kind)

                if key not in self.table:
                    raise SyntaxError(
                        f"Syntax error at {token.line}:{token.col}, got {token.value}"
                    )

                production = self.table[key]

                if not production:
                    node.children.append(Node("ε"))
                else:
                    children = [Node(s) for s in production]
                    node.children.extend(children)

                    for item in reversed(list(zip(production, children))):
                        stack.append(item)

            else:
                if symbol != token.kind:
                    raise SyntaxError(
                        f"Syntax error at {token.line}:{token.col}, got {token.value}"
                    )

                node.token = token
                self.pos += 1

        return root


def make_dot(root):
    lines = ["digraph {"]

    def walk(node):
        safe = node.label.replace("\\", "\\\\").replace('"', '\\"')
        lines.append(f'  n{node.id} [label="{safe}"]')


        for child in node.children:
            lines.append(f"  n{node.id} -> n{child.id}")
            walk(child)

        if len(node.children) > 1:
            chain = " -> ".join(f"n{child.id}" for child in node.children)
            lines.append(f"  {{ rank=same; {chain} [style=invis] }}")

    walk(root)
    lines.append("}")
    return "\n".join(lines)


def main():
    text = open("input.txt", encoding="utf-8").read()

    tokens = Lexer(text).tokenize()
    tree = Parser(tokens).parse()

    print(make_dot(tree))


if __name__ == "__main__":
    main()