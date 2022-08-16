#pragma once

#include "Aoz1284.h"
#include <Eni/LoadControl/AdjustableControl.h>

namespace Eni::LoadControl {
	class Aoz1284Adj : public Aoz1284, public AdjustableControl {
	public:
		Aoz1284Adj(const GpioPin& enable, const GpioPin& feedbackPwm, Pwm& pwm) :
			Aoz1284(enable),
			AdjustableControl(pwm, 0.8f, 147.f, 10.f, 22.f, 47.f)
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

	class Aoz1284HighAdj : public Aoz1284, public AdjustableControl {
		public:
		Aoz1284HighAdj(const GpioPin& enable, const GpioPin& feedbackPwm, Pwm& pwm) :
				Aoz1284(enable),
				AdjustableControl(pwm, 0.8f, 200.f, 10.f, 33.f, 33.f)
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
