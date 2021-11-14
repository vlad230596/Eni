#pragma once

#include "EniConfig.h"

#if ENI_TARGET_OS == ENI_OS_FREERTOS

#include "FreeRTOS.h"
#include "task.h"

#include "Eni/Mcu/Mcu.h"

namespace Eni::Threading {
class CriticalSection {
	public:
		CriticalSection() {
			_status = lock();
		}

		~CriticalSection() {
			unlock();
		}

		void unlock() {
			unlock(_status);
		}


		static uint32_t lock() {
			if (Eni::Mcu::isInterruptHandling()) {
				return taskENTER_CRITICAL_FROM_ISR();
			}
			else {
				taskENTER_CRITICAL();
			}
			return 0;
		}

		static void unlock(uint32_t irqStatus) {
			if (Eni::Mcu::isInterruptHandling()) {
				taskEXIT_CRITICAL_FROM_ISR(irqStatus);
			}
			else {
				taskEXIT_CRITICAL();
			}
		}

	private:
		uint32_t _status = 0;
	};
}

#endif
