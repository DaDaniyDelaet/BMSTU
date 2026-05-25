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

object Main extends App {

  val p1 = new Polynom(List(5,3,2))
  println(p1.degree)
  println(p1.diff)

  val p2 = new Polynom(List(1.0,4.0,0.0,7.0))
  println(p2.degree)
  println(p2.diff)
}