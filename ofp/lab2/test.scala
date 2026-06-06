@main def testSequence(): Unit = {
  val s1 = Sequence(1, 2)
  val s2 = Sequence(3, 1)

  println("s1.get(4) = " + s1.get(4))
  println("(s1 + s2).get(4) = " + (s1 + s2).get(4))
  println("(s1 * 3).get(4) = " + (s1 * 3).get(4))

  val s3 = s1 / 3

  println("s3.get(0) = " + s3.get(0))
  println("s3.get(1) = " + s3.get(1))
  println("s3.get(2) = " + s3.get(2))
  println("s3.get(3) = " + s3.get(3))
  println("s3.get(4) = " + s3.get(4))
}
