/*
 * My_SDCard.h
 *
 *  Created on: Jan 28, 2023
 *      Author: Admin
 */

#ifndef INC_MY_SDCARD_H_
#define INC_MY_SDCARD_H_

#include "main.h"
#include "fatfs.h"
#include "string.h"
#include "Frame.h"
#include "ed25519.h"
#include "sha3.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;



HAL_StatusTypeDef tree_hash(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t* md, uint32_t *size, uint32_t *rd);
#endif /* INC_MY_SDCARD_H_ */
