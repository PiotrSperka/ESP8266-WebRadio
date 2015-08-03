/**
  ******************************************************************************
  * @file    vs1003.c
  * @author  Przemyslaw Stasiak
  * @version V0.1
  * @date    25/04/2011
  * @brief   This file provides VS1003 usage and control functions.
  ******************************************************************************
*/

/** @addtogroup VS1003
  * @{
  */

#include "user/vs1053b-patches-flac.h"
#include "user/vs1053.h"
#include "user/spi.h"

SPI_HandleTypeDef SPI_Handle;

void VS1053_HW_init(){
	// todo: Config CS!
 	spi_init(HSPI);
	spi_clock(HSPI, 4, 10); //2MHz
}

void VS1003_SPI_SpeedUp()
{
	spi_clock(HSPI, 2, 5); //8MHz
}

uint8_t SPIPutChar(uint8_t data){
	spi_tx8(HSPI, data);
	while(spi_busy(HSPI));
	return spi_rx8(data);
}

void Delay(uint32_t nTime)
{
	unsigned int i;
	unsigned long j;
	for(i = nTime;i > 0;i--)
		for(j = 1000;j > 0;j--);
}

void ControlReset(uint8_t State){
	//todo: HAL_GPIO_WritePin(XRESET_PORT,XRESET_PIN,!State);
}

void SCI_ChipSelect(uint8_t State){
	//todo: HAL_GPIO_WritePin(CS_PORT,CS_PIN,!State);
}

void SDI_ChipSelect(uint8_t State){
	//todo: HAL_GPIO_WritePin(XDCS_PORT,XDCS_PIN,!State);
}

void VS1053_SineTest(){
	ControlReset(SET);
	ResetChip();
	Delay(1000);
	SPIPutChar(0xff);

	SCI_ChipSelect(RESET);
	SDI_ChipSelect(RESET);
	ControlReset(RESET);

	//ResetChip();
	Delay(500);

	WriteRegister(SPI_MODE,0x08,0x20);
	Delay(500);

	//todo: while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);

	SDI_ChipSelect(SET);
	SPIPutChar(0x53);
	SPIPutChar(0xef);
	SPIPutChar(0x6e);
	SPIPutChar(0x03); //0x24
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	SPIPutChar(0x00);
	Delay(1000);
	SDI_ChipSelect(RESET);

}

void WriteRegister(uint8_t addressbyte, uint8_t highbyte, uint8_t lowbyte)
{
	SDI_ChipSelect(RESET);
	//while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);
	SCI_ChipSelect(SET);
	SPIPutChar(VS_WRITE_COMMAND);
	SPIPutChar(addressbyte);
	SPIPutChar(highbyte);
	SPIPutChar(lowbyte);
	//while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);
	SCI_ChipSelect(RESET);
}

uint16_t ReadRegister(uint8_t addressbyte){
	uint16_t result;
	SDI_ChipSelect(RESET);
	//while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);
	SCI_ChipSelect(SET);
	SPIPutChar(VS_READ_COMMAND);
	SPIPutChar(addressbyte);
	result = SPIPutChar(0) << 8;
	result |= SPIPutChar(0);
	//while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);
	SCI_ChipSelect(RESET);
	return result;

}

void ResetChip(){
	ControlReset(SET); /// NIE DIZA LA???
	Delay(1000);
	SPIPutChar(0xff);
	SCI_ChipSelect(RESET);
	SDI_ChipSelect(RESET);
	ControlReset(RESET);
	Delay(100);

	//while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);
	Delay(100);
}

uint16_t MaskAndShiftRight(uint16_t Source, uint16_t Mask, uint16_t Shift){
	return ( (Source & Mask) >> Shift );
}

void VS1003_sine(char pitch)
{
		WriteRegister(SPI_AUDATA, 0xac,0x45);
	WriteRegister(SPI_AICTRL0, 0x02,0xe7);
	WriteRegister(SPI_AICTRL1, 0x02,0xe7);
	WriteRegister(SPI_AIADDR, 0x40,0x20);
}

