// SiLabs CP2130 via LibUSB
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

#include "LibUSB.h"
#include "CP2130.h"
#include "SpiFlash.h"

int main(void)
{
	LibUSB lusb;
	LibUSB_dev d(lusb, 0x10C4, 0x87A0);
	CP2130 spi(&d);

	spi.set_gpio_cs(0, 2);
	spi.set_spi_word(0); // use CS0
	std::vector<uint8_t> c(4);
	c[0] = 0x9F;
	spi.writeread(c);
	for (auto it = c.begin() ; it != c.end(); ++it)
		std::cout << ' ' << (int) *it << std::endl;

	SpiFlash f(&spi);
	f.write_file(0, "example.cpp");
}
