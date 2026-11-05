    clr $ACC0
    clr'l $ACC1 : $AC0.M, @$AR1
    lrrd $AC1.M, @$AR1
    srri @$AR3, $AC0.M
    add $ACC0, $ACC1
    srri @$AR3, $AC0.M
