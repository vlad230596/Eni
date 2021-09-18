#include <EniConfig.h>

#if defined(ENI_NRF) && defined(ENI_USB_DEVICE)

#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#include <Eni/UsbDevice/Descriptors.h>
#include "usb_raw.h"

#define SERIAL_NUMBER_STRING_SIZE (16)


char UsbRawSerialString[16 + 1] = "0";
char UsbRawProductString[64] = "ChangeMe";

UserEventHandlerTypedef userEventHandler;
void* userContext;

#define APP_USBD_RAW_CONFIG(iface_raw, epin_data, epout_data)   \
        ((iface_raw, epin_data, epout_data))


typedef struct {
	uint8_t const* p_raw_desc;
	size_t         raw_desc_size;
} UsbRawConstants;

#define USB_RAW_CONSTANTS UsbRawConstants inst;


APP_USBD_CLASS_FORWARD(app_usbd_raw);

APP_USBD_CLASS_TYPEDEF(app_usbd_raw,            \
		APP_USBD_RAW_CONFIG(0, 0, 0), \
		USB_RAW_CONSTANTS, \
		\
);



typedef struct UsbRawIterfaceDescriptor {
	USBD_InterfaceDescriptor ifdesc;
	USBD_EndpointDescriptor inEp;
	USBD_EndpointDescriptor outEp;
}UsbRawIterfaceDescriptor;

#pragma pack(push, 1)

static struct UsbRawIterfaceDescriptor rawInterfaceDescriptor = {
	.ifdesc = {
		.bLength 			= sizeof(USBD_InterfaceDescriptor),
		.bDescriptorType 	= APP_USBD_DESCRIPTOR_INTERFACE,
		.bInterfaceNumber 	= USB_RAW_INTERFACE_ID,
		.bAlternateSetting 	= 0x00,
		.bNumEndpoints 		= 2,
		.bInterfaceClass 	= 0xFF, //Vendor class
		.bInterfaceSubClass = 0x00,
		.bInterfaceProtocol = 0x00,
		.iInterface 		= 5//USBD_IDX_INTERFACE_STR;
	},
	.inEp = {
		.bLength 			= sizeof(USBD_EndpointDescriptor),
		.bDescriptorType 	= APP_USBD_DESCRIPTOR_ENDPOINT,
		.bEndpointAddress 	= USB_RAW_EPIN,
		.bmAttributes 		= APP_USBD_DESCRIPTOR_EP_ATTR_TYPE_BULK,
		.wMaxPacketSize 	= NRF_DRV_USBD_EPSIZE,
		.bInterval 			= 0
	},
	.outEp = {
		.bLength 			= sizeof(USBD_EndpointDescriptor),
		.bDescriptorType 	= APP_USBD_DESCRIPTOR_ENDPOINT,
		.bEndpointAddress 	= USB_RAW_EPOUT,
		.bmAttributes 		= APP_USBD_DESCRIPTOR_EP_ATTR_TYPE_BULK,
		.wMaxPacketSize 	= NRF_DRV_USBD_EPSIZE,
		.bInterval 			= 0
	}
};

#pragma pack(pop)


static inline app_usbd_raw_t const * rawGet(app_usbd_class_inst_t const * p_inst){
    ASSERT(p_inst != NULL);
    return (app_usbd_raw_t const *)p_inst;
}


static inline void callRawUserEventHandler(app_usbd_class_inst_t const* p_inst, UsbdRawUserEvent event, nrf_drv_usbd_ep_t ep){
	(void)p_inst;
	//ToDo ALT change
	if(userEventHandler != NULL)
		userEventHandler(event, ep, userContext);
}

