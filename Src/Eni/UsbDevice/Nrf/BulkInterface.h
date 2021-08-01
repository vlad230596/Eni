#pragma once

#include <EniConfig.h>

#if defined(ENI_NRF) && defined(ENI_USB_DEVICE)

#include "usb_raw.h"
#include <cstdint>

#include "app_util.h"

namespace Eni::UsbDevice {

class BulkInterface {
public:
	enum class Event {
		StateChanged,
		Suspend,
		Reset
	};
	struct EventListener {
		virtual ~EventListener() = default;

		virtual void onDataReceive(size_t size, bool status) = 0;//Todo on Transfer complete with endpoint
		virtual void onTxComplete(size_t size, bool status) = 0;

		virtual void onEvent(Event event) = 0;
	};

	BulkInterface() {
		setUserEventHandler(BulkInterface::eventHandlerStatic, this);
		initUSB();
	}


	void start(EventListener* eventListener, const char* serialNumber, const char* productName, uint16_t pid = 0) {
		_eventListener = eventListener;
		app_usbd_set_vid_pid(APP_USBD_VID, pid);
		startUSB(serialNumber, productName);
	}
	void stop()	{
		stopUSB();
		_eventListener = nullptr;
	}

	bool requestRx(uint8_t* data, size_t size) {
		nrf_drv_usbd_transfer_t transfer;
		transfer.p_data.rx = data;
		transfer.size = size;
		transfer.flags = 0;

		return app_usbd_ep_transfer(USB_RAW_EPOUT, &transfer) == NRF_SUCCESS;
	}

	bool requestTx(const uint8_t* data, size_t size) {
		nrf_drv_usbd_transfer_t transfer;
		transfer.p_data.tx = data;
		transfer.size = size;
		transfer.flags = 0;

		return app_usbd_ep_transfer(USB_RAW_EPIN, &transfer) == NRF_SUCCESS;
	}

private:
	static void eventHandlerStatic(UsbdRawUserEvent event, nrf_drv_usbd_ep_t ep, void* context) {
		reinterpret_cast<BulkInterface*>(context)->eventHandler(event, ep);
	}

	void eventHandler(UsbdRawUserEvent event, nrf_drv_usbd_ep_t ep) {
		switch(event){
		case APP_USBD_RAW_USER_EVT_SUSPEND:
			if(_eventListener) {
				_eventListener->onEvent(Event::Suspend);
			}
			break;
		case APP_USBD_RAW_USER_EVT_RX_DONE:
			if(_eventListener) {
				size_t size;
				if(nrf_drv_usbd_ep_status_get(ep, &size) == NRF_SUCCESS) {
					_eventListener->onDataReceive(size, true);
				} else {
					_eventListener->onDataReceive(0, false);
				}
			}
			break;
		case APP_USBD_RAW_USER_EVT_TX_DONE:
			if(_eventListener) {
				size_t size;
				if(nrf_drv_usbd_ep_status_get(ep, &size) == NRF_SUCCESS) {
					_eventListener->onTxComplete(size, true);
				} else {
					_eventListener->onTxComplete(0, false);
				}
			}
			break;
		case APP_USBD_RAW_USER_EVT_RESET:
			if(_eventListener) {
				_eventListener->onEvent(Event::Reset);
			}
			break;
		case APP_USBD_RAW_USER_EVT_START:
			break;
		case APP_USBD_RAW_USER_EVT_RESUME:
			break;
		case APP_USBD_RAW_USER_EVT_STATE_CHANGED:
			if(_eventListener) {
				_eventListener->onEvent(Event::StateChanged);
			}
			break;
		default:
			break;
		}
	}


private:
	EventListener* _eventListener {};
};

}

#endif
