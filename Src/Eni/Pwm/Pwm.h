#pragma once

#include <EniConfig.h>

#if defined(ENI_NRF)
	#include "NrfPwm.h"
#else
#error "Missing Pwm.h for target platform"
#endif
