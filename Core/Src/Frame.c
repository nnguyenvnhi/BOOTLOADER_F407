#include "Frame.h"

HAL_StatusTypeDef sd_init(){
	if(BSP_SD_Init() == MSD_OK ) return HAL_OK;
	else return HAL_ERROR;
}

FRESULT sd_mount(FATFS *fatfs){
	return f_mount(fatfs, "", 1);
}

FRESULT sd_open(FIL *myfile, const char *file_name, uint8_t mode){
	return f_open(myfile, file_name, mode);
}

UINT sd_write(FIL *myfile, const uint8_t *write_buf, uint16_t size){
		UINT bw;
		f_write (myfile, write_buf, size, &bw);
		return bw;
}

uint32_t sd_read(FIL *myfile, uint8_t *read_buf, uint32_t size){
	uint32_t br = 0;
	f_read(myfile, read_buf, size,(UINT*) &br);
	return br;
}

void sd_close(FIL *myfile){
	f_close(myfile);
}

void Frame_SendResponseFrame(defResponseType response){
	defResponsePacket res_fr = {
			.Sof = SOF_SIGNAL,
			.Type = RESPONSE_TYPE,
			.DataLength = {0x00, 0x01,},
			.Command = response,
			.Eof = EOF_SIGNAL,
	};

	res_fr.Crc.DataUint32 = crc32(&response, 1);

	HAL_UART_Transmit(&MY_UART, (uint8_t*)&res_fr.Sof, 1, 1000);
	HAL_UART_Transmit(&MY_UART, (uint8_t*)&res_fr.Type, 1, 1000);
	HAL_UART_Transmit(&MY_UART, (uint8_t*)res_fr.DataLength, 2, 1000);
	HAL_UART_Transmit(&MY_UART, (uint8_t*)&res_fr.Command, 1, 1000);
	HAL_UART_Transmit(&MY_UART, (uint8_t*)res_fr.Crc.DataArray, 4, 1000);
	HAL_UART_Transmit(&MY_UART, (uint8_t*)&res_fr.Eof, 1, 1000);
}


defReturnType Frame_ReadStartFrame(){
	uint8_t buf[COMMAND_FRAME_LENGHT] = {0,};

	HAL_UART_Receive(&MY_UART, buf, COMMAND_FRAME_LENGHT, 3000);

	if(!((buf[0] == SOF_SIGNAL)&&(buf[COMMAND_FRAME_LENGHT - 1]  == EOF_SIGNAL)))
		return HANDLE_FAILED;

	if(buf[1] != COMMAND_TYPE) return HANDLE_FAILED;

	if(buf[4] != START_COMMAND) return HANDLE_FAILED;

	return HANDLE_OK;
}


defReturnType Frame_ReadStopFrame(){
	uint8_t buf[COMMAND_FRAME_LENGHT] = {0,};

	HAL_UART_Receive(&MY_UART, buf, COMMAND_FRAME_LENGHT, 3000);

	if(!((buf[0] = SOF_SIGNAL)&&(buf[COMMAND_FRAME_LENGHT - 1]  = EOF_SIGNAL)))
		return HANDLE_FAILED;

	if(buf[1] != COMMAND_TYPE) return HANDLE_FAILED;

	if(buf[4] != STOP_COMMAND) return HANDLE_FAILED;

	return HANDLE_OK;
}


defReturnType Frame_ReadHeaderFrame(){
	uint8_t buf[HEADER_FRAME_LENGHT] = {0,};
	uint32To4Bytes crc_merge;
	HAL_UART_Receive(&MY_UART, (uint8_t *) buf, HEADER_FRAME_LENGHT, 3000);

	if(!((buf[0] == SOF_SIGNAL)&&(buf[HEADER_FRAME_LENGHT - 1]  == EOF_SIGNAL)))
		return HANDLE_FAILED;

	crc_merge.DataArray[0] = buf[11];
	crc_merge.DataArray[1] = buf[10];
	crc_merge.DataArray[2] = buf[9];
	crc_merge.DataArray[3] = buf[8];

	if(buf[1] != HEADER_TYPE) return HANDLE_FAILED;

	uint32_t crc_header = crc32((uint8_t*)(&buf[0] + 4), 4);

	if(crc_header != crc_merge.DataUint32) return HANDLE_FAILED;
	Frame_NumberOfKbyteData = buf[6]*256 + buf[7];

	return HANDLE_OK;
}


