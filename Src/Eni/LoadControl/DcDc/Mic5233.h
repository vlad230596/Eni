#pragma once

#include "Aoz1284.h"
#include <Eni/LoadControl/AdjustableControl.h>
namespace Eni::LoadControl {
	class Mic5233 : public Aoz1284, public AdjustableControl {
	public:
		Mic5233(const GpioPin& enable, const GpioPin& feedbackPwm, Pwm& pwm) :
			Aoz1284(enable),
			AdjustableControl(pwm, 1.24f, 75.f, 10.f, 30.f, 30.f)
		{
			pwm.setPins({feedbackPwm.nativePinId});
		}

		bool setMode(Mode mode) {
			Aoz1284::setMode(mode);
			return true;
		}

		bool enableAdjusting(bool enabled) {
			if(enabled) {
				return _pwm.start();
			} else {
				_pwm.stop();
				return true;
			}
		}

	private:
	};

	class Tpl810 : public Aoz1284, public AdjustableControl {
	public:
		Tpl810(const GpioPin& enable, const GpioPin& feedbackPwm, Pwm& pwm) :
			Aoz1284(enable),
			AdjustableControl(pwm, 1.2f, 100.f, 12.f, 30.f, 30.f)
		{
			pwm.setPins({feedbackPwm.nativePinId});
		}

		bool setMode(Mode mode) {
			Aoz1284::setMode(mode);
			return true;
		}

		bool enableAdjusting(bool enabled) {
			if(enabled) {
				return _pwm.start();
			} else {
				_pwm.stop();
				return true;
			}
		}

	private:
	};

}
