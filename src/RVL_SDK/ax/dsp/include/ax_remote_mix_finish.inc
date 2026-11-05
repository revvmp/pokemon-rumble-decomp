    addp's $ACC1 : @$AR3, $AC0.L
    asr16 $ACC1
    srri @$AR3, $AC1.M
    movp's $ACC0 : @$AR3, $AC1.L
    mrr $AX0.L, $AC0.M
