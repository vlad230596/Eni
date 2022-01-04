

#include <EniConfig.h>

#if defined(ENI_TERMINAL) && defined(ENI_STM)

#include <usart.h>
#include <cstring>
#include <algorithm>

#include <Eni/Threading/Thread.h>
#include <Eni/Debug/Assert.h>


namespace Eni::Terminal {

	std::array<char, 256> temp;
	size_t actualSize = 0;


	void write(const char* msg, std::size_t length) {
		while(HAL_UART_GetState(&(ENI_TERMINAL_STM_USART_HANDLE)) != HAL_UART_STATE_READY) {
			Eni::Threading::ThisThread::yield();
		}
		auto appended = (std::min)(length, temp.size() - actualSize);
		memcpy(temp.data() + actualSize, msg, appended);
		actualSize += appended;
	}

	void flush() {
		while(HAL_UART_GetState(&(ENI_TERMINAL_STM_USART_HANDLE)) != HAL_UART_STATE_READY) {
			Eni::Threading::ThisThread::yield();
		}
		HAL_UART_Transmit_DMA(&(ENI_TERMINAL_STM_USART_HANDLE), reinterpret_cast<uint8_t*>(const_cast<char*>(temp.data())), actualSize);
		actualSize = 0;
	}

}

#endif