defReturnType Frame_ReadDataFrame(uint8_t* data){
	uint32To4Bytes crc_merge;
	uint8_t buf[4];
//	while(MY_UART.RxXferCount == 0);
	HAL_UART_Receive(&MY_UART, buf, 4, 1000);
	Frame_DataLenghtPerFrame = buf[2]*256 + buf[3];

	HAL_UART_Receive(&MY_UART, data, Frame_DataLenghtPerFrame + 5, 3000);

	if(!((buf[0] == SOF_SIGNAL)&&(data[Frame_DataLenghtPerFrame + 5 - 1] == EOF_SIGNAL))) {
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(200);
		return HANDLE_FAILED;
	}

	if(buf[1] != DATA_TYPE){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		HAL_Delay(200);
		return HANDLE_FAILED;
	}

	crc_merge.DataArray[0] = data[Frame_DataLenghtPerFrame + 5 - 2];
	crc_merge.DataArray[1] = data[Frame_DataLenghtPerFrame + 5 - 3];
	crc_merge.DataArray[2] = data[Frame_DataLenghtPerFrame + 5 - 4];
	crc_merge.DataArray[3] = data[Frame_DataLenghtPerFrame + 5 - 5];

	uint32_t crc_data = crc32((uint8_t*)(&data[0]), Frame_DataLenghtPerFrame);

	if(crc_data != crc_merge.DataUint32){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_Delay(200);
		return HANDLE_FAILED;
	}

	return HANDLE_OK;
}


defReturnType Frame_STM32OTA(uint8_t slot){
	if(Frame_InitFlash(slot) != HANDLE_OK) return HANDLE_FAILED;

	Frame_SendResponseFrame(RESPONSE_ACK);

	if(Frame_ReadStartFrame() == HANDLE_OK){
		Frame_SendResponseFrame(RESPONSE_ACK);
	}
	else{
		Frame_SendResponseFrame(RESPONSE_NACK);
		Frame_ReadStopFrame();
		return HANDLE_FAILED;
	}

	if(Frame_ReadHeaderFrame() == HANDLE_OK){
		Frame_SendResponseFrame(RESPONSE_ACK);
	}
	else{
		Frame_SendResponseFrame(RESPONSE_NACK);
		Frame_ReadStopFrame();
		return HANDLE_FAILED;
	}

	for(uint16_t NoFrame = 0; NoFrame < Frame_NumberOfKbyteData; NoFrame ++){
		uint8_t bufOTA[DATA_FRAME_LENGHT - 4];
		uint8_t flag = 0x00;
		__HAL_TIM_SET_COUNTER(&htim1,0);
		while(__HAL_TIM_GET_COUNTER(&htim1) < 20000){
			if(Frame_ReadDataFrame(bufOTA) == HANDLE_OK){
				Frame_WriteFlashToSlot(slot,(uint8_t*)bufOTA, Frame_DataLenghtPerFrame);
				Frame_FlashAddUntillNow += Frame_DataLenghtPerFrame;
				Frame_TotalSize += Frame_DataLenghtPerFrame;
				Frame_SendResponseFrame(RESPONSE_ACK);
				flag = 0x01;
				break;
			}
			else{
				memset(bufOTA, 0, DATA_FRAME_LENGHT - 4);
			}
		}
		if(flag == 0x00){
			Frame_SendResponseFrame(RESPONSE_NACK);
			Frame_ReadStopFrame();
			return HANDLE_FAILED;
		}
		memset(bufOTA, 0, DATA_FRAME_LENGHT - 4);
	}
	Frame_FlashAddUntillNow = 0;
	HAL_FLASH_Lock();
	return HANDLE_OK;
}


defReturnType Frame_InitFlash(uint8_t slot){
	defReturnType ret;

	ret = HAL_FLASH_Unlock();
	if(ret != (uint8_t)HAL_OK){
		return HANDLE_FAILED;
	}

	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError;

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;

	if(slot == 0x00){
		EraseInitStruct.Sector        = FLASH_SECTOR_5;
	}
	else if(slot == 0x01){
		EraseInitStruct.Sector        = FLASH_SECTOR_7;
	}
	else if(slot == 0x02){
		EraseInitStruct.Sector        = FLASH_SECTOR_9;
	}
	EraseInitStruct.NbSectors     = 2;           //erase 2 sectors
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

	ret = HAL_FLASHEx_Erase( &EraseInitStruct, &SectorError );
	if( ret != (uint8_t)HAL_OK )
	{
		return HANDLE_FAILED;
	}

	return HANDLE_OK;
}