static ret_code_t usbRawEndpointEventHandler(app_usbd_class_inst_t const* p_inst, app_usbd_complex_evt_t const* p_event) {
    if (NRF_USBD_EPIN_CHECK(p_event->drv_evt.data.eptransfer.ep)){
        switch (p_event->drv_evt.data.eptransfer.status) {
            case NRF_USBD_EP_OK:
                callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_TX_DONE, p_event->drv_evt.data.eptransfer.ep);
                return NRF_SUCCESS;
            case NRF_USBD_EP_ABORTED:
                return NRF_SUCCESS;
            default:
                return NRF_ERROR_INTERNAL;
        }
    }

    if (NRF_USBD_EPOUT_CHECK(p_event->drv_evt.data.eptransfer.ep)) {
        switch (p_event->drv_evt.data.eptransfer.status) {
            case NRF_USBD_EP_OK:
            	callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_RX_DONE, p_event->drv_evt.data.eptransfer.ep);
                return NRF_SUCCESS;
            case NRF_USBD_EP_WAITING:
            case NRF_USBD_EP_ABORTED:
                return NRF_SUCCESS;
            default:
                return NRF_ERROR_INTERNAL;
        }
    }

    return NRF_ERROR_NOT_SUPPORTED;
}


static ret_code_t usbdRawClassRequestIn(app_usbd_class_inst_t const* p_inst, app_usbd_setup_evt_t const*  p_setup_ev){
	(void)p_inst;
	(void)p_setup_ev;
    return NRF_ERROR_NOT_SUPPORTED;
}

static ret_code_t usbdRawClassRequestOut(app_usbd_class_inst_t const* p_inst, app_usbd_setup_evt_t const*  p_setup_ev){
	(void)p_inst;
	(void)p_setup_ev;
    return NRF_ERROR_NOT_SUPPORTED;
}

static ret_code_t usbdRawStdRequestOut(app_usbd_class_inst_t const* p_inst, app_usbd_setup_evt_t const*  p_setup_ev){
	(void)p_inst;
	switch (p_setup_ev->setup.bRequest){
        default:
            break;
    }
    return NRF_ERROR_NOT_SUPPORTED;
}

static ret_code_t usbdRawStdRequestIn(app_usbd_class_inst_t const* p_inst,  app_usbd_setup_evt_t const*  p_setup_ev){
    switch (p_setup_ev->setup.bRequest){
        case APP_USBD_SETUP_STDREQ_GET_DESCRIPTOR:{
            uint8_t desc[256];
            size_t dsc_len = sizeof(desc);
            ret_code_t res = app_usbd_class_descriptor_find(p_inst, p_setup_ev->setup.wValue.hb, p_setup_ev->setup.wValue.lb, &desc[0], &dsc_len);
            if (res == NRF_SUCCESS) {
                return app_usbd_core_setup_rsp(&(p_setup_ev->setup), desc, dsc_len);
            }
            break;
        }case APP_USBD_SETUP_STDREQ_GET_INTERFACE: {
            size_t tx_maxsize;
            uint8_t * p_tx_buff = app_usbd_core_setup_transfer_buff_get(&tx_maxsize);

            p_tx_buff[0] = 0;
            return app_usbd_core_setup_rsp(&p_setup_ev->setup,p_tx_buff,sizeof(uint8_t));
        }default:
            break;
    }

    return NRF_ERROR_NOT_SUPPORTED;
}

static ret_code_t usbdRawVendorRequestIn(app_usbd_class_inst_t const* p_inst, app_usbd_setup_evt_t const*  p_setup_ev){
	(void)p_inst;
	(void)p_setup_ev;
	return NRF_ERROR_NOT_SUPPORTED;
}
static ret_code_t usbdRawVendorRequestOut(app_usbd_class_inst_t const* p_inst, app_usbd_setup_evt_t const*  p_setup_ev){
	(void)p_inst;
	(void)p_setup_ev;
	return NRF_ERROR_NOT_SUPPORTED;
}

static ret_code_t unknownInterfaceRequestHandler(uint8_t iFace, app_usbd_complex_evt_t const* const p_setup){
	(void)iFace;
	const app_usbd_setup_evt_t* p_setup_ev = &(p_setup->setup_evt);
	if(p_setup_ev->setup.bRequest == (uint8_t)USB_VENDOR_CODE_WINUSB){
		if(p_setup_ev->setup.wIndex.w == 0x05){
			return app_usbd_core_setup_rsp(&p_setup_ev->setup, &NDC_ExtCompatPropDesc, sizeof(NDC_ExtCompatPropDesc));
		}
	}
	return NRF_ERROR_NOT_SUPPORTED;
}

