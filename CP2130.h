// SiLabs CP2130 via LibUSB
// Copyright 2016 Harmon Instruments, LLC
// MIT License -- see LICENSE.md

#pragma once
#include <stdexcept>
#include <vector>
#include <string>

#include "LibUSB.h"

class CP2130
{
private:
	class LibUSB_dev * dev;
	void set_dual_string(std::string & s, const uint8_t req);
public:
	CP2130(class LibUSB_dev * d) {dev = d;}
	void write(uint8_t * data, const uint32_t len);
	void writeread(uint8_t * data, const uint32_t len);
	void read(uint8_t *data, const uint32_t len);

	template <typename T> void write(std::vector<T> &d) {
		write((uint8_t *) &d[0], sizeof(T) * d.size());
	}
	template <typename T> void writeread(std::vector<T> &d) {
		writeread((uint8_t *) &d[0], sizeof(T) * d.size());
	}
	template <typename T> void read(std::vector<T> &d) {
		read((uint8_t *) &d[0], sizeof(T) * d.size());
	}

	void set_serial(const std::string s){
		if(s.length() > 30)
			throw std::runtime_error("serial string too long");
		uint8_t d[64] = {(uint8_t) (2*s.length()+2), 3, };
		for(int i=2; i<64; i++)
			d[i] = 0;
		for(int i=0; i<s.length(); i++)
			d[2*i+2] = s[i];
		dev->control_transfer(0x40, 0x6B, d, 64, 0xA5F1);
	}

	void set_manufacturer_string(std::string s){
		set_dual_string(s, 0x63);
	}

	void set_prod_string(std::string s){
		set_dual_string(s, 0x67);
	}

	// set the clock divider for GPIO5 to 24 MHz / clockdiv
	// for clockdiv = 0, the divisor is 256
	void set_clockdiv(uint8_t clockdiv) {
		dev->control_transfer(0x40, 0x47, &clockdiv, 1);
	}

	// channel is the GPIO pin (0 - 10)
	// mode is 0: disabled, 1: enabled, 2: enabled with all others disabled
	void set_gpio_cs(uint8_t channel, uint8_t mode) {
		uint8_t d[2] = {channel, mode};
		dev->control_transfer(0x40, 0x25, d, 2);
	}

	// channel is the GPIO pin for CS (0 - 10)
	// bitmap: see SiLabs AN792 Set_SPI_Word (0x08 for mode 0, 12 MHz)
	void set_spi_word(uint8_t channel, uint8_t bitmap = 0x08){
		uint8_t d[2] = {channel, bitmap};
		dev->control_transfer(0x40, 0x31, d, 2);
	}

	void set_gpio(uint8_t pin, uint8_t mode, uint8_t level) {
		uint8_t data[] = {pin, mode, level};
		dev->control_transfer(0x40, 0x23, data, 3);
	}

	void get_lock_byte() {
		uint8_t d[2];
		dev->control_transfer(0xC0, 0x6E, d, 2);
		std::cout << (int) d[0] << " " << (int) d[2] << std::endl;
	}

	void set_lock_byte(uint8_t b0, uint8_t b1) {
		uint8_t d[] = {b0, b1};
		dev->control_transfer(0x40, 0x6F, d, 2, 0xA5F1);
	}

	void set_usb_config(uint16_t vid, // USB vendor ID
			    uint16_t pid, // USB product ID
			    uint8_t max_power, // 2 mA units
			    uint8_t power_mode, // usually 0, see AN792
			    uint8_t release_major,
			    uint8_t release_minor,
			    uint8_t mask
		) {
		uint8_t d[] = {(uint8_t) (vid & 0xFF), (uint8_t) (vid >> 8),
			       (uint8_t) (pid & 0xFF), (uint8_t) (pid >> 8),
			       max_power, power_mode, release_major, release_minor,
			       0, mask};
		dev->control_transfer(0x40, 0x61, d, 10, 0xA5F1);
	}

	void set_pin_config(uint8_t *d) {
		dev->control_transfer(0x40, 0x6D, d, 20, 0xA5F1);
	}
};
