
#include "EniConfig.h"

#if defined(ENI_GPIO) && defined(STM32)

#include "../Gpio.h"
#include "GpioPin.h"

#if !IS_ENI_GPIO_SUPPORTED
	#if defined(HAL_INCLUDE_FILE)
	#include HAL_INCLUDE_FILE
	#else
	#error "HAL include file missing declaration"
	#endif
#endif

namespace Eni {

void Gpio::applyConfig(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	*(pin.getConfigRegister()) = static_cast<uint32_t>(pin.init.value) << ((pin.pin % (GpioPin::countPinInConfigRegister)) << 2);
#else
	HAL_GPIO_Init(pin.port, &pin.init);
#endif
}

void Gpio::initInput(const GpioPin& pin, InputMode inputMode) {
#if IS_ENI_GPIO_SUPPORTED
	pin.init.mode.mode = 0;
	switch(inputMode) {
		case InputMode::Analog:
			pin.init.mode.cnf1 = 0;
			pin.init.mode.cnf0 = 0;
			break;
		case InputMode::Floating:
			pin.init.mode.cnf1 = 0;
			pin.init.mode.cnf0 = 1;
			break;
		case InputMode::PullDown:
			pin.init.mode.cnf1 = 1;
			pin.init.mode.cnf0 = 0;
			pin.port->ODR &= ~pin.mask;
			break;
		case InputMode::PullUp:
			pin.init.mode.cnf1 = 1;
			pin.init.mode.cnf0 = 0;
			pin.port->ODR |= pin.mask;
			break;
	}

#else
	pin.init.Mode = GPIO_MODE_INPUT;
	switch(inputMode) {
	case InputMode::Analog:
		pin.init.Mode = GPIO_MODE_ANALOG;
		pin.init.Pull = GPIO_NOPULL;
		break;
	case InputMode::Floating:
		pin.init.Pull = GPIO_NOPULL;
		break;
	case InputMode::PullUp:
		pin.init.Pull = GPIO_PULLUP;
		break;
	case InputMode::PullDown:
		pin.init.Pull = GPIO_PULLDOWN;
		break;
	}
#endif
	applyConfig(pin);
}

void Gpio::initOutput(const GpioPin& pin, OutputMode mode, bool isAlternateFunction, PinSpeed speed) {
#if IS_ENI_GPIO_SUPPORTED
	pin.init.mode.mode = static_cast<uint8_t>(speed);
	pin.init.mode.cnf1 = isAlternateFunction;
	switch(mode) {
		case OutputMode::PushPull:
			pin.init.mode.cnf0 = 0;
			break;
		case OutputMode::OpenDrain:
			pin.init.mode.cnf0 = 1;
			break;
	}
#else
	switch(mode) {
	case OutputMode::OpenDrain:
		if(isAlternateFunction) {
			pin.init.Mode = GPIO_MODE_AF_OD;
		}
		else {
			pin.init.Mode = GPIO_MODE_OUTPUT_OD;
		}
		break;
	case OutputMode::PushPull:
		if(isAlternateFunction) {
			pin.init.Mode = GPIO_MODE_AF_PP;
		}
		else {
			pin.init.Mode = GPIO_MODE_OUTPUT_PP;
		}
		break;
	}
#endif
	applyConfig(pin);
}

void Gpio::disconnect(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	initInput(pin, InputMode::Floating);
#else
	HAL_GPIO_DeInit(pin.port, pin.pin);
#endif
}


void Gpio::set(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	pin.port->BSRR = pin.mask;
#else
	HAL_GPIO_WritePin(pin.port, pin.mask, GPIO_PIN_SET);
#endif

}

void Gpio::reset(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	pin.port->BSRR = static_cast<uint32_t>(pin.mask) << 16U;
#else
	HAL_GPIO_WritePin(pin.port, pin.mask, GPIO_PIN_RESET);
#endif
}

void Gpio::toggle(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	pin.port->ODR ^= pin.mask;
#else
	HAL_GPIO_TogglePin(pin.port, pin.mask);
#endif
}

bool Gpio::read(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	return (pin.port->IDR & pin.mask) != 0;
#else
	return HAL_GPIO_ReadPin(pin.port, pin.mask) == GPIO_PIN_SET;
#endif
}

bool Gpio::lock(const GpioPin& pin) {
#if IS_ENI_GPIO_SUPPORTED
	/*LOCK key writing sequence:
		Write 1
		Write 0
		Write 1
		Read 0
		Read 1 (this read is optional but confirms that the lock is active)
		Note: During the LOCK Key Writing sequence, the value of LCK[15:0] must not change
	 */
	  pin.port->LCKR = GPIO_LCKR_LCKK | pin.mask;
	  pin.port->LCKR = pin.mask;
	  pin.port->LCKR = GPIO_LCKR_LCKK | pin.mask;
	  auto dummy = pin.port->LCKR;
	  static_cast<void>(dummy);

	  return (pin.port->LCKR & GPIO_LCKR_LCKK) != 0;
#else
	return HAL_GPIO_LockPin(pin.port, pin.pin) == HAL_OK;
#endif
}

}

#endif
