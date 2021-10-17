#include "UsbDevice.h"

UsbDevice::UsbDevice():
    devHandle(nullptr),
    usbContext(nullptr),
    pThread(nullptr)
{

}

UsbDevice::~UsbDevice()
{
    stopListening();
    if (pThread != nullptr) {
        delete pThread;
        pThread = nullptr;
    }
    release();
}

static void detectDevice(libusb_device *dev, std::fstream& detectFile)
{
    struct libusb_device_descriptor desc;
    libusb_device_handle *handle = NULL;
    unsigned char strData[256];
    int ret;
    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
        detectFile<<"failed to get device descriptor"<<std::endl;
        detectFile<<"----------------------------------------------------------"<<std::endl;
        return;
    }
    detectFile<<"bus number: "<<libusb_get_bus_number(dev)<<std::endl;
    detectFile<<"device address: "<<libusb_get_device_address(dev)<<std::endl;
    detectFile<<"vendor ID: "<<desc.idVendor<<std::endl;
    detectFile<<"product ID: "<<desc.idProduct<<std::endl;
    ret = libusb_open(dev, &handle);
    if (ret != LIBUSB_SUCCESS) {
        detectFile<<"failed to open device"<<std::endl;
        detectFile<<"----------------------------------------------------------"<<std::endl;
        return;
    }
    if (desc.iManufacturer != 0) {
        ret = libusb_get_string_descriptor_ascii(handle, desc.iManufacturer, strData, sizeof(strData));
        if (ret > 0) {
            detectFile<<"Manufacturer: "<< (char *)strData<<std::endl;
        }
    }

    if (desc.iProduct != 0) {
        ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, strData, sizeof(strData));
        if (ret > 0) {
            detectFile<<"Product: "<< (char *)strData<<std::endl;
        }
    }

    if (desc.iSerialNumber != 0) {
        ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, strData, sizeof(strData));
        if (ret > 0) {
            detectFile<<"Serial Number: "<<strData<<std::endl;
        }
    }
    if (handle != nullptr) {
        libusb_close(handle);
    }
    detectFile<<"----------------------------------------------------------"<<std::endl;
    return;
}

void UsbDevice::detect()
{
    release();
    libusb_device **devs;
    ssize_t cnt;
    std::fstream detectFile;
    detectFile.open("detectFile.usb", std::ios_base::out|std::ios_base::trunc);
    if (!detectFile.is_open()) {
        return;
    }
    int ret = libusb_init(&usbContext);
    if (ret != LIBUSB_SUCCESS) {
        detectFile<<"fail to init usb"<<std::endl;
        return;
    }
    cnt = libusb_get_device_list(nullptr, &devs);
    if (cnt < 0) {
        detectFile<<"fail to get device's list"<<std::endl;
        return;
    }
    for (int i = 0; devs[i]; i++) {
        detectDevice(devs[i], detectFile);
    }
    detectFile.close();
    libusb_free_device_list(devs, 1);
    return;
}

void UsbDevice::release()
{
    if (devHandle != nullptr) {
        int ret = libusb_release_interface(devHandle, 0);
        if (ret != LIBUSB_SUCCESS) {
            LOG_INFO("fail to release interface", ret);
        }
        libusb_close(devHandle);
        devHandle = nullptr;
    }
    if (usbContext != nullptr) {
        libusb_exit(usbContext);
        usbContext = nullptr;
    }
    return;
}

