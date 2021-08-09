#pragma once

#include <cstdint>
#include <cstddef>

#include <array>
#include <initializer_list>

#include <nrfx_pwm.h>

#include <Eni/Debug/Assert.h>

namespace Eni {

	class BasePwm {
	public:
        static constexpr uint16_t RisingEdgePolarity = (0u << 15);
        static constexpr uint16_t FallingEdgePolarity = (1u << 15);

        static constexpr size_t MaxPinsCount = 4;
        static constexpr uint16_t MaxTopValue = (std::numeric_limits<int16_t>::max)();

        BasePwm(nrfx_pwm_t instance, nrf_pwm_clk_t clk = NRF_PWM_CLK_125kHz, uint16_t counterTop = 255, const std::initializer_list<uint32_t>& pins = {}) :
			_instance(instance)
		{
		    setPins(pins);

			_config.irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY;
			_config.base_clock = clk;
			_config.count_mode = NRF_PWM_MODE_UP;
			_config.top_value = counterTop;
			_config.load_mode    = NRF_PWM_LOAD_INDIVIDUAL;
			_config.step_mode    = NRF_PWM_STEP_AUTO;
		}

		void setClk(nrf_pwm_clk_t clk) {
			_config.base_clock = clk;
		}

		void setTopValue(uint16_t counterTop) {
			_config.top_value = counterTop;
		}

		uint16_t getTopValue() const {
			return _config.top_value;
		}

        void setPins(const std::initializer_list<uint32_t>& pins) {
        	eniAssert(pins.size() <= MaxPinsCount);
            for(size_t i = 0; i < MaxPinsCount; ++i) {
                if(i < pins.size()) {
                	_config.output_pins[i] = *(pins.begin() + i);
                } else {
                	_config.output_pins[i] = NRFX_PWM_PIN_NOT_USED;
                }
            }
        }

        void setPin(size_t index, uint32_t pin) {
        	eniAssert(index < MaxPinsCount);
        	_config.output_pins[index] = pin;
        }

	protected:
        bool applyConfig() {
			return nrfx_pwm_init(&_instance, &_config, nullptr) == NRF_SUCCESS;
        }

	protected:
		nrfx_pwm_config_t _config;
		nrfx_pwm_t _instance;
	};


	class Pwm : public BasePwm {
	public:

        using Intensity = std::array<uint16_t, MaxPinsCount>;

		Pwm(nrfx_pwm_t instance, nrf_pwm_clk_t clk = NRF_PWM_CLK_125kHz, uint16_t counterTop = 255, const std::initializer_list<uint32_t>& pins = {}) :
			BasePwm(instance, clk, counterTop, pins)
		{
			_sequence.values.p_individual = reinterpret_cast<const nrf_pwm_values_individual_t*>(_values.data());
			_sequence.length = _values.size();
			_sequence.repeats = 0;
			_sequence.end_delay = 0;
		}

		bool used() const {
			return _used;
		}

		bool start() {
			if(!applyConfig()) {
			    return false;
			}
			_used = true;
			return nrfx_pwm_simple_playback(&_instance, &_sequence, 1, NRFX_PWM_FLAG_LOOP) == NRF_SUCCESS;
		}

		void stop() {
			if(_used) {
				auto status = nrfx_pwm_stop(&_instance, true);
				static_cast<void>(status);
				nrfx_pwm_uninit(&_instance);
			}
			_used = false;
		}

		void setIntensity(const Intensity& values) {
			_values = values;
		}

		Intensity getCurrentIntensity() const {
			return _values;
		}

	private:
		nrf_pwm_sequence_t _sequence;
		Intensity _values {};
		bool _used = false;
	};


}

