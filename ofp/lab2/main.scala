class Sequence(val a0: Int, val d: Int, val limit: Int = -1) {

  def get(i: Int): Int =
    if (limit != -1 && i >= limit) 0
    else a0 + d * i

  def +(other: Sequence): Sequence =
    new Sequence(a0 + other.a0, d + other.d)

  def *(k: Int): Sequence =
    new Sequence(a0 * k, d * k, limit)

  def /(n: Int): Sequence =
    new Sequence(a0, d, n)

  override def toString: String =
    "Sequence(" + a0 + ", " + d + ")"
}


object Main extends App {

  val s1 = new Sequence(1,2)
  val s2 = new Sequence(3,1)

  println(s1.get(4))
  println((s1+s2).get(4))
  println((s1*3).get(4))

  val s3=s1/3

  println(s3.get(0))
  println(s3.get(1))
  println(s3.get(2))
  println(s3.get(3))
  println(s3.get(4))
}