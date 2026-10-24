#include "w25q.h"
uint32_t W25Q_Read_JEDEC_ID(void){
	W25Q_CS_L;
	uint8_t cmd = W25Q_JEDEC_ID;
	HAL_SPI_Transmit(&hspi1,&cmd,1,0xff);
	uint8_t data[3] = {0};
	HAL_SPI_Receive(&hspi1,data,3,0xff);
	W25Q_CS_H;
	uint32_t id = data[0] << 16 | data[1] << 8 | data[2];
	return id;
}
void W25Q_WriteEnable(void){
	W25Q_CS_L;
	uint8_t cmd = W25Q_WRITE_ENABLE;
	HAL_SPI_Transmit(&hspi1,&cmd,1,0xff);
	W25Q_CS_H;
}
void W25Q_WaitBusy(void){
	W25Q_CS_L;
	uint8_t cmd = W25Q_READ_STATUS_REGESTER_1;
	HAL_SPI_Transmit(&hspi1,&cmd,1,0xff);
	uint8_t status = 0;
	do{
		HAL_SPI_Receive(&hspi1,&status,1,0xff);
	}while((status & 0x01) == 0x01);
	W25Q_CS_H;
}
void W25Q_EraseSector(uint32_t addr){
	W25Q_WriteEnable();
	W25Q_WaitBusy();
	uint8_t buf[4];
	buf[0] = W25Q_SECTOR_ERASE_4KB;
	buf[3] = addr & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 16) & 0xff;
	W25Q_CS_L;
	HAL_SPI_Transmit(&hspi1,buf,4,0xff);
	W25Q_CS_H;
	W25Q_WaitBusy();
}
void W25Q_WriteData(uint32_t addr,uint8_t *data,uint32_t size){
	W25Q_WriteEnable();
	W25Q_WaitBusy();
	uint8_t buf[4];
	buf[0] = W25Q_PAGE_PROGRAM;
	buf[3] = addr & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 16) & 0xff;
	W25Q_CS_L;
	HAL_SPI_Transmit(&hspi1,buf,4,0xff);
	HAL_SPI_Transmit(&hspi1,data,size,0xffff);
	W25Q_CS_H;
	W25Q_WaitBusy();
}
void W25Q_ReadData(uint32_t addr,uint8_t *data,uint32_t size){
	W25Q_WaitBusy();
	uint8_t buf[4];
	buf[0] = W25Q_READ_DATA;
	buf[3] = addr & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 16) & 0xff;
	W25Q_CS_L;
	HAL_SPI_Transmit(&hspi1,buf,4,0xff);
	HAL_SPI_Receive(&hspi1,data,size,0xffff);
	W25Q_CS_H;
}
void W25Q_WriteDataDMA(uint32_t addr,uint8_t *data,uint32_t size){
	W25Q_WriteEnable();
	W25Q_WaitBusy();
	uint8_t buf[4];
	buf[0] = W25Q_PAGE_PROGRAM;
	buf[3] = addr & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 16) & 0xff;
	W25Q_CS_L;
	HAL_SPI_Transmit(&hspi1,buf,4,0xff);
	HAL_SPI_Transmit_DMA(&hspi1,data,size);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {}
	W25Q_CS_H;
	W25Q_WaitBusy();
}
void W25Q_ReadDataDMA(uint32_t addr,uint8_t *data,uint32_t size){
	W25Q_WriteEnable();
	W25Q_WaitBusy();
	uint8_t buf[4];
	buf[0] = W25Q_READ_DATA;
	buf[3] = addr & 0xff;
	buf[2] = (addr >> 8) & 0xff;
	buf[1] = (addr >> 16) & 0xff;
	W25Q_CS_L;
	HAL_SPI_Transmit(&hspi1,buf,4,0xff);
	HAL_SPI_Receive_DMA(&hspi1,data,size);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {}
	W25Q_CS_H;
}
	