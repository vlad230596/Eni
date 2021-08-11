#pragma once

#include <Eni/Gpio/Gpio.h>

namespace Eni::LoadControl {
	class Lm25011 {
	public:

		enum class Mode {
			Enable,
			Disable
		};

		Lm25011(const GpioPin& enable, const GpioPin& powerGood) :
			_enable(enable),
			_powerGood(powerGood)
		{
			setMode(Mode::Disable);
			Gpio::initInput(_powerGood, InputMode::PullUp);
			Gpio::initOutput(_enable);
		}

		void setMode(Mode mode) {
			switch(mode) {
			case Mode::Disable:
				Gpio::set(_enable);
				break;
			case Mode::Enable:
				Gpio::reset(_enable);
				break;
			}
		}

		bool powerOk() {
			return Gpio::read(_powerGood);
		}
	private:
		GpioPin _enable;
		GpioPin _powerGood;
	};
}
