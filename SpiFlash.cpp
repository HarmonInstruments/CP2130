/*
 * Harmon Instruments SPI flash interface
 * Copyright (C) 2014-2016 Harmon Instruments, LLC
 * MIT License -- See LICENSE.md
 */

#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "SpiFlash.h"

SpiFlash::SpiFlash(CP2130 *s)
{
	cp2130 = s;
	// power up
	//put_byte(0xAB, 1);
	read_id();
}

void SpiFlash::spi(uint8_t *data, int len) {
	cp2130->writeread(data, len);
}

void SpiFlash::write_enable() {
	uint8_t d[] = {0x06};
	spi(d, 1);
}

uint32_t SpiFlash::read_id()
{
	uint8_t d[] = {0x9F, 0, 0, 0, 0};
	spi(d,5);
	uint32_t rbuf = d[1] << 24;
	rbuf |= d[2] << 16;
	rbuf |= d[3] << 8;
	rbuf |= d[4];
        fprintf(stderr, "ID = 0x%.8X\n", rbuf);
	return rbuf;
}

void SpiFlash::read(int addr, int count, uint8_t *buf)
{
	std::vector<uint8_t> v(count+5);
	v[0] = 0x0B;
	v[1] = (uint8_t) (addr >> 16);
	v[2] = (uint8_t) (addr >> 8);
	v[3] = (uint8_t) (addr >> 0);
	spi(&v[0], count+5);
	for (int i=0; i<count; i++)
		buf[i] = v[i+5];
}

int SpiFlash::read_status()
{
	uint8_t d[] = {0x05, 0};
	spi(d, 2);
	return d[1];
}

void SpiFlash::write_status(uint8_t val)
{
        write_enable();
	uint8_t d[] = {0xBB, val};
	spi(d, 2);
        wait_busy();
}

void SpiFlash::wait_busy()
{
        for(int i=0; i<10000; i++) {
		int status = read_status();
		if((status & 0x01) == 0)
			return;
		usleep(1000);
	}
	throw std::runtime_error("SPI flash timeout");
}

void SpiFlash::page_program(int address, uint8_t *data)
{
	if((address & 0xFF0000FF) != 0)
		throw std::runtime_error("page program address must be aligned to 256 bytes");
        write_enable();
	uint8_t d[260] = {0x02,
			  (uint8_t) (address >> 16),
			  (uint8_t) (address >> 8),
			  0,
	};
	for (int i=0; i<256; i++)
		d[i+4] = data[i];
	cp2130->write(d, 260);
        wait_busy();
}

void SpiFlash::sector_erase(int address)
{
	write_enable();
        if((address & 0xFF00FFFF) != 0)
		throw std::runtime_error("sector erase address must be aligned to 65536 bytes");
	uint8_t d[4] = {0xD8, (uint8_t) (address >> 16), 0, 0};
	spi(d, 4);
	wait_busy();
}

void SpiFlash::sector_write(int address, uint8_t *data)
{
        write_status(0);
	fprintf(stderr, "erasing sector %d of flash\n", address/65536);
	sector_erase(address);
	fprintf(stderr, "writing sector\n");
	for(int i=0; i<sector_size; i+= page_size) {
		page_program(address + i, data + i);
	}
	fprintf(stderr, "reading back sector of flash\n");
	uint8_t rbbuf[65536];
	for(int i=0; i<sector_size; i+=64)
		read(address+i, 64, &rbbuf[i]);
	if(memcmp(rbbuf, data, sector_size) != 0) {
		int failcount = 0;
		for(int i=0; i<sector_size; i++){
			if(data[i] != rbbuf[i])
				fprintf(stderr, "at %.4X, expected 0x%.2X, got 0x%.2X\n",
					i, data[i], rbbuf[i]);
			if(failcount++ > 32)
				break;
		}
		throw std::runtime_error("SPI flash sector write failed");
	}
	fprintf(stderr, "flash write complete\n");
}

void SpiFlash::write_file(int address, const char *filename)
{
	uint8_t secbuf[65536];
	int count = 0;
	if((address & (sector_size - 1)) != 0) {
		throw std::runtime_error("write address must be aligned to sector size");
	}
	FILE* fp = fopen(filename, "rb");
	if(!fp) {
		perror("SpiFlash::write_file opening failed");
		throw std::runtime_error("SpiFlash::write_file opening failed");
	}
	for(;;){
		int rv = fread(secbuf, 1, 65536, fp);
		if(rv < 1)
			return;
		for(int i=rv; i<65536; i++) {
			secbuf[i] = 0;
		}
		sector_write(address + count, secbuf);
		count += rv;
	}
	fclose(fp);
}