void VS1003_regtest()
{
	/*int MP3Status = ReadRegister(SPI_STATUS);
	int MP3Mode = ReadRegister(SPI_MODE);
  int MP3Clock = ReadRegister(SPI_CLOCKF);
	int vsVersion ;
	printf("SCI_Mode (0x4800) = 0x%X\r\n",MP3Mode);

  printf("SCI_Status (0x48) = 0x%X\r\n",MP3Status);

  vsVersion = (MP3Status >> 4) & 0x000F; //Mask out only the four version bits
  printf("VS Version (VS1053 is 4) = %d\r\n",vsVersion);
 //The 1053B should respond with 4. VS1001 = 0, VS1011 = 1, VS1002 = 2, VS1003 = 3

  printf("SCI_ClockF = 0x%X\r\n",MP3Clock);
	WriteRegister(SPI_CLOCKF, 0x60, 0x00); //Set multiplier to 3.0x

  //From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz. 
  //Internal clock multiplier is now 3x.
  //Therefore, max SPI speed is 5MHz. 4MHz will be safe.
  //SPI.setClockDivider(SPI_CLOCK_DIV4); //Set SPI bus speed to 4MHz (16MHz / 4 = 4MHz)

  MP3Clock = ReadRegister(SPI_CLOCKF);
  printf("SCI_ClockF = 0x%X\r\n",MP3Clock);*/
}

void VS1003_PluginLoad()
{
	int i;
  for (i=0;i<CODE_SIZE;i++) {
    WriteRegister(atab[i], (dtab[i]>>8), (dtab[i]&0xff));
  }
}

void VS1003_Start(){
	//int i;
	/*ControlReset(SET);
	Delay(1000);
	SPIPutChar(0xFF);
	SCI_ChipSelect(RESET);
	SDI_ChipSelect(RESET);
	ControlReset(RESET);
	VS1003_SoftwareReset();
	Delay(1000);*/
	ResetChip();

	////while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);

	/*WriteRegister(SPI_MODE,0x08,0x00);
	WriteRegister(SPI_CLOCKF,0x98,0x00);
	WriteRegister(SPI_AUDATA,0xAC,0x45);
	WriteRegister(SPI_BASS,0x08,0x00);
	WriteRegister(SPI_VOL,0x0B,0x0B);
	WriteRegister(SPI_STATUS,0,0x33);*/
	//i=(SM_TESTS)|(SM_SDISHARE)|(SM_STREAM)|(SM_SDINEW);
	WriteRegister(SPI_CLOCKF,0x60,0x00);
	WriteRegister(0x00, 0x08, 0x02);

	////while(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0);
}

int VS1003_SendMusicBytes(uint8_t* music,int quantity){
	////if(HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0 || quantity < 1) return 0;
	//if(quantity > 32) quantity = 32;
	SDI_ChipSelect(SET);
	int o = 0;
	while(1)
	{
		int t = quantity;
		if(t > 32) t = 32;
		for (int k=o; k < o+t; k++)
		{
			SPIPutChar(music[k]);
		}
		o += t;
		quantity -= t;
		////if(quantity == 0 || HAL_GPIO_ReadPin(DREQ_PORT,DREQ_PIN) == 0) break;
	}
	SDI_ChipSelect(RESET);
	return o;
}

void VS1003_SoftwareReset(){
	WriteRegister(SPI_MODE,0x00,0x04);
}

uint8_t 	VS1003_GetVolume(){
	return ( ReadRegister(SPI_VOL) & 0x00FF );
}
/**
 * Function sets the same volume level to both channels.
 * @param xMinusHalfdB describes damping level as a multiple
 * 		of 0.5dB. Maximum volume is 0 and silence is 0xFEFE.
 */
void	VS1003_SetVolume(uint8_t xMinusHalfdB){
	WriteRegister(SPI_VOL,xMinusHalfdB,xMinusHalfdB);
}

