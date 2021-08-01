
#include "EniConfig.h"

#if defined(ENI_GPIO) && defined(ENI_NRF)

#include "../Gpio.h"
#include <Eni/Gpio/Nrf52/GpioPin.h>
#include <nrf_gpio.h>



namespace Eni {



void Gpio::initInput(const GpioPin& pin, InputMode inputMode) {
	nrf_gpio_cfg_input(pin.nativePinId, static_cast<nrf_gpio_pin_pull_t>(inputMode));
}

void Gpio::initOutput(const GpioPin& pin, OutputMode mode) {
    nrf_gpio_cfg(
		pin.nativePinId,
        NRF_GPIO_PIN_DIR_OUTPUT,
        NRF_GPIO_PIN_INPUT_DISCONNECT,
        NRF_GPIO_PIN_NOPULL,
        static_cast<nrf_gpio_pin_drive_t>(mode),
        NRF_GPIO_PIN_NOSENSE);
}



void Gpio::set(const GpioPin& pin) {
	nrf_gpio_pin_set(pin.nativePinId);
}

void Gpio::reset(const GpioPin& pin) {
	nrf_gpio_pin_clear(pin.nativePinId);
}

void Gpio::toggle(const GpioPin& pin) {
	nrf_gpio_pin_toggle(pin.nativePinId);
}

bool Gpio::read(const GpioPin& pin) {
	return nrf_gpio_pin_read(pin.nativePinId);
}

}

#endif
