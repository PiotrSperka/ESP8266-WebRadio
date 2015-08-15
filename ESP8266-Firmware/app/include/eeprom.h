#pragma once

#include "c_types.h"

uint8_t eeGetByte(uint32_t address);
void eeSetByte(uint32_t address, uint8_t data);
uint32_t eeGet4Byte(uint32_t address);
void eeSet4Byte(uint32_t address, uint32_t data);
void eeGetData(int address, void* buffer, int size);
void eeSetData(int address, void* buffer, int size);
