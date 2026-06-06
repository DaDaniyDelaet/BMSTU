% Лабораторная работа № 4 «Case-классы и сопоставление с образцом в Scala»
% 26 мая 2026 г.
% Данил Кравец, ИУ9-61Б

# Цель работы

Приобретение навыков разработки case-классов на языке Scala
для представления абстрактных синтаксических деревьев.

# Индивидуальный вариант

Абстрактный синтаксис арифметических выражений:

```text
Expr → Expr + Expr | Expr - Expr | Expr * Expr | Expr / Expr
     | Expr ^ Expr | - Expr | NUMBER | VARNAME
     | sin(Expr) | cos(Expr) | exp(Expr) | ln(Expr)
```

Требуется реализовать функцию:

```scala
derivative : (Expr, VARNAME) => Expr
```

вычисляющую частную производную выражения по заданной переменной.
Упрощение выражения не требуется.

# Реализация

В ходе выполнения лабораторной работы было реализовано абстрактное
синтаксическое дерево арифметических выражений с использованием case-классов.
Для представления различных типов узлов дерева были созданы отдельные классы
для чисел, переменных, бинарных операций и математических функций.

Для вычисления производной была реализована функция `derivative`,
использующая механизм сопоставления с образцом `match/case`.
Функция рекурсивно обходит дерево и применяет соответствующие правила дифференцирования.

```scala
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
        if (name == variable)
          Number(1)
        else
          Number(0)

      case Add(a,b) =>
        Add(
          derivative(a,variable),
          derivative(b,variable)
        )

      case Sub(a,b) =>
        Sub(
          derivative(a,variable),
          derivative(b,variable)
        )

      case Mul(a,b) =>
        Add(
          Mul(derivative(a,variable),b),
          Mul(a,derivative(b,variable))
        )

      case Div(a,b) =>
        Div(
          Sub(
            Mul(derivative(a,variable),b),
            Mul(a,derivative(b,variable))
          ),
          Pow(b,Number(2))
        )

      case Pow(a,Number(n)) =>
        Mul(
          Mul(Number(n),
          Pow(a,Number(n-1))),
          derivative(a,variable)
        )

      case Neg(a) =>
        Neg(
          derivative(a,variable)
        )

      case Sin(a) =>
        Mul(
          Cos(a),
          derivative(a,variable)
        )

      case Cos(a) =>
        Neg(
          Mul(
            Sin(a),
            derivative(a,variable)
          )
        )

      case Exp(a) =>
        Mul(
          Exp(a),
          derivative(a,variable)
        )

      case Ln(a) =>
        Div(
          derivative(a,variable),
          a
        )
    }

  def main(args:Array[String]):Unit = {

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

    println(expr)

    println(
      derivative(expr,"x")
    )
  }
}
```

# Тестирование

```scala
scala> scala main.scala

Add(
   Mul(
      Number(2.0),
      Pow(
         Var(x),
         Number(3.0)
      )
   ),
   Sin(
      Var(x)
   )
)

Add(
   Add(
      Mul(
         Number(0.0),
         Pow(
            Var(x),
            Number(3.0)
         )
      ),
      Mul(
         Number(2.0),
         Mul(
            Mul(
               Number(3.0),
               Pow(
                  Var(x),
                  Number(2.0)
               )
            ),
            Number(1.0)
         )
      )
   ),
   Mul(
      Cos(
         Var(x)
      ),
      Number(1.0)
   )
)
```

Первое выражение представляет:

```text
2*x³ + sin(x)
```

Полученная производная соответствует выражению:

```text
6*x² + cos(x)
```

Упрощение выражения не выполнялось в соответствии с условием задания.

# Вывод

В ходе выполнения лабораторной работы были изучены механизмы создания
case-классов и представления выражений в виде абстрактного синтаксического дерева.
Были получены навыки использования сопоставления с образцом (`match/case`)
для обработки различных типов узлов дерева. Также были изучены способы
рекурсивного обхода синтаксических деревьев и реализации преобразований выражений.
