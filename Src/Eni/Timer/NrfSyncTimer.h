#pragma once

#include <nrf.h>
#include <cmath>
#include <nrfx_ppi.h>
#include <nrfx_gpiote.h>
#include <nrfx_timer.h>
#include <Eni/Gpio/Gpio.h>
#include <Eni/Debug/Debug.h>
#include <Eni/Threading/CriticalSection.h>



namespace Eni {


	struct SyncTimer {

		struct Time {
			uint64_t hi;
			uint32_t lo;
		};

		static constexpr uint64_t ToGlobalTicks(const Time& localTime) {
			return (static_cast<uint64_t>(localTime.hi) * ReloadValue + localTime.lo) * 1000 / 16;
		}

		static constexpr uint64_t ToLocalTicks(uint64_t globalTicks) {
			return globalTicks / 125 * 2;
		}

		static constexpr int64_t ToLocalTicks(int64_t globalTicks) {
			return globalTicks / 125 * 2;
		}

		static constexpr nrf_timer_cc_channel_t ReloadChannel = NRF_TIMER_CC_CHANNEL0;
		static constexpr nrf_timer_cc_channel_t CurrentValueChannel = NRF_TIMER_CC_CHANNEL1;
		static constexpr nrf_timer_cc_channel_t ExternalLatchChannel = NRF_TIMER_CC_CHANNEL2;

		static constexpr uint32_t ReloadValue = 20000 * 16;
		static constexpr uint32_t MinUpdateValue = 1000 * 16;


		SyncTimer(nrfx_timer_t timer) :
			_timer(timer)
		{

			nrfx_timer_config_t config;
			config.frequency = NRF_TIMER_FREQ_16MHz;
			config.mode = NRF_TIMER_MODE_TIMER;
			config.bit_width = NRF_TIMER_BIT_WIDTH_32;
			config.interrupt_priority = 5;
			config.p_context = this;
			nrfx_timer_init(&_timer, &config, timerHandlerSt);

			nrfx_timer_extended_compare(&_timer, ReloadChannel, ReloadValue, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

			eniAssert(nrfx_ppi_channel_alloc(&_externalLatchPpi0) == NRFX_SUCCESS);
		}

		~SyncTimer() {
			nrfx_timer_uninit(&_timer);
		}

		uint32_t getTaskAddress(nrf_timer_task_t task) {
			return nrfx_timer_task_address_get(&_timer, task);
		}

		Time getCurrentNativeTime() {
			Time result;
			Eni::Threading::CriticalSection lock;
			bool hiUpdate0 = nrf_timer_event_check(_timer.p_reg, NRF_TIMER_EVENT_COMPARE0);
			result.hi = _hiPart;
			result.lo = nrfx_timer_capture(&_timer, CurrentValueChannel);
			bool hiUpdate1 = nrf_timer_event_check(_timer.p_reg, NRF_TIMER_EVENT_COMPARE0);

			if(hiUpdate0 || hiUpdate1){
				result.hi++;
				result.lo = nrfx_timer_capture(&_timer, CurrentValueChannel);
			}
			return result;
		}

		uint64_t getCurrentTicks() {
			return ToGlobalTicks(getCurrentNativeTime());
		}

		uint64_t getExternalLatchedTicks() {
			auto latched = nrfx_timer_capture_get(&_timer, ExternalLatchChannel);
			auto current = getCurrentNativeTime();

			auto result = current;
			result.lo = latched;

			if(current.lo < latched) {
				--result.hi;
			}
			return ToGlobalTicks(result);
		}

		void correct(int64_t offset) {
			Eni::Threading::CriticalSection lock;
			_correctionOffset = ToLocalTicks(offset);
		}

		bool setExternalLatchEvent(uint32_t externalLatchEvent) {
			nrf_ppi_channel_endpoint_setup(_externalLatchPpi0,
					externalLatchEvent,
					getTaskAddress(NRF_TIMER_TASK_CAPTURE2));
			nrf_ppi_channel_enable(_externalLatchPpi0);
			return true;
		}

		bool clearExternalLatchEvent() {
			nrf_ppi_channel_disable(_externalLatchPpi0);
			return true;
		}

		bool start() {
			nrfx_timer_enable(&_timer);
			return true;
		}


	private:

		void timerHandler(nrf_timer_event_t eventType) {
			if(eventType == NRF_TIMER_EVENT_COMPARE0) {
				++_hiPart;

				int64_t correctionH = _correctionOffset / ReloadValue;
				int32_t correctionL = _correctionOffset - (correctionH * ReloadValue);

				_hiPart += correctionH;


				auto newUpdateValue = ReloadValue - correctionL;
				_correctionOffset = 0;

				if(newUpdateValue < MinUpdateValue) {
					_correctionOffset = MinUpdateValue - newUpdateValue;
					newUpdateValue = MinUpdateValue;
				}
				nrf_timer_cc_write(_timer.p_reg, ReloadChannel, newUpdateValue);
			}
		}

		static void timerHandlerSt(nrf_timer_event_t eventType, void* context) {
			reinterpret_cast<SyncTimer*>(context)->timerHandler(eventType);
		}
	private:
		nrfx_timer_t _timer;
		nrf_ppi_channel_t _externalLatchPpi0;
		int64_t _correctionOffset {};

		uint32_t _hiPart = 0;
	};

}
