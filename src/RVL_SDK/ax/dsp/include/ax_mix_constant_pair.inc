    mulxac's $AX0.H, $AX1.L, $ACC1 : @$AR3, $AC0.L
    asr16'l $ACC1 : $AC0.M, @$AR2
    lrri $AC0.L, @$AR2
    lsl16'ls $ACC0 : $AX0.H, $AC1.M
    mulxac's $AX0.H, $AX1.L, $ACC0 : @$AR3, $AC1.L
    asr16'l $ACC0 : $AC1.M, @$AR2
    lrri $AC1.L, @$AR2
