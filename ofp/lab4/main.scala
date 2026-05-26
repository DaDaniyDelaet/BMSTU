sealed trait Expr

case class Number(value: Double) extends Expr
case class Var(name: String) extends Expr

case class Add(left: Expr, right: Expr) extends Expr
case class Sub(left: Expr, right: Expr) extends Expr
case class Mul(left: Expr, right: Expr) extends Expr
case class Div(left: Expr, right: Expr) extends Expr
case class Pow(left: Expr, right: Expr) extends Expr

case class Neg(expr: Expr) extends Expr

case class Sin(expr: Expr) extends Expr
case class Cos(expr: Expr) extends Expr
case class Exp(expr: Expr) extends Expr
case class Ln(expr: Expr) extends Expr

object Main {

  def derivative(expr: Expr, variable: String): Expr = expr match {
    case Number(_) =>
      Number(0)

    case Var(name) =>
      if (name == variable) Number(1) else Number(0)

    case Add(a, b) =>
      Add(derivative(a, variable), derivative(b, variable))

    case Sub(a, b) =>
      Sub(derivative(a, variable), derivative(b, variable))

    case Mul(a, b) =>
      Add(
        Mul(derivative(a, variable), b),
        Mul(a, derivative(b, variable))
      )

    case Div(a, b) =>
      Div(
        Sub(
          Mul(derivative(a, variable), b),
          Mul(a, derivative(b, variable))
        ),
        Pow(b, Number(2))
      )

    case Pow(a, Number(n)) =>
      Mul(
        Mul(Number(n), Pow(a, Number(n - 1))),
        derivative(a, variable)
      )

    case Pow(a, b) =>
      Mul(
        Pow(a, b),
        Add(
          Mul(derivative(b, variable), Ln(a)),
          Div(Mul(b, derivative(a, variable)), a)
        )
      )

    case Neg(a) =>
      Neg(derivative(a, variable))

    case Sin(a) =>
      Mul(Cos(a), derivative(a, variable))

    case Cos(a) =>
      Neg(Mul(Sin(a), derivative(a, variable)))

    case Exp(a) =>
      Mul(Exp(a), derivative(a, variable))

    case Ln(a) =>
      Div(derivative(a, variable), a)
  }

  def main(args: Array[String]): Unit = {
    val expr =
      Add(
        Mul(Number(2), Pow(Var("x"), Number(3))),
        Sin(Var("x"))
      )

    println(expr)
    println(derivative(expr, "x"))
  }
}