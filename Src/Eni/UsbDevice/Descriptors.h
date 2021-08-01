#pragma once

#include <stdint.h>

#define USB_VENDOR_CODE_WINUSB 'P'

#pragma pack(push, 1)

typedef struct {
	uint8_t bLength; //Size of this descriptor in bytes.
	uint8_t bDescriptorType; //Device Descriptor Type = 1.
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0; //Maximum packet size for Endpoint zero (only 8, 16, 32, or 64 are valid).
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t iManufacturer;
	uint8_t iProduct;
	uint8_t iSerialNumber;
	uint8_t bNumConfigurations;
}USBD_DeviceDescriptor;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wTotalLength;
	uint8_t bNumInterfaces;
	uint8_t bConfigurationValue;
	uint8_t iConfiguration;
	uint8_t bmAttributes;
	uint8_t bMaxPower;
}USBD_ConfigurationDescriptor;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
}USBD_InterfaceDescriptor;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bEndpointAddress;
	uint8_t bmAttributes;
	uint16_t wMaxPacketSize;
	uint8_t bInterval;
}USBD_EndpointDescriptor;

typedef struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t bcdUSB;
	uint8_t bDeviceClass;
	uint8_t bDeviceSubClass;
	uint8_t bDeviceProtocol;
	uint8_t bMaxPacketSize0;
	uint8_t bNumConfigurations;
	uint8_t bReserved;
}USBD_DeviceQualifierDescriptor;

typedef struct{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bFirstInterface;
	uint8_t bInterfaceCount;
	uint8_t bFunctionClass;
	uint8_t bFunctionSubClass;
	uint8_t bFunctionProtocol;
	uint8_t iFunction;
}USBD_InterfaceAssociationDescriptor;

typedef struct{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t qwSignature[14];
	uint8_t bMS_VendorCode;
	uint8_t bPad;
}USBD_MsStringDescriptor;


typedef struct{
	uint32_t dwLength;
	uint16_t bcdVersion;
	uint16_t wIndex;
	uint8_t bCount;
	uint8_t RESERVED[7];
}USBD_ExtendedCompatIdDescHeader;

typedef struct{
	uint8_t bFirstInterfaceNumber;
	uint8_t RESERVED0;
	uint8_t compatibleID[8];
	uint8_t subCompatibleID[8];
	uint8_t RESERVED1[6];
}USBD_ExtendedCompatIdDescFunction;

typedef struct{
	uint32_t dwLength;
	uint16_t bcdVersion;
	uint16_t wIndex;
	uint16_t wCount;
}USBD_ExtendedPropertyDescHeaderSection;

enum USBD_ExtendedPropertyDataType{
	USBD_EX_PROP_STRING 			= 1,
	USBD_EX_PROP_STRING_INCL_ENV 	= 2,
	USBD_EX_PROP_BINARY 			= 3,
	USBD_EX_PROP_LE_INT32 			= 4,
	USBD_EX_PROP_BE_INT32 			= 5,
	USBD_EX_PROP_STRING_INCL_LINK 	= 6,
	USBD_EX_PROP_STRING_ARRAY 		= 7
};

typedef struct{
	USBD_ExtendedCompatIdDescHeader header;
	USBD_ExtendedCompatIdDescFunction function0;
}USBD_ExtendedCompatIdDesc;

#define USBD_DefinePropertySection(__NameLength, __DataLength)\
typedef struct{\
	uint32_t dwSize;\
	uint32_t dwPropertyDataType;\
	uint16_t wPropertyNameLength;\
	uint8_t bPropertyName[__NameLength];\
	uint32_t dwPropertyDataLength;\
	uint8_t bPropertyData[__DataLength];\
}USBD_ExProperty_##__NameLength##_##__DataLength;

//properties desc
USBD_DefinePropertySection(40, 78)

typedef struct{
	USBD_ExtendedPropertyDescHeaderSection header;
	USBD_ExProperty_40_78 if0Guid;
}USBD_ExtendedCompatPropDesc;

#pragma pack(pop)

extern USBD_ExtendedCompatIdDesc NDC_ExtCompatIdOsDesc;
extern USBD_ExtendedCompatPropDesc NDC_ExtCompatPropDesc;