defReturnType Frame_WriteFlashToSlot(uint8_t slot, uint8_t* data, uint16_t lenght){
	uint32_t BaseAddress = 0;
	if(slot == FLASH_SLOT_0) BaseAddress = FLASH_SLOT_0_ADD;
	else if(slot == FLASH_SLOT_1) BaseAddress = FLASH_SLOT_1_ADD;
	else if(slot == FLASH_SLOT_2) BaseAddress = FLASH_SLOT_2_ADD;
	else{}

	for(uint16_t index = 0; index < lenght; index++){
//		uint8_t data_convert = Frame_ConvertCharToUint8(data[index], data[index + 1]);
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, BaseAddress + Frame_FlashAddUntillNow + index, data[index]) != HAL_OK)
			return HANDLE_FAILED;
//		index+=2;
	}

	return HANDLE_OK;
}


uint8_t Frame_ConvertCharToUint8(uint8_t c1, uint8_t c2){
	uint8_t c11;
	uint8_t c22;

	if(!((c1<='9')&&(c1>='0'))){
		if(c1 == 'A') c11 = 10;
		else if(c1 == 'B') c11 = 11;
		else if(c1 == 'C') c11 = 12;
		else if(c1 == 'D') c11 = 13;
		else if(c1 == 'E') c11 = 14;
		else if(c1 == 'F') c11 = 15;
	}
	else{
		c11 = c1 - 48;
	}

	if(!((c2<='9')&&(c2>='0'))){
		if(c2 == 'A') c22 = 10;
		else if(c2 == 'B') c22 = 11;
		else if(c2 == 'C') c22 = 12;
		else if(c2 == 'D') c22 = 13;
		else if(c2 == 'E') c22 = 14;
		else if(c2 == 'F') c22 = 15;
	}
	else{
		c22 = c2 - 48;
	}

	return (c11*16 + c22);
}



defReturnType Frame_WriteCfg(defFlashCfg *Flash_Cfg){//write to sector 11 in STM32F407
	defReturnType ret;

	ret = HAL_FLASH_Unlock();
	if(ret != (uint8_t)HAL_OK){
		return HANDLE_FAILED;
	}

	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError;

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;

	EraseInitStruct.Sector        = FLASH_SECTOR_11;

	EraseInitStruct.NbSectors     = 1;           //erase 1 sectors
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;

	ret = HAL_FLASHEx_Erase( &EraseInitStruct, &SectorError );
	if( ret != (uint8_t)HAL_OK )
	{
		return HANDLE_FAILED;
	}

	uint32_t baseaddr = 0x080E0000; //Sector 11 in STM32F407

	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, baseaddr, Flash_Cfg->FlashOnGoing) != HAL_OK)
			return HANDLE_FAILED;
	if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, baseaddr + 1, Flash_Cfg->ResetCause) != HAL_OK)
			return HANDLE_FAILED;

	HAL_FLASH_Lock();
	return HANDLE_OK;
}


defReturnType Frame_ReadCfg(defFlashCfg* Cfg){//write to sector 4 in STM32F407
	defReturnType ret;

	ret = HAL_FLASH_Unlock();
	if(ret != (uint8_t)HAL_OK){
		return HANDLE_FAILED;
	}


	uint32_t baseaddr = 0x080E0000; //Sector 4 in STM32F407

	uint32_t Rx_Buf = *(__IO uint32_t *)baseaddr;
	Cfg->FlashOnGoing = (uint8_t)((Rx_Buf)&0xFF);
	Cfg->ResetCause = (uint8_t)(((Rx_Buf)>>8)&0xFF);

	HAL_FLASH_Lock();
	return HANDLE_OK;
}


defReturnType Frame_LoadFlash(uint8_t ToSlot){
	uint32_t BaseAddr = 0;

	if(ToSlot == FLASH_SLOT_0){
		BaseAddr = FLASH_SLOT_0_ADD;
		Frame_InitFlash(FLASH_SLOT_0);
	}
	else if(ToSlot == FLASH_SLOT_1){
		BaseAddr = FLASH_SLOT_1_ADD;
		Frame_InitFlash(FLASH_SLOT_1);
	}
	else{

	}

	uint32_t LoadFromAddr = FLASH_SLOT_2_ADD; //Sector 4 in STM32F407


	for(uint32_t index = 0; index <= Frame_TotalSize; index+=4){
		uint32_t Rx_Buf = *(__IO uint32_t *)(LoadFromAddr + index);
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, BaseAddr + index, Rx_Buf) != HAL_OK)
					return HANDLE_FAILED;
	}

	HAL_FLASH_Lock();
	return HANDLE_OK;
}

