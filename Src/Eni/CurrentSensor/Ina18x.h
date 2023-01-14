
#include <Eni/Adc/Adc.h>

namespace Eni {

	class Ina180 {
	public:

		enum class ProductOption {
			A1,
			A2,
			A3,
			A4
		};

		static constexpr float GetGain(ProductOption productOption) {
			switch(productOption) {
			case ProductOption::A1:
				return 20.f;
			case ProductOption::A2:
				return 50.f;
			case ProductOption::A3:
				return 100.f;
			case ProductOption::A4:
				return 200.f;
			}
			return 0.0f;
		}

		Ina180(Adc& adc, uint32_t pin, float Rsense, ProductOption productOption = ProductOption::A1) :
			_adc(adc),
			_channel(Adc::GpioToChannel(pin)),
			_divider(Rsense * GetGain(productOption))
		{}

		float getCurrent() const {
			return _adc.getRawVoltage(_channel) / _divider;
		}

	private:
		Adc& _adc;
		nrf_saadc_input_t _channel;
		float _divider;
	};
}
