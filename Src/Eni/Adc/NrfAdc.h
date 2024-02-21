#pragma once

#include <cstdint>
#include <cstddef>

#include <nrfx_saadc.h>
#include <nrf_gpio.h>

#include <Eni/Debug/EniAssert.h>
#include <Eni/Threading/CriticalSection.h>

namespace Eni {

	class Adc {
		public:

		static constexpr nrf_saadc_input_t GpioToChannel(uint32_t pin) {
			#if defined(NRF52840_XXAA) || defined(NRF52832_XXAA) || defined(NRF52833_XXAA)
				switch(pin) {
					case NRF_GPIO_PIN_MAP(0, 2): return NRF_SAADC_INPUT_AIN0;
					case NRF_GPIO_PIN_MAP(0, 3): return NRF_SAADC_INPUT_AIN1;
					case NRF_GPIO_PIN_MAP(0, 4): return NRF_SAADC_INPUT_AIN2;
					case NRF_GPIO_PIN_MAP(0, 5): return NRF_SAADC_INPUT_AIN3;
					case NRF_GPIO_PIN_MAP(0, 28): return NRF_SAADC_INPUT_AIN4;
					case NRF_GPIO_PIN_MAP(0, 29): return NRF_SAADC_INPUT_AIN5;
					case NRF_GPIO_PIN_MAP(0, 30): return NRF_SAADC_INPUT_AIN6;
					case NRF_GPIO_PIN_MAP(0, 31): return NRF_SAADC_INPUT_AIN7;
				}
				return NRF_SAADC_INPUT_DISABLED;
			#elif defined(NRF52820_XXAA)
				#error "NRF52820_XXAA doesn't support analogs pin"
			#else
				#error "gpioToChannel missing for current platform"
			#endif
			}

			class ResistorDivider {
			public:
				ResistorDivider(Adc& adc, uint32_t pin, float Rtop = 0, float Rbottom = 0) :
					_adc(adc),
					_channel(GpioToChannel(pin)),
					_gain((Rtop + Rbottom) / Rbottom)
				{
					if(Rtop == 0 && Rbottom == 0) {
						_gain = 1;
					}
				}

				float getVoltage() {
					return _adc.getRawVoltage(_channel) * _gain;
				}

			private:
				Adc& _adc;
				nrf_saadc_input_t _channel;
				float _gain;
			};


			Adc() {
				nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);

				_channel.channel_config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
				_channel.channel_config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
				_channel.channel_config.gain = NRF_SAADC_GAIN1_5;
				_channel.channel_config.reference = NRF_SAADC_REFERENCE_INTERNAL;
				_channel.channel_config.acq_time = NRF_SAADC_ACQTIME_40US;
				_channel.channel_config.mode = NRF_SAADC_MODE_SINGLE_ENDED;
				_channel.channel_config.burst = NRF_SAADC_BURST_DISABLED;
			    _channel.channel_config.pin_n = NRF_SAADC_INPUT_DISABLED;

			    _channel.pin_n = _channel.channel_config.pin_n;
			    _channel.channel_index = 0;
			}

			~Adc() {
			    nrfx_saadc_uninit();
			}

	        float getRawVoltage(nrf_saadc_input_t input) {
	            return getRawValue(input) * _vref / _maxValue;
	        }


			size_t getRawValue(nrf_saadc_input_t input) {
				Eni::Threading::CriticalSection lock;
			    _channel.channel_config.pin_p = input;
			    _channel.pin_p = _channel.channel_config.pin_p;

				eniAssert(nrfx_saadc_channels_config(&_channel, 1) == NRFX_SUCCESS);
				eniAssert(nrfx_saadc_simple_mode_set(1 << 0, NRF_SAADC_RESOLUTION_10BIT, NRF_SAADC_OVERSAMPLE_DISABLED, nullptr) == NRFX_SUCCESS);
				nrf_saadc_value_t value;

				eniAssert(nrfx_saadc_buffer_set(&value, 1) == NRFX_SUCCESS);
				eniAssert(nrfx_saadc_mode_trigger() == NRFX_SUCCESS);

				if(value < 0) {
					value = 0;
				}
				return static_cast<size_t>(value);
			}

		private:
			nrfx_saadc_channel_t _channel;
			float _vref = 3.0f;//Todo add calculation from gain
			float _maxValue = 1023.f;//Todo add calculation from Resolution
		};

}

