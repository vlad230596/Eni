#include "EniConfig.h"

#if defined(ENI_USB_DEVICE) && defined(ENI_STM)

#include "UsbDDevice.h"
#include "usbd_conf.h"
#include "Core/usbd_def.h"
#include "Core/usbd_ioreq.h"
#include "Core/usbd_ctlreq.h"

#include "Core/usbd_core.h"
#include ENI_HAL_INCLUDE_FILE
#include "USBTypes.h"
#include "UsbMicrosoftTypes.h"
#include <type_traits>
#include <new>

using namespace Eni;

extern "C" {
	extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
}

namespace Eni::USB {
	__attribute__((used)) USBD_HandleTypeDef UsbDevice::hUsbDevice{};
	__attribute__((used)) UsbDDeviceContext* _context = nullptr;




//For other-speed description
__ALIGN_BEGIN volatile USB::DeviceQualifierDescriptor USBD_NDC_DeviceQualifierDesc __ALIGN_END {
	USB::UsbVersion(2),
	USB::UsbClass::Device::UseInterfaceClass(),
	64,
	1,
	0
};

#define USB_VENDOR_CODE_WINUSB 'P'

__ALIGN_BEGIN volatile USB::Microsoft::MicrosoftStringDescriptor NDC_StringDescriptor __ALIGN_END = {
	(uint8_t)USB_VENDOR_CODE_WINUSB
};


extern volatile USB::DeviceQualifierDescriptor USBD_NDC_DeviceQualifierDesc;
extern volatile USB::Microsoft::MicrosoftStringDescriptor NDC_StringDescriptor;

__attribute__((used)) std::aligned_storage_t<USBD_MAX_STR_DESC_SIZ, 4> _descriptorsBuffer;


UsbDDeviceContext* UsbDevice::getContext(){
	return _context;
}



void hang(){
	while(true){
		asm("nop");
	}
}


__attribute__((used))  const USBD_DescriptorsTypeDef UsbDevice::_descriptorsTable = {
	&UsbDevice::getDeviceDescriptor,
	&UsbDevice::getLangidStrDescriptor,
	&UsbDevice::getManufacturerStringDescriptor,
	&UsbDevice::getProductStringDescriptor,
	&UsbDevice::getSerialStringDescriptor,
	&UsbDevice::getConfigStringDescriptor,
	&UsbDevice::getInterfaceStringDescriptor
};

__attribute__((used))  const USBD_ClassTypeDef UsbDevice::_usbClassBinding = {
	&UsbDevice::coreInit,
	&UsbDevice::coreDeinit,
	&UsbDevice::coreSetup,
	0, //USBD_NDC_EP0_TxReady,
	&UsbDevice::coreEp0RxReady,
	&UsbDevice::coreDataIn,
	&UsbDevice::coreDataOut,
	&UsbDevice::coreSof,
	&UsbDevice::coreIsoInIncomplete,
	&UsbDevice::coreIsoOutIncomplete,
	&UsbDevice::coreGetCfgDesc,
	&UsbDevice::coreGetCfgDesc,
	&UsbDevice::coreGetCfgDesc,
	&UsbDevice::coreGetDeviceQualifierDesc,
	&UsbDevice::coreGetUserStringDesc
};


uint8_t UsbDevice::coreInit(USBD_HandleTypeDef* pdev, uint8_t cfgidx){
	//TODO: use configuration id
	for(size_t i = 0; i < _context->getInterfaceCount(); ++i){
		auto interface = _context->getInterface(i);
		if(interface != nullptr){
			if(!interface->init(pdev)){
				return USBD_FAIL;
			}
		}
	}
	return USBD_OK;
}


uint8_t UsbDevice::coreDeinit(USBD_HandleTypeDef* pdev, uint8_t cfgidx){
	//TODO: use configuration id

	for(size_t i = 0; i < _context->getInterfaceCount(); ++i){
		auto interface = _context->getInterface(i);
		if(interface != nullptr){
			if(!interface->deinit(pdev)){
				//return USBD_FAIL;
			}
		}
	}
	return USBD_OK;
}

uint8_t UsbDevice::coreImplSetup(USBD_SetupReqTypedef request, void* data){
	switch ( request.bmRequest & USB_REQ_RECIPIENT_MASK ){
	case USB_REQ_RECIPIENT_INTERFACE:{
		if(_context != nullptr){
			auto* interface = _context->getInterface(request.wValue);
			if(interface != nullptr){
				hang();
				/*if(interface->control(&hUsbDevice, request.bRequest, (uint8_t*)data, request.wLength)){
					return USBD_OK;
				}*/
			}
		}
		break;
	}

	case USB_REQ_RECIPIENT_ENDPOINT:
		hang();
		/*if(_usb_device_context != nullptr){
			if(request.bRequest == USB_REQ_CLEAR_FEATURE){ //reset pipe is called at host side
				//do reset pipe
				if(_clearFeatureCallback != nullptr){
					_clearFeatureCallback(request.wIndex);
				}
			}
		}*/
		break;
	case USB_REQ_RECIPIENT_DEVICE:
	default:
		break;
	}
	return USBD_OK;
}


uint8_t UsbDevice::coreSetup(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef *req){
	hang();
	/*if (req->wLength){
		//Request with data stage{
		if((req->bmRequest & USB_REQ_DATA_PHASE_MASK) == USB_REQ_DATA_PHASE_DEVICE_TO_HOST){
			//device to host data stage => handler should send data
			return coreImplSetup(*req, 0);
		}else{ //host to device data stage! Can't execute now, read data first & execute later in Ep0Receive callback
			last_request = *req;
			USBD_CtlPrepareRx (pdev, (uint8_t*)&ep0Buffer[0], req->wLength);
		}
	} else {//No data stage => simple request => execute now
		return coreImplSetup(*req, 0);
	}*/
	return USBD_OK;
}

 
 
uint8_t  UsbDevice::coreEp0RxReady(USBD_HandleTypeDef* pdev){
	hang();
	//coreImplSetup(last_request, &ep0Buffer[0]); //data in stage complete => execute request
	//last_request.bRequest = 0xff;
	return USBD_OK;
}



UsbDInterface* UsbDevice::findInterfaceByEndpointAddress(uint8_t address){
	if(_context == nullptr){
		return nullptr;
	}
	auto if_cnt = _context->getInterfaceCount();
	for(uint32_t i = 0; i < if_cnt; ++i){
		auto interface = _context->getInterface(i);
		if(interface != nullptr){
			auto endpoint = interface->getEndpoint(address);
			if(endpoint != nullptr){
				return interface;
			}
		}
	}
	return nullptr;
}
 


uint8_t  UsbDevice::coreDataIn(USBD_HandleTypeDef* pdev, uint8_t epnum){
	auto interface = findInterfaceByEndpointAddress(USB::EndpointAddress::makeIn(epnum));//TODO: cleanup
	if(interface != nullptr){
		interface->txComplete(epnum | 0x80);
	}
    return USBD_OK;
}
uint8_t  UsbDevice::coreDataOut(USBD_HandleTypeDef* pdev, uint8_t epnum){

	uint32_t rxLen = USBD_LL_GetRxDataSize (pdev, epnum);
	auto interface = findInterfaceByEndpointAddress(USB::EndpointAddress::makeOut(epnum));
	if(interface != nullptr){
		interface->rxComplete(rxLen, epnum);
	}
	return USBD_OK;
}
uint8_t  UsbDevice::coreSof(USBD_HandleTypeDef* pdev){
	hang();
	return USBD_OK;
}

uint8_t  UsbDevice::coreIsoInIncomplete(USBD_HandleTypeDef* pdev, uint8_t epnum){
	hang();
	return USBD_OK;
}

uint8_t  UsbDevice::coreIsoOutIncomplete(USBD_HandleTypeDef* pdev, uint8_t epnum){
	hang();
	return USBD_OK;
}

uint8_t* UsbDevice::coreGetCfgDesc(uint16_t* length){
	auto* mem = reinterpret_cast<uint8_t*>(&_descriptorsBuffer);
	auto* buffer = mem;
	USB::ConfigurationDescriptor* cd = new(buffer) USB::ConfigurationDescriptor();
	cd->wTotalLength 		= 0;
	cd->bNumInterfaces 		= (uint8_t)_context->getInterfaceCount();
	cd->bConfigurationValue = 0x01;
	cd->iConfiguration 		= USBD_IDX_CONFIG_STR;
	cd->bmAttributes 		= USB::UsbAttributes().value;
	cd->bMaxPower 			= 500;
	buffer += sizeof(USB::ConfigurationDescriptor);

	for(uint32_t i = 0; i < _context->getInterfaceCount(); ++i){
		auto emptySize = (mem + sizeof(_descriptorsBuffer)) - buffer;
		auto size = _context->getInterface(i)->getDescriptor(buffer, emptySize, i);
		buffer += size;
		cd->wTotalLength += size;
	}
	cd->wTotalLength += sizeof(USB::ConfigurationDescriptor);

	*length = cd->wTotalLength;
	return reinterpret_cast<uint8_t*>(&_descriptorsBuffer);
}


uint8_t* UsbDevice::getDeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	auto mem = reinterpret_cast<uint8_t*>(&_descriptorsBuffer);
	auto& desc = *new(mem) USB::DeviceDescriptor();
	*length = sizeof(USB::DeviceDescriptor);

