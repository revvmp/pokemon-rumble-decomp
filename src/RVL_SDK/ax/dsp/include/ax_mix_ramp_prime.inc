    lrri $AX1.L, @$AR1
    mulx'l $AX0.H, $AX1.L : $AC0.M, @$AR2
    lrri $AC0.L, @$AR2
    lsl16'l $ACC0 : $AC1.M, @$AR2
    lrri $AX0.H, @$AR0
    lrri $AX1.L, @$AR1
    mulxac'l $AX0.H, $AX1.L, $ACC0 : $AC1.L, @$AR2
    asr16 $ACC0
    lsl16'ls $ACC1 : $AX0.H, $AC0.M
    lrri $AX1.L, @$AR1
