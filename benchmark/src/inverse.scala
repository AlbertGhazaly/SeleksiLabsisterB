import scala.io.Source
import java.io.PrintWriter

object Inverse {
  def swapRows(matrix: Array[Array[Double]], row1: Int, row2: Int): Unit = {
    val temp = matrix(row1)
    matrix(row1) = matrix(row2)
    matrix(row2) = temp
  }

  def gaussJordan(matrix: Array[Array[Double]], inverse: Array[Array[Double]], n: Int): Unit = {
    for (i <- 0 until n) {
      // If the pivot is zero, swap with a lower row
      if (matrix(i)(i) == 0) {
        for (j <- (i + 1) until n) {
          if (matrix(j)(i) != 0) {
            swapRows(matrix, i, j)
            swapRows(inverse, i, j)
            // Break the inner loop after swapping
            return
          }
        }
      }
      val pivot = matrix(i)(i)
      for (j <- 0 until n) {
        matrix(i)(j) /= pivot
        inverse(i)(j) /= pivot
      }
      for (j <- 0 until n) {
        if (j != i) {
          val factor = matrix(j)(i)
          for (k <- 0 until n) {
            matrix(j)(k) -= factor * matrix(i)(k)
            inverse(j)(k) -= factor * inverse(i)(k)
          }
        }
      }
    }
  }

  def main(args: Array[String]): Unit = {
    val inputFile = "input/input.txt"
    val outputFile = "output/scala.txt"
    val lines = Source.fromFile(inputFile).getLines().toArray
    val n = lines(0).toInt

    val matrix = Array.ofDim[Double](n, n)
    val inverse = Array.ofDim[Double](n, n)

    for (i <- 0 until n) {
      val rowValues = lines(i + 1).split(" ").map(_.toDouble)
      for (j <- 0 until n) {
        matrix(i)(j) = rowValues(j)
        inverse(i)(j) = if (i == j) 1.0 else 0.0
      }
    }

    gaussJordan(matrix, inverse, n)

    val writer = new PrintWriter(outputFile)
    for (i <- 0 until n) {
      writer.println(inverse(i).mkString(" "))
    }
    writer.close()
  }
}
