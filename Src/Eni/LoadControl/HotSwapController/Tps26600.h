#pragma once

#include <Eni/Gpio/Gpio.h>

namespace Eni::LoadControl {
	class Tps266600 {
	public:

		enum class Mode {
			Enable,
			Disable
		};

		Tps266600(const GpioPin& shutdown, const GpioPin& fault) :
			_shutdown(shutdown),
			_fault(fault)
		{
			setMode(Mode::Disable);
			Gpio::initInput(_fault, InputMode::PullUp);
			Gpio::initOutput(_shutdown);
		}

		bool powerOk() {
			return Gpio::read(_fault);
		}

		void setMode(Mode mode) {
			switch(mode) {
			case Mode::Disable:
				Gpio::reset(_shutdown);
				break;
			case Mode::Enable:
				Gpio::set(_shutdown);
				break;
			}
		}

	private:
		GpioPin _shutdown;
		GpioPin _fault;
	};
}
