    mulc'ldax $AC1.M, $AX0.H : $AX0, @$AR0
    madd'ldaxm $AX0.L, $AX0.H : $AX0, @$AR0
    madd'lsm $AX0.L, $AX0.H : $AX0.H, $AC1.M
    maddx'l $AX0.H, $AX1.L : $AX0.H, @$AR0
    maddx'l $AX0.H, $AX1.H : $AC1.M, @$AR1
    movp'l $ACC0 : $AX0.H, @$AR0
    asl $ACC0, #2
    clrl $AC1.L
    mrr $AX1.H, $AC0.M
