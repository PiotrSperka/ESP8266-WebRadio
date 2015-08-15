#include "eeprom.h"
#include "flash.h"
#include "stdio.h"

#define ICACHE_STORE_TYPEDEF_ATTR __attribute__((aligned(4),packed))
#define ICACHE_STORE_ATTR __attribute__((aligned(4)))
#define ICACHE_RAM_ATTR __attribute__((section(".iram0.text")))

#define EEPROM_START 0x3F0000 // Last 64k of flash (32Mbits or 4 MBytes)

uint32_t eebuf[1024];

uint8_t eeGetByte(uint32_t address) { // address = number of 1-byte parts from beginning
	uint8_t t = 0;
	spi_flash_read(EEPROM_START + address, (uint32 *)&t, 1);
	return t;
}

void eeSetByte(uint32_t address, uint8_t data) {
	uint32_t addr = (EEPROM_START + address) & 0xFFF000;
	spi_flash_read(addr, (uint32 *)eebuf, 4096);
	spi_flash_erase_sector(addr >> 12);
	eebuf[address & 0xFFF] = data;
	spi_flash_write(addr, (uint32 *)eebuf, 4096);
}

uint32_t eeGet4Byte(uint32_t address) { // address = number of 4-byte parts from beginning
	address *= 4;
	uint32_t t = 0;
	spi_flash_read(EEPROM_START + address, (uint32 *)&t, 4);
	return t;
}

void eeSet4Byte(uint32_t address, uint32_t data) {
	address *= 4;
	uint32_t addr = (EEPROM_START + address) & 0xFFF000;
	spi_flash_read(addr, (uint32 *)eebuf, 4096);
	spi_flash_erase_sector(addr >> 12);
	eebuf[(address/4) & 0xFFF] = data;
	spi_flash_write(addr, (uint32 *)eebuf, 4096);
}

void eeGetData(int address, void* buffer, int size) { // address, size in BYTES !!!!
	spi_flash_read(EEPROM_START + address, (uint32 *)buffer, size);
}

void eeSetData(int address, void* buffer, int size) { // address, size in BYTES !!!!
	uint8_t* inbuf = buffer;
	while(1) {
		uint32_t sector = (EEPROM_START + address) & 0xFFF000;
		spi_flash_read(sector, (uint32 *)eebuf, 4096);
		spi_flash_erase_sector(sector >> 12);
		
		uint8_t* eebuf8 = (uint8_t*)eebuf;
		uint16_t startaddr = address & 0xFFF;
		uint16_t maxsize = 4096 - startaddr;
		uint16_t i;
		
		for(i=0; (i<size && i<maxsize); i++) eebuf8[i+startaddr] = inbuf[i];
		spi_flash_write(sector, (uint32 *)eebuf, 4096);
		
		if(maxsize >= size) break;
		
		address += i;
		inbuf += i;
		size -= i;
	}
}
