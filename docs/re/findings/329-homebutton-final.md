# Home Button final Update function match

## Remaining differences and reconstruction

The baseline UpdateHomeButton comparison was 99.64555. The remaining differences were the saved waiting-state register, the stick divisor and X/Y floating-point registers, and an incorrectly shared zero between controller reset and the InitializeHomeButtonInfo argument.

A local OpenState enum preserves the loaded waiting value as a named local. Declaring opened before waiting, then assigning opened=false inside the existing waiting branch, gives the target saved-register order. These are D-0016 allocation controls; the enum is descriptive rather than a recovered original declaration.

The private InitializeHomeButtonInfo allowYield parameter is now int rather than bool. Its 580 normalized PPC instructions are unchanged. The public InitializeHomeButton caller's 480 instructions are also unchanged. The Update caller now emits the target's separate zero for the controller-device reset and for the initialization argument. The private inferred symbol changes from InitializeHomeButtonInfo__24@unnamed@homebutton_cpp@FP11HBMDataInfoP13HBMDataInfoExb to InitializeHomeButtonInfo__24@unnamed@homebutton_cpp@FP11HBMDataInfoP13HBMDataInfoExi at 0x801E24C0. Signedness is not independently established: unsigned int produces the same allocation. This is explicitly a D-0016 signature accommodation, not a claim that an original signature was recovered.

The two scalar helpers are also allocation controls. DivideStick(float coordinate,float divisor) returns coordinate/divisor and is used for the X component. ReadStickY(const float& value) returns value and is used for the Y load. Their inlined code gives the target divisor in f0, X load and division in f1, and Y load in f2. A by-value ReadStickY does not match; the const-reference form does. Neither helper adds a call or instruction.

## Verification

Direct Wii/1.1 compilation against real headers reports UpdateHomeButton at 100%. No other named matched function regresses. The original private helper name needs the documented root symbol correction before the ordinary unit comparison can pair that function; independent normalized instruction comparison proves the helper and its public initialization caller unchanged. Evidence is in ignored tools/local/session/329-homebutton/applied.json, inittype0.cpp, and yhelper1.cpp. Root integrated the private signature correction and emission probes, completed ninja with exit zero, and passed frozen-manifest verification. The homebutton unit is retained as Matching.

Rejected probes include same-type scalar identities; named X/Y loads with declaration permutations; volatile coordinate or divisor loads; named float/double conversion temporaries; enum device zero/reference views; a union zero view; and using waiting/opened expressions as the initialization argument. These either preserve the differences or regress the code. The accepted combination preserves the baseline algorithm and every target instruction.

## Adjacent CUIMenu attempt

CUIMenu::OnInput remains 99.893616. Only four operands differ: target loads the divisor into r3 and uses the selected index saved in r25, while source retains the call result in r3 and loads the divisor into r4. Typed selected-index and row-count enum locals, const references, candidate expressions, same-type reassignment, and six modulo-helper parameter forms did not improve it. No CUIMenu source change was retained. Scratch evidence is under 329-uimenu.


## Native emission order

The first native link exposed two misplaced weak functions despite all target instruction bodies matching: CHBMParam::GetResourceRoot and shared_ptr<CHBMParam>::operator->. A named-false request in OnHomeButtonSound emits GetResourceRoot immediately after that callback. A named-false operator-> request in OpenHomeButtonFile emits the shared-pointer accessor immediately after that wrapper. Both probes disappear from executed code and are D-0016 timing controls. The applied candidate reproduces the complete 51-entry target text order. No shared header gate is needed.

The InitializeHomeButtonInfo direct score of 99.844826 is solely strict relocation-label granularity: the target's large data-base symbol and addends versus source-local string symbols identify identical data offsets. Every normalized PPC instruction is equal, and its entire raw nonrelocation data-prefix comparison matches. CodeWarrior mangles operator-> as __rf__ and operator* as __ml__; confusing them leads to harmless but ineffective template emission probes.

The final native trial with both emission probes passed the frozen main.dol SHA. All 51 target functions and their required data now link from source.
