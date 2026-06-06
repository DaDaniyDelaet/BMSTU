class Polynom[T](val coeffs: List[T]) {

  def degree: Int =
    coeffs.length - 1

  def diff(implicit num: Numeric[T]): Polynom[T] = {
    val newCoeffs =
      coeffs
        .zipWithIndex
        .tail
        .map { case (coef, power) =>
          num.times(coef, num.fromInt(power))
        }

    new Polynom[T](newCoeffs)
  }

  override def toString: String =
    coeffs.mkString("Polynom(", ", ", ")")
}
