#pragma once


#include "Eni/Mcu/Mcu.h"
#include <Eni/Debug/EniAssert.h>

#include <cassert>
#include <cstdint>

#include "FreeRTOS.h"
#include "portable.h"
#include "queue.h"
#include "semphr.h"


namespace Eni::Threading {

class Semaphore {
public:
	Semaphore(SemaphoreHandle_t handle) :
		_handle(handle)
	{
		eniAssert(_handle);
	}

	~Semaphore(){
		vSemaphoreDelete(_handle);
	}

	size_t messagesWaiting() const {
		return (size_t)uxQueueMessagesWaiting(_handle);
	}

	bool take(TickType_t wait = portMAX_DELAY) {
		if (Eni::Mcu::isInterruptHandling()) {
			portBASE_TYPE taskWoken = pdFALSE;
			if (xSemaphoreTakeFromISR(_handle, &taskWoken) != pdTRUE) {
				return false;
			}
			portEND_SWITCHING_ISR(taskWoken);
			return true;
		}  else {
			return xSemaphoreTake(_handle, wait) == pdTRUE;
		}
	}

	bool give() {
		if (Eni::Mcu::isInterruptHandling()) {
			portBASE_TYPE taskWoken = pdFALSE;
			if (xSemaphoreGiveFromISR(_handle, &taskWoken) != pdTRUE) {
				return false;
			}
			portEND_SWITCHING_ISR(taskWoken);
			return true;
		} else {
			return xSemaphoreGive(_handle) == pdTRUE;
		}
	}

protected:
	SemaphoreHandle_t _handle;
};

class BinarySemaphore : public Semaphore{
public:
	BinarySemaphore() : Semaphore(xSemaphoreCreateBinary())
	{
	}
};

class CountingSemaphore : public Semaphore{
public:
	CountingSemaphore(UBaseType_t maxCount, UBaseType_t initialCount) : Semaphore(xSemaphoreCreateCounting(maxCount, initialCount))
	{
	}
};

}

