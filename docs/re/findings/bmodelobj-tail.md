# BModelObj tail follow-up

CalcSize remains95.72392%. Direct comparison shows326 native instructions in both objects. Differences include minZ fabs/extentX register coalescing, a reversed multiply operand pair, and model-scale load scheduling in the second branch. A scratch sweep of360 permutations of the six CalcExtent scalar declarations, keeping assignment order unchanged, produced no improvement. No BModelObj source change was retained. Earlier expression and absolute-extrema experiments are recorded in sprint-a17cx.md and sprint-a15.md.

Message InitMsgResList was also probed in isolation: twelve allocator-tag helper signatures varied named-tag declaration order, by-value/reference parameters and expression temporaries. None improved the existing99.98734%; passing temporary tags to mutable references was rejected by the compiler. No shared vector change was retained.