	desc.bcdUSB = 0x0200;
	desc.classDescription = USB::UsbClassDescriptor(0,0,0);
	desc.bMaxPacketSize0 = 64;
	desc.idVendor = _context->vid;
	desc.idProduct = _context->pid;
	desc.bcdDevice = USB::UsbVersion(2);
	desc.iManufacturer = USBD_IDX_MFC_STR;
	desc.iProduct = USBD_IDX_PRODUCT_STR;
	desc.iSerialNumber = USBD_IDX_SERIAL_STR;
	desc.bNumConfigurations = 1;
	return reinterpret_cast<uint8_t*>(&desc);
}

struct USBDDummyClassData{
	uint32_t reserved;
};



__attribute__((used)) static USBDDummyClassData _classData = {};

void UsbDevice::start(UsbDDeviceContext* context){
	_context = context;

	hUsbDevice.pClassData = &_classData; //Otherwise USBD_Reset handler would not disable interfaces ((
	//hUsbDevice.pClassData = nullptr; //Init?
	hUsbDevice.dev_speed = USBD_SPEED_FULL;

	USBD_Init(&hUsbDevice, const_cast<USBD_DescriptorsTypeDef*>(&_descriptorsTable), 0);

	USBD_RegisterClass(&hUsbDevice, const_cast<USBD_ClassTypeDef*>(&_usbClassBinding));
	USBD_Start(&hUsbDevice);

}
uint8_t* UsbDevice::coreGetDeviceQualifierDesc (uint16_t *length){
	*length = sizeof (USBD_NDC_DeviceQualifierDesc);
	return (uint8_t*)&USBD_NDC_DeviceQualifierDesc;
}

