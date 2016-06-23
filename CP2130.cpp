// SiLabs CP2130 via LibUSB
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#include <stdexcept>
#include <vector>
#include <string>

#include "CP2130.h"

void CP2130::writeread(uint8_t * data, const uint32_t len)
{
	//std::cout << "writeread " << len << std::endl;
	// send the command and some of the data
	uint32_t d32[16] = {0x00020000, len, };
	uint8_t *d = (uint8_t *) &d32;
	for(int i=0; i<std::min(len, 56u); i++)
		d[8+i] = data[i];
	dev->bulk_transfer(0x01, d, std::min(8u+len, 64u));
	// it replies with the first up to 56 bytes first
	// this isn't clear in AN792
	dev->bulk_transfer(0x82, &data[0], std::min(len, 56u));
	// send and receive the data
	for(uint32_t i=56; i<len; i+=64) {
		uint32_t wlen = std::min(len-i, 64u);
		dev->bulk_transfer(0x01, &data[i], wlen);
		dev->bulk_transfer(0x82, &data[i], wlen);
	}
	// the docs mention a zero length packet on completion,
	// but it doesn't seem to exist
}

void CP2130::write(uint8_t * data, const uint32_t len)
{
	uint32_t d32[16] = {0x00010000, len, };
	uint8_t *d = (uint8_t *) &d32;
	// send the command and first 56 or fewer bytes
	for(int i=0; i<std::min(len, 56u); i++)
		d[8+i] = data[i];
	dev->bulk_transfer(0x01, d, std::min(len+8u, 64u));
	// send the remainder
	uint32_t count = 56;
	while(len > count) {
		dev->bulk_transfer(0x01, &data[count], std::min(len-count, 64u));
		count += 64;
	}
}

void CP2130::read(uint8_t *data, const uint32_t len)
{
	// send the command
	uint32_t d[2] = {0x00000000, len};
	dev->bulk_transfer(0x01, (uint8_t *) d, 8);
	// fetch the data
	for(uint32_t i=0; i<=len; i+=64) {
		dev->bulk_transfer(0x82, &data[i], std::min(len-i, 64u));
	}
}

void CP2130::set_dual_string(std::string & s, const uint8_t req)
{
	if(s.length() > 62)
		throw std::runtime_error("string too long");
	uint8_t d[128] = {(uint8_t) (2*s.length()+2), 3, };
	for(int i=2; i<128; i++)
		d[i] = 0;
	for(int i=0; i<s.length(); i++)
	{
		if(i < 31)
			d[2*i+2] = s[i];
		else
			d[2*i+3] = s[i];
	}
	dev->control_transfer(0x40, req+0, d, 64, 0xA5F1);
	dev->control_transfer(0x40, req+2, d, 64, 0xA5F1);
}
