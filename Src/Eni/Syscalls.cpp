
#include "EniConfig.h"

#if defined(ENI_NEWLIB_TO_RTOS_MALLOC)

#include <Eni/Debug/EniAssert.h>

#include "FreeRTOS.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <malloc.h>

extern "C" {

void* _realloc_r(struct _reent *re, void* oldAddr, size_t newSize) {
	static_cast<void>(re);
	static_cast<void>(oldAddr);
	static_cast<void>(newSize);
	eniAssert(false); //No mechanism is currently available to correctly reallocate memory, so just don't allow it for now
	return nullptr;
}

//void* _calloc_r(struct _reent *re, size_t num, size_t size) {
//	static_cast<void>(re);
//	void* result = pvPortMalloc(num*size);
//	if(result){
//		uint8_t* bytes = (uint8_t*)result;
//		for(size_t i = 0; i < (num * size); ++i){
//			bytes[i] = 0;
//		}
//	}
//	return result;
//}

void* _malloc_r(struct _reent *re, size_t size) {
	static_cast<void>(re);
	return pvPortMalloc(size);
}

void _free_r(struct _reent *re, void* ptr) {
	static_cast<void>(re);
	vPortFree(ptr);
}

} //extern "C"

#endif
