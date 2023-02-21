/*
 * WQ25xxConf.h
 *
 *  Created on: Feb 20, 2023
 *      Author: Admin
 */

#ifndef INC_WQ25XXCONF_H_
#define INC_WQ25XXCONF_H_

#include "main.h"

extern SPI_HandleTypeDef hspi2;

#define _W25QXX_SPI                   hspi2
#define _W25QXX_CS_GPIO               GPIOB
#define _W25QXX_CS_PIN                GPIO_PIN_12
#define _W25QXX_USE_FREERTOS          0
#define _W25QXX_DEBUG                 0

#endif /* INC_WQ25XXCONF_H_ */