HAL_StatusTypeDef ota_and_write_to_sd_card(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t mode){

	if(sd_init() == HAL_ERROR) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	if(sd_mount(fatfs) != FR_OK) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	if(sd_open(myfile, file_name, FA_WRITE|FA_CREATE_ALWAYS) != FR_OK) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);


	Frame_SendResponseFrame(RESPONSE_ACK);

	if(Frame_ReadStartFrame() == HANDLE_OK){
		Frame_SendResponseFrame(RESPONSE_ACK);
	}
	else{
		Frame_SendResponseFrame(RESPONSE_NACK);
		Frame_ReadStopFrame();
		return HAL_ERROR;
	}

	if(Frame_ReadHeaderFrame() == HANDLE_OK){
		Frame_SendResponseFrame(RESPONSE_ACK);
	}
	else{
		Frame_SendResponseFrame(RESPONSE_NACK);
		Frame_ReadStopFrame();
		return HAL_ERROR;
	}

	for(uint16_t NoFrame = 0; NoFrame < Frame_NumberOfKbyteData; NoFrame ++){
		uint8_t bufOTA[DATA_FRAME_LENGHT - 4];
		uint8_t flag = 0x00;
		__HAL_TIM_SET_COUNTER(&htim1,0);
		while(__HAL_TIM_GET_COUNTER(&htim1) < 10000){
			if(Frame_ReadDataFrame(bufOTA) == HANDLE_OK){
				if(sd_write(myfile, bufOTA, Frame_DataLenghtPerFrame)!=Frame_DataLenghtPerFrame){
//					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
					HAL_Delay(500);
				}
//				Frame_WriteFlashToSlot(FLASH_SLOT_0,(uint8_t*)bufOTA, Frame_DataLenghtPerFrame);
				Frame_FlashAddUntillNow += Frame_DataLenghtPerFrame;
				Frame_TotalSize += Frame_DataLenghtPerFrame;
				Frame_SendResponseFrame(RESPONSE_ACK);
				flag = 0x01;
				break;
			}
			else{
				break;
			}
		}
		if(flag == 0x00){
			Frame_SendResponseFrame(RESPONSE_NACK);
			Frame_ReadStopFrame();
			return HAL_ERROR;
		}
		memset(bufOTA, 0, DATA_FRAME_LENGHT - 4);
	}
	Frame_FlashAddUntillNow = 0;
//	HAL_FLASH_Lock();
	sd_close(myfile);
	return HAL_OK;
}


void store_into_flash(uint8_t* buffer, uint32_t address, uint16_t size){
	for(uint16_t s = 0; s < size; s++){
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address, *(buffer));
		address++;
		buffer++;
	}
}


void test(FATFS *fatfs, FIL *myfile){
	uint8_t bufff[1024] = {0};


	  sd_init();
	  sd_mount(fatfs);
	  sd_open(myfile,"testc.bin" , FA_WRITE|FA_CREATE_ALWAYS);
		for(int i = 0;i<1024;i++){
			bufff[i] = i/5+1;
		}
//		for(int c = 0; c<10; c++){
//
//			 sd_write(myfile, bufff);
//		}
//	  sd_write(myfile, "NGUYEN VAN NHI");
//	  sd_write(myfile, "NGUYEN VAN NHI");
//	  sd_write(myfile, "NGUYEN VAN NHI");
//	  sd_write(myfile, bufff);
//	  sd_close(myfile);
}