int UsbDevice::lookUp(int productID, int vendorID)
{
    int ret = 0;
    ssize_t deviceNum = 0;
    in = 0;
    out = 0;
    /* init usb */
    ret = libusb_init(&usbContext);
    if (ret != LIBUSB_SUCCESS) {
        LOG_INFO("fail to init", ret);
        return ret;
    }
    /* get device's list */
    libusb_device** devList = nullptr;
    deviceNum = libusb_get_device_list(usbContext, &devList);
    if (deviceNum <= 0) {
        LOG_INFO("fail to get device list", cnt);
        return LIBUSB_ERROR_NO_DEVICE;
    }
    /* get device's handle */
    for (int index = 0; index < deviceNum; index++) {
        if (devList[index] == nullptr) {
            continue;
        }
        /* get device's descriptor */
        libusb_device_descriptor desc;
        memset((void*)&desc, 0, sizeof(libusb_device_descriptor));
        ret = libusb_get_device_descriptor(devList[index], &desc);
        if (ret < 0) {
            LOG_INFO("fail to get device descriptor", ret);
            break;
        }
        /* match product_id and vendor_id */
        if ((desc.idVendor == vendorID) && (desc.idProduct == productID)) {
            libusb_config_descriptor *pstConfigDesc = nullptr;
            ret = libusb_get_config_descriptor(devList[index], 0, &pstConfigDesc);
            if (ret != LIBUSB_SUCCESS) {
                LOG_INFO("fail to get config descriptor", ret);
                break;
            }
            /* find endpoint */
             for(int i = 0; i < pstConfigDesc->bNumInterfaces; i++){
                 for (int j = 0; j < pstConfigDesc->interface[i].num_altsetting; j++) {
                     for (int k = 0; k < pstConfigDesc->interface[i].altsetting[j].bNumEndpoints; k++) {
                         const struct libusb_endpoint_descriptor *endpoint = &(pstConfigDesc->interface[i].altsetting[j].endpoint[k]);
                         if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                             in = endpoint->bEndpointAddress;
                         } else {
                             out = endpoint->bEndpointAddress;
                         }
                     }
                 }
             }
             libusb_free_config_descriptor(pstConfigDesc);
             /* get handle */
             ret = libusb_open(devList[index], &devHandle);
             if (ret != LIBUSB_SUCCESS) {
                 continue;
             }
             /* set config */
             ret = libusb_set_configuration(devHandle, 1);
             if (ret != LIBUSB_SUCCESS) {
                 LOG_INFO("fail to set configuration", ret);
             }
             /* claim interface */
             ret = libusb_claim_interface(devHandle, 0);
             if (ret != LIBUSB_SUCCESS) {
                 LOG_INFO("fail to claim interface", ret);
             }
             break;
        } else {
            ret = LIBUSB_ERROR_NOT_FOUND;
        }
    }
    libusb_free_device_list(devList, 1);
    return ret;
}
int UsbDevice::hotplug_attach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void* userdata)
{
    if (ctx == nullptr || dev == nullptr || userdata == nullptr) {
        return LIBUSB_ERROR_NOT_FOUND;
    }
    UsbDevice *pUsbDevice = (UsbDevice*)userdata;
    /* get device's descriptor */
    int ret = 0;
    libusb_device_descriptor desc;
    memset((void*)&desc, 0, sizeof(libusb_device_descriptor));
    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
        LOG_INFO("fail to get device descriptor", ret);
        return ret;
    }
    /* match product_id and vendor_id */
    libusb_config_descriptor *pstConfigDesc = nullptr;
    ret = libusb_get_config_descriptor(dev, 0, &pstConfigDesc);
    if (ret != LIBUSB_SUCCESS) {
        LOG_INFO("fail to get config descriptor", ret);
        return ret;
    }
    /* find endpoint */
    for (int i = 0; i < pstConfigDesc->bNumInterfaces; i++){
        for (int j = 0; j < pstConfigDesc->interface[i].num_altsetting; j++) {
             for (int k = 0; k < pstConfigDesc->interface[i].altsetting[j].bNumEndpoints; k++) {
                 const struct libusb_endpoint_descriptor *endpoint = &(pstConfigDesc->interface[i].altsetting[j].endpoint[k]);
                 if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                     pUsbDevice->in = endpoint->bEndpointAddress;
                 } else {
                     pUsbDevice->out = endpoint->bEndpointAddress;
                 }
            }
        }
    }
    libusb_free_config_descriptor(pstConfigDesc);
    /* get handle */
    ret = libusb_open(dev, &(pUsbDevice->devHandle));
    if (ret != LIBUSB_SUCCESS) {
        return ret;
    }
    /* set config */
    ret = libusb_set_configuration(pUsbDevice->devHandle, 1);
    if (ret != LIBUSB_SUCCESS) {
        LOG_INFO("fail to set configuration", ret);
        return ret;
    }
    /* claim interface */
    ret = libusb_claim_interface(pUsbDevice->devHandle, 0);
    if (ret != LIBUSB_SUCCESS) {
        LOG_INFO("fail to claim interface", ret);
        return ret;
    }
    return LIBUSB_SUCCESS;
}

