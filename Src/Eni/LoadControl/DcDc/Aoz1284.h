#pragma once

#include <Eni/Gpio/Gpio.h>

namespace Eni::LoadControl {
	class Aoz1284 {
	public:

		enum class Mode {
			Enable,
			Disable
		};

		Aoz1284(const GpioPin& enable) :
			_enable(enable)
		{
			setMode(Mode::Disable);
			Gpio::initOutput(_enable);
		}

		void setMode(Mode mode) {
			switch(mode) {
			case Mode::Disable:
				Gpio::reset(_enable);
				break;
			case Mode::Enable:
				Gpio::set(_enable);
				break;
			}
		}

	private:
		GpioPin _enable;
	};
}
