def reverse(lst: List[List[Int]]): List[List[Int]] = lst match {
  case Nil => Nil
  case x :: xs =>
    def revInner(l: List[Int]): List[Int] = l match {
      case Nil => Nil
      case h :: t => revInner(t) ::: List(h)
    }

    reverse(xs) ::: List(revInner(x))
}