/**
 * Function increases volume level for both channels.
 * If it is impossible to increase volume as much as
 * xHalfdB, volume is set to maximum.
 * @param xHalfdB multiple of 0.5dB describing how
 * 		much volume should be turned up.
 */
void 	VS1003_VolumeUp(uint8_t xHalfdB){
	uint8_t currentVol = VS1003_GetVolume();

	//if it is impossible to turn volume up as we want
	if ( (uint8_t)(currentVol - xHalfdB) > currentVol )
		VS1003_SetVolume(0);
	else
		VS1003_SetVolume(currentVol - xHalfdB);
}

/**
 * Function decreases volume level for both channels.
 * @note If it is impossible to decrease volume as much as
 * xHalfdB, volume is muted.
 * @param xHalfdB multiple of 0.5dB describing how
 * 		much volume should be turned down.
 */
void	VS1003_VolumeDown(uint8_t xHalfdB){
	uint8_t currentVol = VS1003_GetVolume();

	//if it is impossible to turn volume down as we want
	if ( currentVol + xHalfdB < currentVol ||
			currentVol + xHalfdB == 255)
			VS1003_SetVolume(0xFE);
		else
			VS1003_SetVolume(currentVol + xHalfdB);
}

/**
 * Functions returns level of treble enhancement.
 * @return Returned value describes enhancement in multiplies
 * 		of 1.5dB. 0 value means no enhancement, 8 max (12dB).
 */
uint8_t	VS1003_GetTreble(){
	return ( (ReadRegister(SPI_BASS) & 0xF000) >> 12);
}

/**
 * Sets treble level.
 * @note If xOneAndHalfdB is greater than max value, sets treble
 * 		to maximum.
 * @param xOneAndHalfdB describes level of enhancement. It is a multiplier
 * 		of 1.5dB. 0 - no enhancement, 8 - maximum, 12dB.
 * @return void
 */
void	VS1003_SetTreble(uint8_t xOneAndHalfdB){
	uint16_t bassReg = ReadRegister(SPI_BASS);
	if ( xOneAndHalfdB <= 8)
		WriteRegister( SPI_BASS, MaskAndShiftRight(bassReg,0x0F00,8) | (xOneAndHalfdB << 4), bassReg & 0x00FF );
	else
		WriteRegister( SPI_BASS, MaskAndShiftRight(bassReg,0x0F00,8) | 0x80, bassReg & 0x00FF );
}

/**
 * Turns up treble.
 * @note If xOneAndHalfdB is greater than max value, sets treble
 * 		to maximum.
 * @param xOneAndHalfdB describes how many dBs add to current treble level.
 *  	It is a multiplier of 1.5dB.
 * @return void
 */
void	VS1003_TrebleUp(uint8_t xOneAndHalfdB){
	uint8_t currentTreble = VS1003_GetTreble();

	if ((uint8_t)(currentTreble - xOneAndHalfdB) > currentTreble)
		VS1003_SetTreble(0);
	else
		VS1003_SetTreble(currentTreble - xOneAndHalfdB);
}

/**
 * Turns down treble.
 * @note If it is impossible to decrease by xdB, the minimum value is set (off).
 * @param xOneAndHalfdB describes how many dBs subtract from current treble level.
 *  	It is a multiplier of 1.5dB.
 * @return void
 */
void	VS1003_TrebleDown(uint8_t xOneAndHalfdB){
	uint8_t currentTreble = VS1003_GetTreble();

	if (currentTreble + xOneAndHalfdB >= 8)
		VS1003_SetTreble(8);
	else
		VS1003_SetTreble(currentTreble + xOneAndHalfdB);
}
/**
 * Sets low limit frequency of treble enhancer.
 * @note new frequency is set only if argument is valid.
 * @param xkHz The lowest frequency enhanced by treble enhancer.
 * 		Values from 0 to 15 (in kHz)
 * @return void
 */
