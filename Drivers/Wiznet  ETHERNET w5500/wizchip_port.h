/*
  ******************************************************************************
  * @file           : wizchip_port.h
  * @author         : Enrico
  * @date           : 22 dic 2021
  ******************************************************************************
*/

//Functions
void w5500_cris_enter(void);
void w5500_cris_exit(void);
void w5500_cs_select(void);
void w5500_cs_deselect(void);
uint8_t w5500_spi_readbyte(void);
void w5500_spi_writebyte(uint8_t wb);
void w5500_spi_readburst(uint8_t* pBuf, uint16_t len);
void w5500_spi_writeburst(uint8_t* pBuf, uint16_t len);
void w5500_hard_reset(void);
int w5500_chip_init(void);
void w5500_phy_init(void);
void w5500_network_info_init(void);
void w5500_network_info_show(void);
int w5500_init(void);

