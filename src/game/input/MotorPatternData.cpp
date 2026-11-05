#include <game/input/MotorPattern.h>

#pragma section sconst_type ".sdata"
extern const RumbleStep gMotorPatternShortWii[] = {{50, false, 0, 0, 0}};
extern const RumbleStep gMotorPatternShortGC[] = {{50, false, 0, 0, 0}};
extern const RumbleStep gMotorPatternLongWii[] = {{100, false, 0, 0, 0}};
extern const RumbleStep gMotorPatternLongGC[] = {{100, false, 0, 0, 0}};
extern const RumbleStep gMotorPatternPulseWii[] = {{50, false, 0, 0, 0}};
extern const RumbleStep gMotorPatternPulseGC[] = {{50, false, 0, 0, 0}};

#pragma section sconst_type ".sdata2"
extern const MotorPattern gMotorPatterns[] = {
    {0, gMotorPatternShortWii, gMotorPatternShortGC},
    {1, gMotorPatternLongWii, gMotorPatternLongGC},
    {2, gMotorPatternPulseWii, gMotorPatternPulseGC}
};
