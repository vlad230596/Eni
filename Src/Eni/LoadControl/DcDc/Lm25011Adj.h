#pragma once

#include "Lm25011.h"
#include <Eni/LoadControl/AdjustableControl.h>

namespace Eni::LoadControl {
	class Lm25011Adj : public Lm25011, public AdjustableControl {
	public:
		Lm25011Adj(const GpioPin& enable, const GpioPin& powerGood, const GpioPin& feedbackPwm, Pwm& pwm) :
			Lm25011(enable, powerGood),
			AdjustableControl(pwm, 2.51f, 22.f, 10.f, 1.5f, 5.1f)
		{
			pwm.setPins({feedbackPwm.nativePinId});
		}

		bool setMode(Mode mode) {
			Lm25011::setMode(mode);
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
