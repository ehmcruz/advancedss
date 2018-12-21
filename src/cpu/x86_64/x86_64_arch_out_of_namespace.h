#ifndef _ADVANCEDSS_X8664_INC_OUTOFNAMESPACE_HEADER_
#define _ADVANCEDSS_X8664_INC_OUTOFNAMESPACE_HEADER_

#include "bochs/bochs.h"
#include "bochs/fpu/softfloat.h"
#include "bochs/fpu/tag_w.h"
#include "bochs/fpu/status_w.h"
#include "bochs/fpu/control_w.h"

extern int FPU_tagof(const floatx80 &reg);

#endif
