class Sequence private (val get: Int => Int) {

  def +(other: Sequence): Sequence =
    new Sequence(i => this.get(i) + other.get(i))

  def *(k: Int): Sequence =
    new Sequence(i => k * this.get(i))

  def /(n: Int): Sequence =
    new Sequence(i => if (i >= n) 0 else this.get(i))

  override def toString: String =
    "Sequence"
}

object Sequence {

  def apply(a0: Int, d: Int): Sequence =
    new Sequence(i => a0 + d * i)
}