uint8_t* UsbDevice::coreGetUserStringDesc(USBD_HandleTypeDef* pdev, uint8_t index, uint16_t* length){
	*length = 0;
	if ( 0xEE == index ){
		*length = sizeof (NDC_StringDescriptor);
		return (uint8_t*)&NDC_StringDescriptor;
	}
	return NULL;
}

uint8_t* UsbDevice::getLangidStrDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	auto mem = static_cast<void*>(&_descriptorsBuffer);
	auto& desc = *new(mem) USB::LanguageIDStringDescriptor<1>();
	*length = sizeof(USB::LanguageIDStringDescriptor<1>);
	desc.languages[0] = USB::LanguageID::EnglishUnitedStates;
	return reinterpret_cast<uint8_t*>(&desc);
}
uint8_t* UsbDevice::getManufacturerStringDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	return USB::MakeStringDescriptor(_context->manufacturerStringDescriptor, &_descriptorsBuffer, sizeof(_descriptorsBuffer), length);
}
uint8_t* UsbDevice::getProductStringDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	return USB::MakeStringDescriptor(_context->productStringDescriptor, &_descriptorsBuffer, sizeof(_descriptorsBuffer), length);
}


uint8_t* UsbDevice::getSerialStringDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	return USB::MakeStringDescriptor(_context->serialStringDescriptor, &_descriptorsBuffer, sizeof(_descriptorsBuffer), length);
}
uint8_t* UsbDevice::getConfigStringDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	return USB::MakeStringDescriptor(_context->configurationStringDescriptor, &_descriptorsBuffer, sizeof(_descriptorsBuffer), length);
}
uint8_t* UsbDevice::getInterfaceStringDescriptor(USBD_SpeedTypeDef speed, uint16_t* length){
	return USB::MakeStringDescriptor(_context->interfaceStringDescriptor, &_descriptorsBuffer, sizeof(_descriptorsBuffer), length);
}


USBD_StatusTypeDef UsbDevice::interfaceRequest(USBD_HandleTypeDef* pdev, USBD_SetupReqTypedef* req){
	uint8_t interface_id = (uint8_t)req->wValue; //TODO: bug! wIndex == interface id
	auto interface = _context->getInterface(interface_id);
	if(interface == nullptr){
		return USBD_FAIL;
	}
	return interface->interfaceRequest(pdev, req);
}

}

#endif

