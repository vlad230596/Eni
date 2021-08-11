#pragma once

#include <Eni/Pwm/Pwm.h>

namespace Eni::LoadControl {

	class AdjustableControl {
		static constexpr float Voh = 3.3f;//Assumed, that Vol = 0.f
	public:
		AdjustableControl(Eni::Pwm& pwm, float vref, float r1, float r2, float r3, float rfilter) :
			_pwm(pwm),
			_vref(vref),
			_r1(r1),
			_r2(r2),
			_r3(r3),
			_rfilter(rfilter)
		{

		}

		bool setVoltage(float target) {
			float vbase = _vref * (1.0f + _r1 / _r2);
			float duty = (_vref - (target - vbase) / _r1 * (_r3 + _rfilter)) / Voh;
			if(duty < 0.f || duty > 1.f) {
				return false;
			}
			_pwm.setIntensity({static_cast<uint16_t>(_pwm.getTopValue() * duty)});
			return true;
		}

	protected:
		Eni::Pwm& _pwm;

	private:
		float _vref = 0.6f;
		float _r1 = 17.4f;
		float _r2 = 10.0f;
		float _r3 = 15.4f;
		float _rfilter = 15.4f;
	};

}
