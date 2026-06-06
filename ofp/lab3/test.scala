object Main extends App {
  val p1 = new Polynom(List(5, 3, 2))

  println("p1 = " + p1)
  println("p1.degree = " + p1.degree)
  println("p1.diff = " + p1.diff)

  val p2 = new Polynom(List(1.0, 4.0, 0.0, 7.0))

  println("p2 = " + p2)
  println("p2.degree = " + p2.degree)
  println("p2.diff = " + p2.diff)

  val p3 = new Polynom(List("a", "b", "c"))

  println("p3 = " + p3)
  println("p3.degree = " + p3.degree)
