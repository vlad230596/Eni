#pragma once

#include "USBTypes.h"

namespace Eni::USB {

class UsbEndpoint {
public:
    UsbEndpoint(const EndpointDescriptor descriptor):_descriptor(descriptor){

	}

    const EndpointDescriptor& getDescriptor() const {
        return _descriptor;
    }

private:
    EndpointDescriptor _descriptor;
};

}
