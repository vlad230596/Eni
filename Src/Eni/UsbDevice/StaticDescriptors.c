#include <EniConfig.h>

#if defined(ENI_USB_DEVICE)

#include "Descriptors.h"

#if defined(ENI_NRF)
	#include "app_usbd_descriptor.h"
	#define USB_DESC_TYPE_DEVICE_QUALIFIER APP_USBD_DESCRIPTOR_DEVICE_QUALIFIER
	#define USB_DESC_TYPE_STRING APP_USBD_DESCRIPTOR_STRING
	#define AF_ALIGN_BEGIN
	#define AF_ALIGN_END
#elif
#error "Not supported platform"
#endif

AF_ALIGN_BEGIN USBD_DeviceQualifierDescriptor USBD_NDC_DeviceQualifierDesc AF_ALIGN_END = {
	.bLength 			= sizeof(USBD_DeviceQualifierDescriptor),
	.bDescriptorType 	= USB_DESC_TYPE_DEVICE_QUALIFIER,
	.bcdUSB 			= 0x0200,
	.bDeviceClass 		= 0x00,
	.bDeviceSubClass 	= 0x00,
	.bDeviceProtocol 	= 0x00,
	.bMaxPacketSize0 	= 0x40,
	.bNumConfigurations = 0x01,
	.bReserved 			= 0x00
};

AF_ALIGN_BEGIN USBD_MsStringDescriptor NDC_StringDescriptor AF_ALIGN_END = {
	.bLength = sizeof(NDC_StringDescriptor),
	.bDescriptorType = USB_DESC_TYPE_STRING,
	.qwSignature = {
		'M', 0x00,
		'S', 0x00,
		'F', 0x00,
		'T', 0x00,
		'1', 0x00,
		'0', 0x00,
		'0', 0x00,
	},
	.bMS_VendorCode = (uint8_t)USB_VENDOR_CODE_WINUSB,
	.bPad = 0x00
};

AF_ALIGN_BEGIN USBD_ExtendedCompatIdDesc NDC_ExtCompatIdOsDesc  AF_ALIGN_END = {
	.header = {
		.dwLength 	= sizeof(USBD_ExtendedCompatIdDesc),
		.bcdVersion = 0x0100,
		.wIndex 	= 0x04,
		.bCount 	= 1, //NDC_Interfaces_Count, TODO: fix this!
		.RESERVED 	= {0, 0, 0, 0, 0, 0, 0}
	},
	.function0 = {
		.bFirstInterfaceNumber 	= 0,
		.RESERVED0 				= 0,
		.compatibleID 			= {0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00}, //WINUSB
		.subCompatibleID 		= {0, 0, 0, 0, 0, 0, 0, 0},
		.RESERVED1 				= {0, 0, 0, 0, 0, 0}
	}
};

AF_ALIGN_BEGIN USBD_ExtendedCompatPropDesc NDC_ExtCompatPropDesc AF_ALIGN_END = {
	.header = {
		.dwLength 	= sizeof(USBD_ExtendedCompatPropDesc),
		.bcdVersion = 0x0100,
		.wIndex 	= 0x0005,
		.wCount 	= 0x0001
	},
	.if0Guid = {
		.dwSize = sizeof(USBD_ExProperty_40_78),
		.dwPropertyDataType = USBD_EX_PROP_STRING,
		.wPropertyNameLength = 40,
		.bPropertyName = {
			'D',0, 'e',0, 'v',0, 'i',0, 'c',0, 'e',0, 'I',0, 'n',0,
			't',0, 'e',0, 'r',0, 'f',0, 'a',0, 'c',0, 'e',0, 'G',0,
			'U',0, 'I',0, 'D',0, 0,0
		},
		.dwPropertyDataLength = 78,
		.bPropertyData = {
			'{',0, 'E',0, 'E',0, 'E',0, 'E',0, 'E',0, 'E',0, 'E',0,
			'3',0, '-',0, '5',0, '0',0, 'F',0, '3',0, '-',0, '4',0,
			'8',0, '8',0, '8',0, '-',0, '8',0, '4',0, 'B',0, '4',0,
			'-',0, '7',0, '4',0, 'E',0, '5',0, '0',0, 'E',0, '1',0,
			'6',0, '4',0, '9',0, 'D',0, 'B',0, '}',0,  0 ,0 //{EEEEEEE3-50F3-4888-84B4-74E50E1649DB}
		}
	}
};

#endif //AF_USB
