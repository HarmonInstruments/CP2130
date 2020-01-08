# CP2130 - Unmaintained
SiLabs CP2130 USB to SPI Llbusb C++ class

The CP2130 is no longer used by Harmon Instruments and this repo will be archived and no longer be maintained. 

CP2130 interface spec
http://www.silabs.com/Support%20Documents/TechnicalDocs/AN792.pdf

The writeread (full duplex) transaction doesn't work with longer lengths. 67 bytes works. 260 bytes does not. The documentation is unclear on how this mode works.

It doesn't appear to be possible to do multiple transactions without deasserting CS so the read and write commands are less than useful.

Read the interface spec and be careful with the commands that write to the one time programmable memory.

# Example usage

```
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

	// set GPIO 2 to push pull and low
	spi.set_gpio(2,2,0);

	// setup to use CS 0
        spi.set_gpio_cs(0, 2);
        spi.set_spi_word(0);

	// get the ID of a SPI flash on CS0
        std::vector<uint8_t> c(4);
        c[0] = 0x9F;
        spi.writeread(c);
        for (auto it = c.begin() ; it != c.end(); ++it)
                std::cout << ' ' << (int) *it << std::endl;

	// Use the SPI flash class to access the same
        SpiFlash f(&spi);
        f.write_file(0, "spi.cpp");
}

```
