/*
 * vs1003.h
 *
 *  Created on: 25-04-2011
 *      Author: Przemyslaw Stasiak
 */
//#include "stm32f10x.h"
#include <stdio.h>
#include "stm32f2xx_hal.h"

#ifndef VS1003_H_
#define VS1003_H_

#define DREQ_PIN	GPIO_PIN_0
#define DREQ_PORT	GPIOC
#define CS_PORT		GPIOC
#define CS_PIN		GPIO_PIN_1
#define XDCS_PORT   GPIOC
#define XDCS_PIN	GPIO_PIN_3
#define XRESET_PORT	GPIOC
#define XRESET_PIN	GPIO_PIN_2

#define RXNE    0x01
#define TXE     0x02
#define BSY     0x80

#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03
#define SPI_MODE        	0x00
#define SPI_STATUS      	0x01
#define SPI_BASS        	0x02
#define SPI_CLOCKF      	0x03
#define SPI_DECODE_TIME 	0x04
#define SPI_AUDATA      	0x05
#define SPI_WRAM        	0x06
#define SPI_WRAMADDR    	0x07
#define SPI_HDAT0       	0x08
#define SPI_HDAT1       	0x09
#define SPI_AIADDR      	0x0a
#define SPI_VOL         	0x0b
#define SPI_AICTRL0     	0x0c
#define SPI_AICTRL1     	0x0d
#define SPI_AICTRL2     	0x0e
#define SPI_AICTRL3     	0x0f
#define SM_DIFF         	0x01
#define SM_JUMP         	0x02
#define SM_RESET        	0x04
#define SM_OUTOFWAV     	0x08
#define SM_PDOWN        	0x10
#define SM_TESTS        	0x20
#define SM_STREAM       	0x40
#define SM_PLUSV        	0x80
#define SM_DACT         	0x100
#define SM_SDIORD       	0x200
#define SM_SDISHARE     	0x400
#define SM_SDINEW       	0x800
#define SM_ADPCM        	0x1000
#define SM_ADPCM_HP     	0x2000

//public functions
void 	VS1003_GPIO_conf();
void 	VS1003_SPI_conf();
void 	VS1003_SineTest();
void	VS1003_Start();
//void 	VS1003_SendMusicBytes(uint8_t* music,int quantity);
int 	VS1003_SendMusicBytes(uint8_t* music,int quantity);
void 	VS1003_SoftwareReset();
uint16_t	VS1003_GetBitrate();
uint16_t	VS1003_GetSampleRate();
uint16_t	VS1003_GetDecodeTime();

//Volume control
uint8_t 	VS1003_GetVolume();
void	VS1003_SetVolume(uint8_t xMinusHalfdB);
void 	VS1003_VolumeUp(uint8_t xHalfdB);
void	VS1003_VolumeDown(uint8_t xHalfdB);
//Treble control
uint8_t	VS1003_GetTreble();
void	VS1003_SetTreble(uint8_t xOneAndHalfdB);
void	VS1003_TrebleUp(uint8_t xOneAndHalfdB);
void	VS1003_TrebleDown(uint8_t xOneAndHalfdB);
void	VS1003_SetTrebleFreq(uint8_t xkHz);
//Bass control
uint8_t	VS1003_GetBass();
void	VS1003_SetBass(uint8_t xdB);
void	VS1003_BassUp(uint8_t xdB);
void	VS1003_BassDown(uint8_t xdB);
void	VS1003_SetBassFreq(uint8_t xTenHz);

//private functions
uint8_t SPIPutChar(uint8_t outB);
void Delay(uint32_t nTime);
void ControlReset(uint8_t State);
void SCI_ChipSelect(uint8_t State);
void SDI_ChipSelect(uint8_t State);
void WriteRegister(uint8_t addressbyte,
		uint8_t highbyte, uint8_t lowbyte);
uint16_t ReadRegister(uint8_t addressbyte);
void ResetChip();

uint16_t MaskAndShiftRight(uint16_t Source, uint16_t Mask, uint16_t Shift);

void VS1003_regtest();
void VS1003_sine(char pitch);
void VS1003_SPI_SpeedUp();
void VS1003_PluginLoad();

#endif /* VS1003_H_ */
