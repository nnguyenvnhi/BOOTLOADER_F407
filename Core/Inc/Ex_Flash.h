/*
 * Ex_Flash.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Van - Nhi Nguyen
 */

#ifndef INC_EX_FLASH_H_
#define INC_EX_FLASH_H_

#include "main.h"
#include "W25Qxx.h"

#define ADDRESS_STORE_BASE 0
#define ADDRESS_STORE_BOOTLOADER_CERT	ADDRESS_STORE_BASE
#define ADDRESS_STORE_APPLICATION1_CERT	ADDRESS_STORE_BOOTLOADER_CERT + 160
#define ADDRESS_STORE_APPLICATION2_CERT	ADDRESS_STORE_APPLICATION1_CERT	+ 160

typedef enum{
	SECTOR_ERASE = 0x00,
	BLOCK32_ERASE,
	BLOCK64_ERASE,
	FULLCHIP_ERASE
}Erase_Type;

extern SPI_HandleTypeDef hspi2;

typedef struct{
	uint8_t signature[64];
	uint8_t publickey[32];
	uint8_t hash[64];
}Certificate;

static Certificate Bootloader_Cert;
static Certificate Application1_Cert;
static Certificate Application2_Cert;

void Update_Signature(Certificate *cert, uint8_t* signature);
void Update_Publickey(Certificate *cert, uint8_t* publickey);
void Update_HashValue(Certificate *cert, uint8_t* hashvalue);
void Update_BootloaderCert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue);
void Update_Application1Cert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue);
void Update_Application2Cert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue);
void StoreAllCert();

#endif /* INC_EX_FLASH_H_ */
