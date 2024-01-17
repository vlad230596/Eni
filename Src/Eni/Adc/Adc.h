
#pragma once

#include <EniConfig.h>

#if defined(ENI_NRF)
	#include "NrfAdc.h"
#else
#error "Missing Adc.h for target platform"
#endif