void	VS1003_SetTrebleFreq(uint8_t xkHz){
	uint16_t bassReg = ReadRegister(SPI_BASS);
	if ( xkHz <= 15 )
		WriteRegister( SPI_BASS, MaskAndShiftRight(bassReg,0xF000,8) | xkHz, bassReg & 0x00FF );
}

/**
 * Returns level of bass boost in dB.
 * @return Value of bass enhancement from 0 (off) to 15(dB).
 */
uint8_t	VS1003_GetBass(){
	return ( (ReadRegister(SPI_BASS) & 0x00F0) >> 4);
}

/**
 * Sets bass enhancement level (in dB).
 * @note If xdB is greater than max value, bass enhancement is set to its max (15dB).
 * @param xdB Value of bass enhancement from 0 (off) to 15(dB).
 * @return void
 */
void	VS1003_SetBass(uint8_t xdB){
	uint16_t bassReg = ReadRegister(SPI_BASS);
	if (xdB <= 15)
		WriteRegister(SPI_BASS, (bassReg & 0xFF00) >> 8, (bassReg & 0x000F) | (xdB << 4) );
	else
		WriteRegister(SPI_BASS, (bassReg & 0xFF00) >> 8, (bassReg & 0x000F) | 0xF0 );
}

/**
 * Increases level of bass enhancement.
 * @note If it is impossible to increase by xdB, the maximum value is set.
 * @param xdB Value of bass enhancement from 0 (off) to 15(dB).
 */
void	VS1003_BassUp(uint8_t xdB){
	uint8_t currentBass = VS1003_GetBass();

	if (currentBass + xdB >= 15)
		VS1003_SetBass(15);
	else
		VS1003_SetBass(currentBass + xdB);
}

/**
 * Decreases level of bass enhancement.
 * @note If it is impossible to decrease by xdB, the minimum value is set.
 * @param xdB Value of bass enhancement from 0 (off) to 15(dB).
 */
void	VS1003_BassDown(uint8_t xdB){
	uint8_t currentBass = VS1003_GetBass();
	if (currentBass - xdB > currentBass)
		VS1003_SetBass(0);
	else
		VS1003_SetBass(currentBass - xdB);
}

/**
 * Sets low limit frequency of bass enhancer.
 * @note new frequency is set only if argument is valid.
 * @param xTenHz The lowest frequency enhanced by bass enhancer.
 * 		Values from 2 to 15 ( equal to 20 - 150 Hz).
 * @return void
 */
void	VS1003_SetBassFreq(uint8_t xTenHz){
	uint16_t bassReg = ReadRegister(SPI_BASS);
	if (xTenHz >=2 && xTenHz <= 15)
		WriteRegister(SPI_BASS, MaskAndShiftRight(bassReg,0xFF00,8), (bassReg & 0x00F0) | xTenHz );
}

uint16_t	VS1003_GetDecodeTime(){
	return ReadRegister(SPI_DECODE_TIME);
}

uint16_t	VS1003_GetBitrate(){
	uint16_t bitrate = (ReadRegister(SPI_HDAT0) & 0xf000) >> 12;
	uint8_t ID = (ReadRegister(SPI_HDAT1) & 0x18) >> 3;
	uint16_t res;
	if (ID == 3)
	{	res = 32;
		while(bitrate>13)
		{
			res+=64;
			bitrate--;
		}
		while (bitrate>9)
		{
			res+=32;
			bitrate--;
		}
		while (bitrate>5)
		{
			res+=16;
			bitrate--;
		}
		while (bitrate>1)
		{
			res+=8;
			bitrate--;
		}
	}
	else
	{	res = 8;

		while (bitrate>8)
		{
			res+=16;
			bitrate--;
		}
		while (bitrate>1)
		{
			res+=8;
			bitrate--;
		}
	}
	return res;
}

uint16_t	VS1003_GetSampleRate(){
	return (ReadRegister(SPI_AUDATA) & 0xFFFE);
}
