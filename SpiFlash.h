/*
 * HIFIFO: Harmon Instruments PCI Express to FIFO
 * Copyright (C) 2014 Harmon Instruments, LLC
 * MIT License
 */

#include "CP2130.h"

class SpiFlash
{
private:
	class CP2130 * cp2130;
public:
	const int sector_size = 65536;
	const int page_size = 256;
	SpiFlash(CP2130 *s);
	void spi(uint8_t *d, int count);
	void txrx(int spi_address, char * data, int len, int read_offset);
	void put_byte(uint8_t data, bool last);
	uint8_t get_byte(bool last);
	void write_enable();
	uint32_t read_id();
	void read(int addr, int count, uint8_t *buf);
	int read_status();
	void write_status(uint8_t val);
	void wait_busy();
	void page_program(int address, uint8_t *data);
	void sector_erase(int address);
	void sector_write(int address, uint8_t *data);
	void write_file(int address, const char *filename);
};
