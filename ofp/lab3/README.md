% Лабораторная работа № 3. Разработка обобщённых классов на языке Scala
  с использованием неявных преобразований типов
% 23 мая 2026 г.
% Данил Кравец, ИУ9-61Б


# Цель работы

Приобретение навыков разработки обобщённых классов на языке Scala с
использованием неявных преобразований типов.

# Индивидуальный вариант

Класс Polynom[T], представляющий полином с коэффициентами типа T и операцией,
возвращающей степень полинома. В случае, если тип T является числовым,
для Polynom[T] также должна быть доступна операция дифференцирования.

# Реализация

В ходе выполнения лабораторной работы был разработан обобщённый класс Polynom[T],
представляющий полином с коэффициентами произвольного типа.

Коэффициенты полинома хранятся в списке:

```text
List(a0, a1, a2, ...)
```

где:

```text
a0 + a1*x + a2*x² + ...
```

Класс содержит:

- поле coeffs — список коэффициентов;
- метод degree для определения степени полинома;
- метод diff для вычисления производной;
- переопределённый метод toString для корректного вывода объекта.

В реализации использовались следующие возможности языка Scala:

- обобщённые классы;
- параметризированные типы;
- списки List;
- рекурсия;
- сопоставление с образцом (match/case);
- неявные параметры (implicit);
- класс Numeric[T].

Для реализации операции дифференцирования использовался неявный параметр:

```scala
implicit num: Numeric[T]
```

Это позволяет выполнять дифференцирование только для числовых типов данных. 
Если коэффициенты имеют нечисловой тип, например String,
операция дифференцирования становится недоступной.

Реализация класса:

```scala
class Polynom[T](val coeffs: List[T]) {

  def degree: Int = {
    def deg(xs: List[T], n: Int): Int = xs match {
      case Nil => n - 1
      case _ :: tail => deg(tail, n + 1)
    }

    deg(coeffs, 0)
  }

  def diff(implicit num: Numeric[T]): Polynom[T] = {

    def d(xs: List[T], power: Int): List[T] = xs match {
      case Nil => Nil
      case _ :: Nil => Nil
      case _ :: tail => diffTail(tail, power + 1)
    }

    def diffTail(xs: List[T], power: Int): List[T] = xs match {
      case Nil => Nil
      case x :: tail =>
        num.times(x, num.fromInt(power)) ::
        diffTail(tail, power + 1)
    }

    new Polynom(d(coeffs,0))
  }

  override def toString: String =
    coeffs.toString
}
```

Степень полинома определяется как:

```text
degree = количество коэффициентов − 1
```

Дифференцирование реализовано на основе правила:

```text
(a*xⁿ)' = a*n*xⁿ⁻¹
```

# Тестирование

```scala
scala> val p1 = new Polynom(List(5,3,2))

p1: Polynom[Int] = List(5, 3, 2)

scala> p1.degree

res0: Int = 2

scala> p1.diff

res1: Polynom[Int] =
List(3,4)


scala> val p2 =
new Polynom(List(1.0,4.0,0.0,7.0))

p2: Polynom[Double] =
List(1.0,4.0,0.0,7.0)

scala> p2.degree

res2: Int = 3

scala> p2.diff

res3: Polynom[Double] =
List(4.0,0.0,21.0)


scala> val p3 =
new Polynom(List("a","b","c"))

p3: Polynom[String] =
List(a,b,c)

scala> p3.degree

res4: Int = 2

scala> p3.diff

Ошибка компиляции:
could not find implicit value
for parameter num:
Numeric[String]
```

# Вывод

В ходе выполнения лабораторной работы были изучены механизмы разработки
обобщённых классов в языке Scala. Были получены навыки использования
параметров типа, неявных параметров (implicit) и встроенного класса Numeric[T].
Также были изучены способы ограничения доступности методов в зависимости
от типа параметров класса и особенности реализации вычислений над объектами
обобщённого типа.
