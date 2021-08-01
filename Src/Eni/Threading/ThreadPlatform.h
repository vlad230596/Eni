#pragma once

#include "EniConfig.h"

#if defined(ENI_THREADING)


#if ENI_TARGET_OS == ENI_OS_FREERTOS
	#include "FreeRTOS.h"
	#include "task.h"

namespace Eni::Threading {
	using ThreadID = TaskHandle_t;
}
#else
#include <thread>
namespace Eni::Threading {
	using ThreadID = std::thread::id;
}
#endif


#endif //LF_THREADING
