#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include "libusb.h"

#if 0
#define LOG_INFO(message, ret) do { \
    std::cout<<"function: "<<__FUNCTION__<<" line: "<<__LINE__<<" message: "<<(message)<<std::endl; \
    std::cout<<"ret = "<<(ret)<<std::endl; \
    std::cout<<libusb_error_name((ret))<<std::endl; \
} while (0)
#else
#define LOG_INFO(message, ret)
#endif
class UsbDevice
{
public:
    typedef libusb_hotplug_callback_fn HotplugF;
public:
    UsbDevice();
    ~UsbDevice();
    void detect();
    void release();
    int lookUp(int productID, int vendorID);
    static int hotplug_attach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void* userdata);
    static int hotplug_detach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void* userdata);
    static void handleEvent(UsbDevice *pUsbDevice);
    void stopListening();
    int registerHotPlug(int productID, int vendorID, void *userData, HotplugF attach, HotplugF detach);
    int send(unsigned char* pucData, unsigned short usLen);
    int waitResponse(unsigned char* pucData, unsigned short usLen, unsigned short usTimeout);
public:
    libusb_device_handle* devHandle;
    libusb_context* usbContext;
    libusb_hotplug_callback_handle hotplugCallback[2];
    unsigned char in;
    unsigned char out;
    std::atomic<bool> isHandling;
    std::thread *pThread;
};

#endif // USBDEVICE_H
