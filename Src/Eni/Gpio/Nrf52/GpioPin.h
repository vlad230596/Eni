#pragma once

#include "EniConfig.h"

#if defined(ENI_GPIO) && defined(ENI_NRF)

#include <nrf_gpio.h>


namespace Eni {

enum class OutputMode {
	PushPull = NRF_GPIO_PIN_S0S1,
	OpenDrain = NRF_GPIO_PIN_S0D1,
    H0S1 = NRF_GPIO_PIN_H0S1,
    S0H1 = NRF_GPIO_PIN_S0H1,
    H0H1 = NRF_GPIO_PIN_H0H1,
    D0S1 = NRF_GPIO_PIN_D0S1,
    D0H1 = NRF_GPIO_PIN_D0H1,
    H0D1 = NRF_GPIO_PIN_H0D1
};

enum class InputMode {
	Floating = NRF_GPIO_PIN_NOPULL,
	PullUp = NRF_GPIO_PIN_PULLUP,
	PullDown = NRF_GPIO_PIN_PULLDOWN
};


struct GpioPin {
public:
	constexpr GpioPin(uint32_t nativeId) :
		nativePinId(nativeId)
	{

	}

	uint32_t nativePinId;
};

};

#endif