uint32_t read_file_and_store_into_flash(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t mode, defFlashSlot slot, uint8_t *signature, uint8_t *publickey, uint8_t *hashvalue){

	uint32_t BaseAddress;
	if(sd_init() == HAL_ERROR) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
	if(sd_mount(fatfs) != FR_OK) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
	if(sd_open(myfile, file_name, mode) != FR_OK) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
	uint32_t size_file = f_size(myfile) - 160;

	if(Frame_InitFlash(slot) != HANDLE_OK) return HAL_ERROR;
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);

	if(slot == FLASH_SLOT_0) BaseAddress = FLASH_SLOT_0_ADD;
	else if(slot == FLASH_SLOT_1) BaseAddress = FLASH_SLOT_1_ADD;
	else if(slot == FLASH_SLOT_2) BaseAddress = FLASH_SLOT_2_ADD;
	else{}

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	uint16_t round = size_file/1024; round += (size_file%1024 != 0)?1:0;

	uint8_t buffer_cipher[1024] = {0,};
	uint8_t buffer_plaintext[1024] = {0,};

	if(sd_read(myfile, signature, 64) != 64) return HAL_ERROR;
	if(sd_read(myfile, publickey, 32) != 32) return HAL_ERROR;
	if(sd_read(myfile, hashvalue, 64) != 64) return HAL_ERROR;

	for(uint16_t r = 0; r < round - 1; r++){
		if(sd_read(myfile, buffer_cipher, 1024) != 1024) return HAL_ERROR;
		Chacha20_Xor(Gb_scKeyStream, Gb_sulCell, buffer_cipher, 1024, buffer_plaintext);
		store_into_flash(buffer_plaintext, BaseAddress + r*1024, 1024);
		memset(buffer_cipher,0,1024);
		memset(buffer_plaintext,0,1024);
	}

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);

	if(sd_read(myfile, buffer_cipher, size_file%1024) != size_file%1024) return HAL_ERROR;
	Chacha20_Xor(Gb_scKeyStream, Gb_sulCell, buffer_cipher, size_file%1024, buffer_plaintext);
	store_into_flash(buffer_plaintext, BaseAddress + (round - 1)*1024, size_file%1024);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);

	sd_close(myfile);
	HAL_FLASH_Lock();

	return size_file;
}

HAL_StatusTypeDef tree_hash_on_file(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t* md, uint32_t *size, uint32_t *rd){

	if(sd_init() == HAL_ERROR) return HAL_ERROR;

	if(sd_mount(fatfs) != FR_OK) return HAL_ERROR;

	if(sd_open(myfile, file_name, FA_READ) != FR_OK) return HAL_ERROR;

	uint32_t size_file = f_size(myfile) - 1;
	*size = size_file;
	uint16 round = size_file/1024; round += (size_file%64 != 0)?1:0;
	*rd = round;
	uint8_t buff_hash[round][64];

	for(uint16 i = 0;i<round - 1;i++){
		uint8 buff_file[1024] = {0,};
		if(sd_read(myfile, buff_file, 1024) != 1024) return HAL_ERROR;
		sha3(buff_file,1024, &buff_hash[i], 64);
	}

	uint8_t buff_file[1024] = {0,};
	if(sd_read(myfile, buff_file, size_file%1024) != size_file%1024) return HAL_ERROR;
	sd_close(myfile);
	sha3(buff_file,size_file%1024, &buff_hash[round-1], 64);


	uint8_t all_of_hash[round*64];
	for(uint16 i = 0; i< round; i++){
		for(uint16 j = 0;j<64;j++){
			all_of_hash[i*64+j] = buff_hash[i][j];
		}
	}

	sha3(all_of_hash, round*64, md, 64);


	return HAL_OK;
}

void read_chunk_data_in_flash(uint8_t *buf, uint32_t address, uint16_t size){
	for(uint32_t index = 0; index< size ; index++){
		*buf = *(__IO uint8_t *)(address + index);
		buf++;
	}
}

HAL_StatusTypeDef tree_hash_on_flash(defFlashSlot slot, uint8_t* md, uint32_t size, uint32_t *rd){
	uint16 round = size/1024; round += (size%64 != 0)?1:0;
	uint8_t buff_hash[round][64];
	uint32_t BaseAddress;

	if(slot == FLASH_SLOT_0) BaseAddress = FLASH_SLOT_0_ADD;
	else if(slot == FLASH_SLOT_1) BaseAddress = FLASH_SLOT_1_ADD;
	else if(slot == FLASH_SLOT_2) BaseAddress = FLASH_SLOT_2_ADD;
	else{}


	for(uint16 i = 0;i<round - 1;i++){
		uint8 buff_file[1024] = {0,};
		read_chunk_data_in_flash(buff_file, BaseAddress + i*1024, 1024);
		sha3(buff_file,1024, &buff_hash[i], 64);
	}

	uint8_t buff_file[1024] = {0,};
	read_chunk_data_in_flash(buff_file, BaseAddress + (round-1)*1024, size%1024);

	sha3(buff_file, size%1024, &buff_hash[round-1], 64);


	uint8_t all_of_hash[round*64];
	for(uint16 i = 0; i< round; i++){
		for(uint16 j = 0;j<64;j++){
			all_of_hash[i*64+j] = buff_hash[i][j];
		}
	}

	sha3(all_of_hash, round*64, md, 64);


	return HAL_OK;
}
