import abc
import enum
import re
import sys
from dataclasses import dataclass
from pprint import pprint

import parser_edsl as pe


class RoutineKind(enum.Enum):
    Function = "Function"
    Sub = "Sub"


@dataclass
class Program:
    routines: list


@dataclass
class Routine:
    kind: RoutineKind
    name: str
    params: list
    body: list


@dataclass
class Param:
    name: str
    is_array: bool


class Statement(abc.ABC):
    pass


@dataclass
class Assign(Statement):
    target: object
    expr: object


@dataclass
class Dim(Statement):
    name: str
    size: object


@dataclass
class ForStatement(Statement):
    var: str
    start: object
    end: object
    body: list


@dataclass
class IfStatement(Statement):
    condition: object
    body: list


@dataclass
class DoStatement(Statement):
    condition_type: str
    condition: object
    body: list
    post_condition: bool


class Expr(abc.ABC):
    pass


@dataclass
class Var(Expr):
    name: str


@dataclass
class CallOrIndex(Expr):
    name: str
    args: list


@dataclass
class Const(Expr):
    value: object


@dataclass
class BinOp(Expr):
    left: object
    op: str
    right: object


@dataclass
class UnOp(Expr):
    op: str
    expr: object


def kw(name):
    return pe.Terminal(name, name, lambda _: None,
                       re_flags=re.IGNORECASE, priority=10)


def make_op(op):
    return lambda: op


def make_name(text):
    return text.upper()


def make_string(text):
    return text[1:-1]


IDENT = pe.Terminal("IDENT", r"[A-Za-z][A-Za-z0-9]*[%&!#$]?", make_name)
INTEGER = pe.Terminal("INTEGER", r"[0-9]+", int, priority=7)
REAL = pe.Terminal("REAL", r"[0-9]+\.[0-9]*", float, priority=8)
STRING = pe.Terminal("STRING", r'"[^"]*"', make_string)

KW_FUNCTION = kw("Function")
KW_SUB = kw("Sub")
KW_END = kw("End")
KW_DIM = kw("Dim")
KW_FOR = kw("For")
KW_TO = kw("To")
KW_NEXT = kw("Next")
KW_IF = kw("If")
KW_THEN = kw("Then")
KW_DO = kw("Do")
KW_WHILE = kw("While")
KW_UNTIL = kw("Until")
KW_LOOP = kw("Loop")

NProgram = pe.NonTerminal("Program")
NRoutines = pe.NonTerminal("Routines")
NRoutine = pe.NonTerminal("Routine")
NParams = pe.NonTerminal("Params")
NParamList = pe.NonTerminal("ParamList")
NParam = pe.NonTerminal("Param")

NStatements = pe.NonTerminal("Statements")
NStatement = pe.NonTerminal("Statement")
NTarget = pe.NonTerminal("Target")

NExpr = pe.NonTerminal("Expr")
NCmpExpr = pe.NonTerminal("CmpExpr")
NAddExpr = pe.NonTerminal("AddExpr")
NTerm = pe.NonTerminal("Term")
NFactor = pe.NonTerminal("Factor")
NArgs = pe.NonTerminal("Args")
NArgList = pe.NonTerminal("ArgList")
NCmpOp = pe.NonTerminal("CmpOp")
NAddOp = pe.NonTerminal("AddOp")
NMulOp = pe.NonTerminal("MulOp")

NProgram |= NRoutines, Program

NRoutines |= NRoutine, lambda r: [r]
NRoutines |= NRoutines, NRoutine, lambda rs, r: rs + [r]

NRoutine |= (
    KW_FUNCTION, IDENT, "(", NParams, ")", NStatements,
    KW_END, KW_FUNCTION,
    lambda name, params, body: Routine(RoutineKind.Function, name, params, body)
)

NRoutine |= (
    KW_SUB, IDENT, "(", NParams, ")", NStatements,
    KW_END, KW_SUB,
    lambda name, params, body: Routine(RoutineKind.Sub, name, params, body)
)

NParams |= lambda: []
NParams |= NParamList

NParamList |= NParam, lambda p: [p]
NParamList |= NParamList, ",", NParam, lambda ps, p: ps + [p]

NParam |= IDENT, lambda name: Param(name, False)
NParam |= IDENT, "(", ")", lambda name: Param(name, True)

NStatements |= lambda: []
NStatements |= NStatements, NStatement, lambda sts, st: sts + [st]

NStatement |= NTarget, "=", NExpr, Assign
NStatement |= KW_DIM, IDENT, "(", NExpr, ")", Dim

NStatement |= (
    KW_FOR, IDENT, "=", NExpr, KW_TO, NExpr,
    NStatements, KW_NEXT, IDENT,
    lambda var, start, end, body, _: ForStatement(var, start, end, body)
)

NStatement |= (
    KW_IF, NExpr, KW_THEN, NStatements, KW_END, KW_IF,
    IfStatement
)

NStatement |= (
    KW_DO, KW_WHILE, NExpr, NStatements, KW_LOOP,
    lambda cond, body: DoStatement("while", cond, body, False)
)

NStatement |= (
    KW_DO, KW_UNTIL, NExpr, NStatements, KW_LOOP,
    lambda cond, body: DoStatement("until", cond, body, False)
)

NStatement |= (
    KW_DO, NStatements, KW_LOOP, KW_WHILE, NExpr,
    lambda body, cond: DoStatement("while", cond, body, True)
)

NStatement |= (
    KW_DO, NStatements, KW_LOOP, KW_UNTIL, NExpr,
    lambda body, cond: DoStatement("until", cond, body, True)
)

NStatement |= (
    KW_DO, NStatements, KW_LOOP,
    lambda body: DoStatement("forever", None, body, False)
)

NTarget |= IDENT, Var
NTarget |= IDENT, "(", NArgList, ")", CallOrIndex

NExpr |= NCmpExpr

NCmpExpr |= NAddExpr
NCmpExpr |= NCmpExpr, NCmpOp, NAddExpr, BinOp

for op in (">", "<", ">=", "<=", "=", "<>"):
    NCmpOp |= op, make_op(op)

NAddExpr |= NTerm
NAddExpr |= NAddExpr, NAddOp, NTerm, BinOp

NAddOp |= "+", make_op("+")
NAddOp |= "-", make_op("-")

NTerm |= NFactor
NTerm |= NTerm, NMulOp, NFactor, BinOp

NMulOp |= "*", make_op("*")
NMulOp |= "/", make_op("/")

NFactor |= INTEGER, Const
NFactor |= REAL, Const
NFactor |= STRING, Const
NFactor |= IDENT, Var
NFactor |= IDENT, "(", NArgList, ")", CallOrIndex
NFactor |= "(", NExpr, ")"
NFactor |= "-", NFactor, lambda x: UnOp("-", x)
NFactor |= "+", NFactor, lambda x: UnOp("+", x)

NArgList |= NExpr, lambda e: [e]
NArgList |= NArgList, ",", NExpr, lambda args, e: args + [e]

parser = pe.Parser(NProgram, method=pe.EARLEY)
parser.add_skipped_domain(r"\s")
parser.add_skipped_domain(r"'[^\n]*")


if __name__ == "__main__":
    for filename in sys.argv[1:]:
        try:
            with open(filename, encoding="utf-8") as file:
                tree = parser.parse(file.read())
                pprint(tree)
        except pe.Error as e:
            print(f"Ошибка {e.pos}: {e.message}")