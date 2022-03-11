/*
  ******************************************************************************
  * @file           : wizchip_net_def_conf.h
  * @author         : Enrico
  * @date           : 22 dic 2021
  ******************************************************************************
*/
#include "main.h"

//Config
extern SPI_HandleTypeDef hspi1;

#define W5500_SPI_HANDLE    hspi1

#define W5500_CS_PORT       GPIOA
#define W5500_CS_PIN        GPIO_PIN_4
#define W5500_RST_PORT      GPIOA
#define W5500_RST_PIN       GPIO_PIN_3

#define DEFAULT_MAC_ADDR    {0x00,0xf1,0xbe,0xc4,0xa1,0x05}
#define DEFAULT_IP_ADDR     {192,168,10,201}
#define DEFAULT_SUB_MASK    {255,255,255,0}
#define DEFAULT_GW_ADDR     {0,0,0,0}
#define DEFAULT_DNS_ADDR    {8,8,8,8}


