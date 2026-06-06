Function SumArray#(Values#())
  SumArray# = 0
  For i% = 1 To Len%(Values#)
    SumArray# = SumArray# + Values#(i%)
  Next i%
End Function

Sub Fibonacci(res&())
  n% = Len%(res&)

  If n% >= 1 Then
    res&(1) = 1
  End If

  Do While i% <= n%
    res&(i%) = res&(i% - 1) + res&(i% - 2)
    i% = i% + 1
  Loop
End Sub