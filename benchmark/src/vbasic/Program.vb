Imports System
Imports System.IO

Module Program
    Sub Main()
        Dim inputFile As String = "input/input.txt"
        Dim outputFile As String = "output/vb.txt"

        Dim lines() As String = File.ReadAllLines(inputFile)
        Dim n As Integer = Integer.Parse(lines(0))
        Dim matrix(n - 1, n - 1) As Double
        Dim inverse(n - 1, n - 1) As Double

        For i As Integer = 0 To n - 1
            Dim values() As String = lines(i + 1).Split(" ")
            For j As Integer = 0 To n - 1
                matrix(i, j) = Double.Parse(values(j))
                inverse(i, j) = If(i = j, 1.0, 0.0)
            Next
        Next

        For i As Integer = 0 To n - 1
            If matrix(i, i) = 0 Then
                For j As Integer = i + 1 To n - 1
                    If matrix(j, i) <> 0 Then
                        SwapRows(matrix, inverse, i, j, n)
                        Exit For
                    End If
                Next
            End If

            Dim pivot As Double = matrix(i, i)
            For j As Integer = 0 To n - 1
                matrix(i, j) /= pivot
                inverse(i, j) /= pivot
            Next

            For j As Integer = 0 To n - 1
                If j <> i Then
                    Dim factor As Double = matrix(j, i)
                    For k As Integer = 0 To n - 1
                        matrix(j, k) -= factor * matrix(i, k)
                        inverse(j, k) -= factor * inverse(i, k)
                    Next
                End If
            Next
        Next

        Using writer As New StreamWriter(outputFile)
            For i As Integer = 0 To n - 1
                For j As Integer = 0 To n - 1
                    writer.Write(inverse(i, j) & " ")
                Next
                writer.WriteLine()
            Next
        End Using
    End Sub

    Sub SwapRows(ByRef matrix(,) As Double, ByRef inverse(,) As Double, row1 As Integer, row2 As Integer, n As Integer)
        For i As Integer = 0 To n - 1
            Dim temp As Double = matrix(row1, i)
            matrix(row1, i) = matrix(row2, i)
            matrix(row2, i) = temp
            temp = inverse(row1, i)
            inverse(row1, i) = inverse(row2, i)
            inverse(row2, i) = temp
        Next
    End Sub
End Module