static ret_code_t usbRawSetupHandler(app_usbd_class_inst_t const* p_inst, app_usbd_setup_evt_t const* p_setup_ev) {
    ASSERT(p_inst != NULL);
    ASSERT(p_setup_ev != NULL);

    app_usbd_setup_reqrec_t  req_rec = app_usbd_setup_req_rec(p_setup_ev->setup.bmRequestType);
    app_usbd_setup_reqtype_t req_type = app_usbd_setup_req_typ(p_setup_ev->setup.bmRequestType);


    if (req_rec == APP_USBD_SETUP_REQREC_ENDPOINT && req_type == APP_USBD_SETUP_REQTYPE_STD) {
        //return app_usbd_endpoint_std_req_handle(p_setup_ev);
        return NRF_ERROR_NOT_SUPPORTED;
    }


    if (app_usbd_setup_req_dir(p_setup_ev->setup.bmRequestType) == APP_USBD_SETUP_REQDIR_IN) {
        switch (app_usbd_setup_req_typ(p_setup_ev->setup.bmRequestType)) {
            case APP_USBD_SETUP_REQTYPE_STD:
                return usbdRawStdRequestIn(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_CLASS:
                return usbdRawClassRequestIn(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_VENDOR:
            	return usbdRawVendorRequestIn(p_inst, p_setup_ev);
            default:
                break;
        }
    } else /*APP_USBD_SETUP_REQDIR_OUT*/ {
        switch (app_usbd_setup_req_typ(p_setup_ev->setup.bmRequestType)) {
            case APP_USBD_SETUP_REQTYPE_STD:
                return usbdRawStdRequestOut(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_CLASS:
                return usbdRawClassRequestOut(p_inst, p_setup_ev);
            case APP_USBD_SETUP_REQTYPE_VENDOR:
            	return usbdRawVendorRequestOut(p_inst, p_setup_ev);
            default:
                break;
        }
    }

    return NRF_ERROR_NOT_SUPPORTED;
}


ret_code_t usbRawEventHandler(app_usbd_class_inst_t const* const p_inst, app_usbd_complex_evt_t const* const p_event){
	ASSERT(p_inst != NULL);
	ASSERT(p_event != NULL);
	ret_code_t ret = NRF_SUCCESS;
	switch (p_event->app_evt.type)
	{
		case APP_USBD_EVT_DRV_SOF:
			break;
		case APP_USBD_EVT_DRV_RESET:
			callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_RESET, p_event->drv_evt.data.eptransfer.ep);
			break;
		case APP_USBD_EVT_DRV_SETUP:
			ret = usbRawSetupHandler(p_inst, (app_usbd_setup_evt_t const *)p_event);
			break;
		case APP_USBD_EVT_DRV_EPTRANSFER:
			ret = usbRawEndpointEventHandler(p_inst, p_event);
			break;
		case APP_USBD_EVT_DRV_SUSPEND:
			callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_SUSPEND, p_event->drv_evt.data.eptransfer.ep);
			break;
		case APP_USBD_EVT_DRV_RESUME:
			callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_RESUME, p_event->drv_evt.data.eptransfer.ep);
			break;
		case APP_USBD_EVT_INST_APPEND:
		{
			//ret = app_usbd_class_sof_register(p_inst);
			break;
		}
		case APP_USBD_EVT_INST_REMOVE:
		{
			//ret = app_usbd_class_sof_unregister(p_inst);
			break;
		}
		case APP_USBD_EVT_STARTED:
			callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_START, p_event->drv_evt.data.eptransfer.ep);
			break;
		case APP_USBD_EVT_STOPPED:
			callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_STOP, p_event->drv_evt.data.eptransfer.ep);
			break;
		case APP_USBD_EVT_STATE_CHANGED:
			callRawUserEventHandler(p_inst, APP_USBD_RAW_USER_EVT_STATE_CHANGED, p_event->drv_evt.data.eptransfer.ep);
			break;
		default:
			ret = NRF_ERROR_NOT_SUPPORTED;
			break;
	}

	return ret;
}


 bool usbRawGetDescriptors (app_usbd_class_descriptor_ctx_t  * p_ctx,
 								   app_usbd_class_inst_t const      * p_inst,
								   uint8_t                          * p_buff,
								   size_t                             max_size){

 	app_usbd_raw_t const * p_raw = rawGet(p_inst);
	size_t descSize = p_raw->specific.inst.raw_desc_size;
	size_t availableSize = MIN(descSize - p_ctx->line, max_size);
	if(p_buff) {
		memcpy(p_buff, p_raw->specific.inst.p_raw_desc + p_ctx->line, availableSize);
	}
	if(availableSize) {
		p_ctx->line += availableSize;
		return true;
	}
	p_ctx->line = 0;
	return false;
 }