int UsbDevice::hotplug_detach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void* userdata)
{
    if (ctx == nullptr || dev == nullptr || userdata == nullptr) {
        return LIBUSB_ERROR_NOT_FOUND;
    }
    UsbDevice *pUsbDevice = (UsbDevice*)userdata;
    if (pUsbDevice->devHandle != nullptr) {
        libusb_close(pUsbDevice->devHandle);
        pUsbDevice->devHandle = nullptr;
    }
    return LIBUSB_SUCCESS;
}

void UsbDevice::handleEvent(UsbDevice *pUsbDevice)
{
    std::cout<<"handle event"<<std::endl;
    while (pUsbDevice->isHandling.load()) {
        libusb_handle_events_completed(pUsbDevice->usbContext, nullptr);
    }
    return;
}

void UsbDevice::stopListening()
{
    isHandling.store(0);
    if (pThread != nullptr) {
        pThread->join();
    }
    return;
}

int UsbDevice::registerHotPlug(int productID, int vendorID, void *userData, HotplugF attach, HotplugF detach)
{
    if (userData == nullptr || attach == nullptr || detach == nullptr) {
        return LIBUSB_ERROR_INVALID_PARAM;
    }
    /* support */
    if (!libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        std::cout<<"not support hot plug"<<std::endl;
        return LIBUSB_ERROR_NOT_SUPPORTED;
    }
    /* init */
    int ret = libusb_init(&usbContext);
    if (ret != LIBUSB_SUCCESS) {
        return ret;
    }
    /* register callback */
    /* attach */
    ret = libusb_hotplug_register_callback(usbContext,
                                           LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,
                                           LIBUSB_HOTPLUG_NO_FLAGS,
                                           vendorID,
                                           productID,
                                           LIBUSB_HOTPLUG_MATCH_ANY,
                                           attach,
                                           userData,
                                           &hotplugCallback[0]);
    if (ret != LIBUSB_SUCCESS) {
        return ret;
    }
    /* detach */
    ret = libusb_hotplug_register_callback(usbContext,
                                           LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
                                           LIBUSB_HOTPLUG_NO_FLAGS,
                                           vendorID,
                                           productID,
                                           LIBUSB_HOTPLUG_MATCH_ANY,
                                           detach,
                                           userData,
                                           &hotplugCallback[1]);
    if (ret != LIBUSB_SUCCESS) {
        return ret;
    }
    /* listen */
    isHandling.store(1);
    pThread = new std::thread(&handleEvent, this);
    std::cout<<"has init hot plug"<<std::endl;
    return LIBUSB_SUCCESS;
}

int UsbDevice::send(unsigned char* pucData, unsigned short usLen)
{
    int ret;
    int i = 0;
    int actual_size = 0;
    do {
        ret = libusb_bulk_transfer(devHandle, out, pucData, usLen, &actual_size, 5000);
        if (ret == LIBUSB_ERROR_PIPE) {
            LOG_INFO("LIBUSB_ERROR_PIPE", ret);
            libusb_clear_halt(devHandle, out);
        }
        i++;
    } while ((ret == LIBUSB_ERROR_PIPE) && (i < 10));
    if (ret != LIBUSB_SUCCESS) {
        LOG_INFO("transfer out error", ret);
    }
    return ret;
}

int UsbDevice::waitResponse(unsigned char* pucData, unsigned short usLen, unsigned short usTimeout)
{
    int ret;
    int i = 0;
    int actual_size = 0;
    do {
        ret = libusb_bulk_transfer(devHandle, in, pucData, usLen, &actual_size, usTimeout);
        if (ret == LIBUSB_ERROR_PIPE) {
            LOG_INFO("LIBUSB_ERROR_PIPE", ret);
            libusb_clear_halt(devHandle, in);
        }
        i++;
    } while ((ret == LIBUSB_ERROR_PIPE) && (i < 100));
    if (ret != LIBUSB_SUCCESS) {
        LOG_INFO("transfer in error", ret);
    }
    return ret;
}
