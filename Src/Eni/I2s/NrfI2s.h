#pragma once

#include <cstdint>
#include <cstddef>
#include <array>

#include <nrfx_i2s.h>
#include <Eni/Gpio/Gpio.h>

namespace Eni {
	template<size_t PartSize>
	class I2sSlave {
	public:
		struct IListener {
			virtual ~IListener() = default;
			virtual void onDataCaptured(const int32_t* data, size_t size) = 0;
		};

		I2sSlave(Eni::GpioPin sck, Eni::GpioPin lrck, Eni::GpioPin sdin) {
			nrfx_i2s_config_t config;
			config.sck_pin = sck.nativePinId;
			config.lrck_pin = lrck.nativePinId;
			config.mck_pin = NRFX_I2S_PIN_NOT_USED;
			config.sdout_pin = NRFX_I2S_PIN_NOT_USED;
			config.sdin_pin = sdin.nativePinId;
			config.irq_priority = 4; ///< Interrupt priority.

			config.mode = NRF_I2S_MODE_SLAVE;         ///< Mode of operation.
			config.format = NRF_I2S_FORMAT_I2S;       ///< Frame format.
			config.alignment = NRF_I2S_ALIGN_LEFT;    ///< Alignment of sample within a frame.
			config.sample_width = NRF_I2S_SWIDTH_24BIT; ///< Sample width.
			config.channels = NRF_I2S_CHANNELS_STEREO;     ///< Enabled channels.
			config.mck_setup = NRF_I2S_MCK_DISABLED;    ///< Master clock setup.
			config.ratio = NRF_I2S_RATIO_512X;        ///< MCK/LRCK ratio.
			config.context = this;
			nrfx_i2s_init(&config, i2sStatisDataHandler);
		}

		void start(IListener* listener) {
			nrfx_i2s_buffers_t initialBuffer;
			initialBuffer.p_rx_buffer = reinterpret_cast<uint32_t*>(_part0.data()); ///< Pointer to the buffer for received data.
			initialBuffer.p_tx_buffer = nullptr;

			_listener = listener;
			APP_ERROR_CHECK(nrfx_i2s_start(&initialBuffer, _part0.size(), 0));

			nrfx_i2s_buffers_t next_buffers;
			next_buffers.p_rx_buffer = reinterpret_cast<uint32_t*>(_part1.data());
			next_buffers.p_tx_buffer = nullptr;
			APP_ERROR_CHECK(nrfx_i2s_next_buffers_set(&next_buffers));
		}

		void stop() {
			nrfx_i2s_stop();
		}

	private:

		void i2sDataHandler(nrfx_i2s_buffers_t const* p_released, uint32_t status) {
			if (!(status & NRFX_I2S_STATUS_NEXT_BUFFERS_NEEDED)) {
				return;
			}

			if (p_released->p_rx_buffer) {
				APP_ERROR_CHECK(nrfx_i2s_next_buffers_set(p_released));
				if(_listener) {
					_listener->onDataCaptured(reinterpret_cast<int32_t*>(p_released->p_rx_buffer), PartSize);
				}
			}
		}

		static void i2sStatisDataHandler(nrfx_i2s_buffers_t const* p_released, uint32_t status, void* context) {
			reinterpret_cast<I2sSlave*>(context)->i2sDataHandler(p_released, status);
		}
	private:
	    std::array<int32_t, PartSize> _part0{};
	    std::array<int32_t, PartSize> _part1{};
	    IListener* _listener{};
	};
}
