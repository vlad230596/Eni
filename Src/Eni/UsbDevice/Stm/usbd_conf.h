#ifndef USB_CONF_H_
#define USB_CONF_H_
#include <stdint.h>
#include <stm32f4xx_hal.h>

#define USBD_MAX_NUM_INTERFACES     1
#define USBD_MAX_NUM_CONFIGURATION     1
#define USBD_MAX_STR_DESC_SIZ     512
#define USBD_SUPPORT_USER_STRING     1
#define USBD_DEBUG_LEVEL     0
#define USBD_SELF_POWERED     1
#define USBD_SUPPORT_USER_STRING_DESC 1

#endif

