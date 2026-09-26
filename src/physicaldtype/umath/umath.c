#include "umath.h"
#include "physical_arithmetic.h"

int
PhysicalDType_InitUFuncs(void)
{
    if (PhysicalDType_InitArithmeticUFuncs() < 0) {
        return -1;
    }

    // if (PhysicalDType_InitUnaryUFuncs() < 0) {
    //     return -1;
    // }

    return 0;
}