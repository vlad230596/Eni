

#include <EniConfig.h>

#if defined(ENI_TERMINAL) && defined(ENI_NRF)

#include <array>
#include "TerminalNrf.h"
#include <Eni/Threading/Thread.h>
#include <Eni/Debug/Assert.h>

#include <nrfx_uarte.h>

namespace Eni::Terminal {

	nrfx_uarte_t instance;
	std::array<char, 128> temp;

	bool init(uint32_t nativePinId) {

		instance.p_reg        = NRFX_CONCAT_2(NRF_UARTE, 0);
		instance.drv_inst_idx = NRFX_CONCAT_3(NRFX_UARTE, 0, _INST_IDX);

		nrfx_uarte_config_t config;
		config.pseltxd            = nativePinId;
		config.pselrxd            = NRF_UARTE_PSEL_DISCONNECTED;
		config.pselcts            = NRF_UARTE_PSEL_DISCONNECTED;
		config.pselrts            = NRF_UARTE_PSEL_DISCONNECTED;
		config.p_context          = NULL;
		config.hwfc               = (nrf_uarte_hwfc_t)NRFX_UARTE_DEFAULT_CONFIG_HWFC;
		config.parity             = (nrf_uarte_parity_t)NRFX_UARTE_DEFAULT_CONFIG_PARITY;
		config.baudrate           = (nrf_uarte_baudrate_t)NRF_UARTE_BAUDRATE_115200;
		config.interrupt_priority = NRFX_UARTE_DEFAULT_CONFIG_IRQ_PRIORITY;

		auto initStatus = nrfx_uarte_init(&instance, &config, nullptr);

		return initStatus == NRFX_SUCCESS;
	}

	void write(const char* msg, std::size_t length) {
		while(nrfx_uarte_tx_in_progress(&instance)) {
			Eni::Threading::ThisThread::yield();
		}
		auto* ptr = msg;
		if (!nrfx_is_in_ram(msg)) {
			eniAssert(length <= temp.size());
			memcpy(temp.data(), msg, length);
			ptr = temp.data();
		}
		nrfx_uarte_tx(&instance, reinterpret_cast<const uint8_t*>(ptr), length);
	}

	void flush() {

	}

}

#endif