const app_usbd_class_methods_t app_usbd_raw_class_methods = {
	.event_handler = usbRawEventHandler,
	.feed_descriptors = usbRawGetDescriptors,
};

#define USBD_RAW_INTERFACE_CONFIG()                 \
		APP_USBD_RAW_CONFIG(USB_RAW_INTERFACE_ID, \
				USB_RAW_EPIN,      \
				USB_RAW_EPOUT)


#define APP_USBD_RAW_INST_CONFIG(descriptors)   \
	.inst = { .p_raw_desc = (uint8_t const *)&descriptors,  \
		.raw_desc_size = sizeof(descriptors), \
	}

#define APP_USBD_DEFINE_RAW_CLASS(instance_name)\
		APP_USBD_CLASS_INST_GLOBAL_DEF(instance_name, app_usbd_raw, &app_usbd_raw_class_methods, USBD_RAW_INTERFACE_CONFIG(), (APP_USBD_RAW_INST_CONFIG(rawInterfaceDescriptor)))


APP_USBD_DEFINE_RAW_CLASS(usb_raw_test);


static inline app_usbd_class_inst_t const* usbdRawToClassInst(app_usbd_raw_t const* p_raw){
    return &p_raw->base;
}


static ret_code_t setup_req_vendor_in(app_usbd_class_inst_t const* const p_inst, app_usbd_setup_evt_t const* const p_setup_ev){
	(void)p_inst;
	if(p_setup_ev->setup.bRequest == (uint8_t)USB_VENDOR_CODE_WINUSB){
		if(p_setup_ev->setup.wIndex.w == 0x04){
			return app_usbd_core_setup_rsp(&p_setup_ev->setup, &NDC_ExtCompatIdOsDesc, sizeof(NDC_ExtCompatIdOsDesc));
		}
	}
	return NRF_ERROR_NOT_SUPPORTED;
}


static const app_usbd_config_t m_usbd_config;

void initUSB(){
	//Winusb processing
	app_usbd_core_set_device_control_handler(setup_req_vendor_in);
	app_usbd_set_unknown_interface_request_handler(unknownInterfaceRequestHandler);

	ret_code_t ret = app_usbd_init(&m_usbd_config);
	APP_ERROR_CHECK(ret);

	app_usbd_class_inst_t const* class_raw = usbdRawToClassInst(&usb_raw_test);
	ret = app_usbd_class_append(class_raw);
	APP_ERROR_CHECK(ret);
}

void startUSB(const char* serialNumber, const char* productName){
	for (size_t i = 0; i <= strlen(serialNumber); ++i) {
		UsbRawSerialString[i] = serialNumber[i];
	}

	for (size_t i = 0; i <= strlen(productName); ++i) {
		UsbRawProductString[i] = productName[i];
	}
	app_usbd_enable();
	while (!nrf_power_usbregstatus_vbusdet_get()) {
		vTaskDelay(100);
	}
	app_usbd_start();
}

void stopUSB(){
    app_usbd_stop();
	app_usbd_disable();
}

void setUserEventHandler(UserEventHandlerTypedef handler, void* context) {
	userEventHandler = handler;
	userContext = context;
}

#endif
