CCFLAGS = -O2 -std=c++11 -flto
SRC = CP2130.cpp SpiFlash.cpp
HEADERS = CP2130.h LibUSB.h SpiFlash.h

ice40: ice40.cpp $(SRC) $(HEADERS)
	g++ -o ice40 ice40.cpp $(SRC) -lusb-1.0 $(CCFLAGS)
	size ice40

example: example.cpp $(SRC) $(HEADERS)
	g++ -o example example.cpp $(SRC) -lusb-1.0 $(CCFLAGS)
	size example

rom: rom.cpp $(SRC) $(HEADERS)
	g++ -o rom rom.cpp $(SRC) -lusb-1.0 $(CCFLAGS)
	size rom

clean:
	rm -rf *.o *.so *~ example ice40 rom
