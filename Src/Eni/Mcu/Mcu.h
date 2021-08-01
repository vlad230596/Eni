#pragma once

#include "EniConfig.h"
#include <cstddef>

#ifndef ENI_CMSIS_DEVICE_FILE
#error "ENI_CMSIS_DEVICE_FILE path not defined in Config"
#else
#include ENI_CMSIS_DEVICE_FILE
#endif

namespace Eni::Mcu {
	inline size_t getCurrentInterruptHandlerIndex(){
		return __get_IPSR();
	}

	inline bool isHandlingInterrupt() {
		return getCurrentInterruptHandlerIndex() != 0;
	}

	inline bool isDebuggerAttached() {
		return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0;
	}

	inline void setBreakpoint() {
		if(isDebuggerAttached){
			__BKPT(0);
		}
	}
}

