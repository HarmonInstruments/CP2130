// SiLabs CP2130 via LibUSB
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

#include "CP2130.h"

#include <fstream>

std::vector<uint8_t> vecf(std::string name) {
	std::ifstream is(name);
	is.seekg(0, is.end);
	size_t size = is.tellg();
	is.seekg(0, is.beg);
	std::vector<uint8_t> d(size);
	is.read((char *) &d[0], size);
	is.close();
	return d;
}

int main(void)
{
	LibUSB lusb;
	LibUSB_dev d(lusb, 0x10C4, 0x8B00);
	CP2130 spi(&d);

	spi.set_gpio_cs(4, 2);
	spi.set_spi_word(4);
	spi.set_clockdiv(1);

	//spi.set_manufacturer_string("Harmon Instruments, LLC");
	//spi.set_prod_string("ice40");
	//spi.set_lock_byte();
	//spi.set_serial("cal1");
	//spi.get_lock_byte();

	std::vector<uint8_t> v = vecf("/home/dlharmon/vna/fpga/cal/cal.bin");
	std::cout << "file size = " << v.size() << std::endl;
	spi.set_gpio(10,2,0);
	spi.set_gpio(0,2,0);
	usleep(1000);
	spi.set_gpio(10,2,1);
	spi.set_gpio(9,2,0);
	usleep(1000);
	uint8_t pcd[] = {2, 2, 2, 2,
		       2, 4, 2, 2,
		       2, 2, 2, 0x7D,
		       0xF8, 0, 0, 0,
		       0, 0, 0, 1};
	//spi.set_pin_config(pcd);
	v.resize(v.size()+10);
	spi.write(v);
	usleep(1000);
	spi.set_gpio(9,2,1);
	usleep(100000);
	v = vecf("/home/dlharmon/vna/fpga/cm36/cal.bin");
	v.resize(v.size()+1000);
	spi.write(v);
}
