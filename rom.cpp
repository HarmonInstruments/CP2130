// SiLabs CP2130 via LibUSB
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "CP2130.h"

int main(void)
{
	LibUSB lusb;
	LibUSB_dev d(lusb, 0x10C4, 0x87A0);
	CP2130 spi(&d);

	spi.set_manufacturer_string("Harmon Instruments, LLC");
	spi.set_prod_string("ice40");
	spi.set_serial("cal2");
	//spi.set_lock_byte();

	//spi.get_lock_byte();

	uint8_t pcd[] = {2, 2, 2, 2,
		       2, 4, 2, 2,
		       2, 2, 2, 0x7D,
		       0xF8, 0, 0, 0,
		       0, 0, 0, 1};
	spi.set_pin_config(pcd);

	spi.set_usb_config(
		0x10C4, // SiLabs VID
		0x8B00, // Harmon Instruments PID -- DO NOT USE elsewhere
		50, // max power 2 mA units
		0, // power_mode usually 0, see AN792
		0, //uint8_t release_major,
		1, //uint8_t release_minor,
		0x1F//uint8_t mask
                );
}
