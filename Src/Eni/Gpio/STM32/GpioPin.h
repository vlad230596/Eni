#ifndef GPIOPIN_H_
#define GPIOPIN_H_

#include "EniConfig.h"

#if defined(ENI_GPIO) && defined(ENI_STM32)

#include <cstdint>
#if defined(CMSIS_INCLUDE_FILE)
	#include CMSIS_INCLUDE_FILE
#else
	#error "CMSIS include file missing declaration"
#endif

#define IS_ENI_GPIO_SUPPORTED (defined(STM32F1))


namespace Eni {

enum class OutputMode {
	PushPull,
	OpenDrain
};

enum class InputMode {
	Analog,
	Floating,
	PullUp,
	PullDown
};


enum class PinSpeed {
	Speed2MHz = 2,
	Speed10MHz = 1,
	Speed50MHz = 3
};


struct GpioPin {
	#pragma pack(push,1)
	union GpioConfig{
		struct {
			uint8_t mode 	: 2;
			uint8_t cnf0 	: 1;
			uint8_t cnf1 	: 1;
			uint8_t 		: 4;
		} mode;
		uint8_t value;
	};
	#pragma pack(pop)
	static_assert(sizeof(GpioConfig) == 1, "Wrong union size");
public:
	constexpr GpioPin(GPIO_TypeDef * port, uint8_t pin) :
		port(port),
		pin(pin),
		mask(1 << pin)
	{
		#if !IS_ENI_GPIO_SUPPORTED
			init.Pin = mask;
		#else
		#endif
	}

	constexpr GpioPin(GPIO_TypeDef * port, uint16_t mask) :
		port(port),
		pin(0),
		mask(mask)
	{
		for(uint8_t i = 0; i < countPinInPort; ++i) {
			if(mask == static_cast<uint32_t>(1 << i)) {
				pin = i;
			}
		}
		#if !IS_ENI_GPIO_SUPPORTED
			init.Pin = mask;
			init.Pull = GPIO_NOPULL;
			init.Speed = GPIO_SPEED_FREQ_HIGH;
		#else
		#endif
	}
	//Additional constructor for numeric constants
	constexpr GpioPin(GPIO_TypeDef * port, int pin)
		:GpioPin(port, static_cast<uint8_t>(pin))
	{

	}

	__IO uint32_t * getConfigRegister() const {
		if(pin >= (countPinInPort / countPinInConfigRegister)) {
			return &port->CRH;
		}
		else {
			return &port->CRL;
		}
	}

	GPIO_TypeDef * port;
	uint8_t pin;
	uint16_t mask;

#if !IS_ENI_GPIO_SUPPORTED
	mutable GPIO_InitTypeDef init {};
#else
	mutable GpioConfig init {};
#endif

	static constexpr uint8_t countPinInPort = 16;
	static constexpr uint8_t countPinInConfigRegister = countPinInPort / 2;

};

};

#endif

#endif /* GPIOPIN_H_ */
