// LibUSB C++ wrapper
// only a small subset as required for projects
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#pragma once
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdexcept>
#include <vector>
#include <string>

class LibUSB {
private:
	libusb_context * ctx = NULL;
public:
	LibUSB() {
		if (libusb_init(&ctx) < 0) {
			throw std::runtime_error("failed in libusb init");
		}
		libusb_set_debug(ctx, 3);
	}
	~LibUSB() {
		if (ctx)
			libusb_exit(ctx);
	}
	libusb_device_handle *open_device_with_vid_pid(uint16_t vid,
						       uint16_t pid) {
		return libusb_open_device_with_vid_pid(ctx, vid, pid);
	}

};

class LibUSB_dev {
private:
	int kernel_attached;
	int timeout = 1000;
	libusb_device_handle * dev = NULL;
public:
	// find a device with VID and PID
	LibUSB_dev(class LibUSB & lusb, uint16_t vid, uint16_t pid) {
		dev = lusb.open_device_with_vid_pid(vid, pid);
		if (dev == NULL) {
			throw std::runtime_error("failed to open device");
		}
		if (libusb_kernel_driver_active(dev, 0) != 0) {
			libusb_detach_kernel_driver(dev, 0);
			kernel_attached = 1;
		}
		if (libusb_claim_interface(dev, 0) < 0) {
			libusb_close(dev);
			throw std::runtime_error("usb_claim_interface failed");
		}
	}
	~LibUSB_dev() {
		libusb_release_interface(dev, 0);
		if (kernel_attached)
			libusb_attach_kernel_driver(dev, 0);
		libusb_close(dev);
	}
	void control_transfer(uint8_t reqtype,
			      uint8_t request,
			      uint8_t * data,
			      uint16_t len,
			      uint16_t wvalue = 0,
			      uint16_t windex = 0)
	{
		int rv = libusb_control_transfer(
			dev,
			reqtype,
			request,
			wvalue,
			windex,
			data,
			len,
			timeout);
		if (rv < 0)
			throw std::runtime_error("libusb_control_transfer failed");
	}
	void bulk_transfer(int ep, uint8_t * data, int len) {
		int transferred;
		int rv = libusb_bulk_transfer(dev, ep, data, len, &transferred, timeout);
		//std::cout << "bulk " << ep << " " << len << " " << transferred << std::endl;
		if (rv < 0)
			throw std::runtime_error("USB bulk fail");
		if (transferred < len)
			throw std::runtime_error("USB bulk short");
	}
	void set_timeout(int t) {
		timeout = t;
	}
};
