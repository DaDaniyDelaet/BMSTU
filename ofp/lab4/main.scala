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

  def derivative(expr: Expr, variable: String): Expr =
    expr match {
      case Number(_) =>
        Number(0)

      case Var(name) =>
        if (name == variable) Number(1)
        else Number(0)

      case Add(left, right) =>
        Add(
          derivative(left, variable),
          derivative(right, variable)
        )

      case Sub(left, right) =>
        Sub(
          derivative(left, variable),
          derivative(right, variable)
        )

      case Mul(left, right) =>
        Add(
          Mul(derivative(left, variable), right),
          Mul(left, derivative(right, variable))
        )

      case Div(left, right) =>
        Div(
          Sub(
            Mul(derivative(left, variable), right),
            Mul(left, derivative(right, variable))
          ),
          Pow(right, Number(2))
        )

      case Pow(base, Number(n)) =>
        Mul(
          Mul(
            Number(n),
            Pow(base, Number(n - 1))
          ),
          derivative(base, variable)
        )

      case Pow(base, power) =>
        Mul(
          Pow(base, power),
          Add(
            Mul(derivative(power, variable), Ln(base)),
            Mul(power, Div(derivative(base, variable), base))
          )
        )

      case Neg(value) =>
        Neg(
          derivative(value, variable)
        )

      case Sin(value) =>
        Mul(
          Cos(value),
          derivative(value, variable)
        )

      case Cos(value) =>
        Neg(
          Mul(
            Sin(value),
            derivative(value, variable)
          )
        )

      case Exp(value) =>
        Mul(
          Exp(value),
          derivative(value, variable)
        )

      case Ln(value) =>
        Div(
          derivative(value, variable),
          value
        )
    }

  def main(args: Array[String]): Unit = {
    val expr =
      Add(
        Mul(
          Number(2),
          Pow(
            Var("x"),
            Number(3)
          )
        ),
        Sin(
          Var("x")
        )
      )

    println("expr =")
    println(expr)

    println("derivative by x =")
    println(derivative(expr, "x"))

    val expr2 =
      Mul(
        Exp(Var("x")),
        Ln(Var("x"))
      )

    println("expr2 =")
    println(expr2)

    println("derivative by x =")
    println(derivative(expr2, "x"))
  }
}
