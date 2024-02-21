#pragma once

#include <Eni/Gpio/Gpio.h>
#include <Eni/Adc/Adc.h>

namespace Eni::LoadControl {
	class Tps266600 {
	public:

		static constexpr float ImonGain = 0.0000784;

		enum class Mode {
			Enable,
			Disable
		};

		Tps266600(const GpioPin& shutdown, const GpioPin& fault, float rImonValue = 10000.f) :
			_shutdown(shutdown),
			_fault(fault),
			_rImonValue(rImonValue)
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

		float getLoadCurrent(Eni::Adc::ResistorDivider adc) {
			float imonVoltage = adc.getVoltage();
			return (imonVoltage / _rImonValue) / ImonGain;
		} 

	private:
		float _rImonValue;
		GpioPin _shutdown;
		GpioPin _fault;
	};
}
