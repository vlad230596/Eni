#pragma once

#include <EniConfig.h>

#if defined(ENI_NRF) && defined(ENI_USB_DEVICE)

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_power.h"

#include "nrf_drv_usbd.h"
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"

#include "sdk_errors.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#define USB_RAW_EPIN NRF_DRV_USBD_EPIN1
#define USB_RAW_EPOUT NRF_DRV_USBD_EPOUT1
#define USB_RAW_INTERFACE_ID 0

typedef enum  {
    APP_USBD_RAW_USER_EVT_SUSPEND = 0,
    APP_USBD_RAW_USER_EVT_RESUME,
    APP_USBD_RAW_USER_EVT_START,
    APP_USBD_RAW_USER_EVT_STOP,
    APP_USBD_RAW_USER_EVT_RX_DONE,
    APP_USBD_RAW_USER_EVT_TX_DONE,
	APP_USBD_RAW_USER_EVT_RESET,
	APP_USBD_RAW_USER_EVT_STATE_CHANGED
} UsbdRawUserEvent;

typedef void (*UserEventHandlerTypedef)(UsbdRawUserEvent event, nrf_drv_usbd_ep_t ep, void* context);

void initUSB();
void startUSB(const char* serialNumber, const char* productName);
void stopUSB();



void setUserEventHandler(UserEventHandlerTypedef handler, void* context);


#ifdef __cplusplus
}
#endif

#endif
