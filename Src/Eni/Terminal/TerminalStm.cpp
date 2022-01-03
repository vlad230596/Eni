

#include <EniConfig.h>

#if defined(ENI_TERMINAL) && defined(ENI_STM)

#include <usart.h>


#include <Eni/Threading/Thread.h>
#include <Eni/Debug/Assert.h>


namespace Eni::Terminal {

	void write(const char* msg, std::size_t length) {
		while(HAL_UART_GetState(&(ENI_TERMINAL_STM_USART_HANDLE)) != HAL_UART_STATE_READY) {
			Eni::Threading::ThisThread::yield();
		}
		HAL_UART_Transmit_DMA(&(ENI_TERMINAL_STM_USART_HANDLE), reinterpret_cast<uint8_t*>(const_cast<char*>(msg)), length);
	}

}

#endif
