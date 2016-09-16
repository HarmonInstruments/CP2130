// SiLabs CP2130 via LibUSB
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#include <iostream>
#include <string>

#include "CP2130.h"

int main(int argc, char **argv)
{
	LibUSB lusb;
	LibUSB_dev d(lusb, 0x10C4, 0x8B00);
	CP2130 spi(&d);

	if(argc > 1) {
		int n = std::stoi(argv[1]);
		spi.set_gpio(2,2,1 & (n>>0));
		spi.set_gpio(3,2,1 & (n>>1));
	}
}
