/*
 * Ex_Flash.c
 *
 *  Created on: Feb 20, 2023
 *      Author: Van - Nhi Nguyen
 */
#include "Ex_Flash.h"


__weak void Update_Signature(Certificate *cert, uint8_t* signature){
	for(uint8_t index = 0; index < 64; index++){
		cert->signature[index] = signature[index];
	}
}

__weak void Update_Publickey(Certificate *cert, uint8_t* publickey){
	for(uint8_t index = 0;index < 32; index++){
		cert->publickey[index] = publickey[index];
	}
}
__weak void Update_HashValue(Certificate *cert, uint8_t* hashvalue){
	for(uint8_t index = 0;index < 32; index++){
			cert->hash[index] = hashvalue[index];
	}
}

__weak void Update_BootloaderCert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue){
	Update_Signature(&Bootloader_Cert, signature);
	Update_Publickey(&Bootloader_Cert, publickey);
	Update_HashValue(&Bootloader_Cert, hashvalue);
}

__weak void Update_Application1Cert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue){
	Update_Signature(&Application1_Cert, signature);
	Update_Publickey(&Application1_Cert, publickey);
	Update_HashValue(&Application1_Cert, hashvalue);
}

__weak void Update_Application2Cert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue){
	Update_Signature(&Application2_Cert, signature);
	Update_Publickey(&Application2_Cert, publickey);
	Update_HashValue(&Application2_Cert, hashvalue);
}

__weak void Write_BootCert_To_ExFlash(){
	WriteFlash(ADDRESS_STORE_BOOTLOADER_CERT, Bootloader_Cert.signature, 64);
	WriteFlash(ADDRESS_STORE_BOOTLOADER_CERT + 64, Bootloader_Cert.publickey, 32);
	WriteFlash(ADDRESS_STORE_BOOTLOADER_CERT, Bootloader_Cert.hash, 64);
}

__weak void Write_App1Cert_To_ExFlash(){
	WriteFlash(ADDRESS_STORE_APPLICATION1_CERT, Application1_Cert.signature, 64);
	WriteFlash(ADDRESS_STORE_APPLICATION1_CERT + 64, Application1_Cert.publickey, 32);
	WriteFlash(ADDRESS_STORE_APPLICATION1_CERT + 64 + 32, Application1_Cert.hash, 64);
}

__weak void Write_App2Cert_To_ExFlash(){
	WriteFlash(ADDRESS_STORE_APPLICATION2_CERT, Application2_Cert.signature, 64);
	WriteFlash(ADDRESS_STORE_APPLICATION2_CERT + 64, Application2_Cert.publickey, 32);
	WriteFlash(ADDRESS_STORE_APPLICATION2_CERT + 64 + 32, Application2_Cert.hash, 64);
}

__weak void StoreAllCert(){
	Erase(ADDRESS_STORE_BASE, (Erase_Type)SECTOR_ERASE);
	Write_BootCert_To_ExFlash();
	Write_App1Cert_To_ExFlash();
	Write_App2Cert_To_ExFlash();
}


