#ifndef FRAME_H_
#define FRAME_H_

#include "main.h"
#include "string.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "CRC32.h"
#include "fatfs.h"
//#include "My_SDCard.h"

#include "ed25519.h"
#include "sha3.h"
#include "ChaCha20.h"


HAL_StatusTypeDef sd_init();
FRESULT sd_mount(FATFS *fatfs);
FRESULT sd_open(FIL *myfile, const char *file_name, uint8_t mode);
UINT sd_write(FIL *myfile, const uint8_t *write_buf, uint16_t size);
uint32_t sd_read(FIL *myfile, uint8_t *read_buf,  uint32_t size);
void sd_close(FIL *myfile);




void get_signature_in_file(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t *signature);
void get_publickey_in_file(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t *publickey);
void get_hashvalue_in_file(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t *hashvalue);

void test(FATFS *fatfs, FIL *myfile);

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

extern TIM_HandleTypeDef htim1;
#define MY_UART huart6

#define COMMAND_FRAME_LENGHT 10
#define HEADER_FRAME_LENGHT 13
#define DATA_LENGHT 1024
#define DATA_FRAME_LENGHT 9 + DATA_LENGHT


#define FLASH_SLOT_0_ADD	0x08020000
#define FLASH_SLOT_1_ADD	0x08060000
#define FLASH_SLOT_2_ADD	0x080A0000

/* Number of frame data, for example: Length of file is 9.5Kbyte then Number = 10 */
static uint8_t Frame_NumberOfKbyteData = 0;
static uint16_t Frame_DataLenghtPerFrame = 0;
static uint16_t Frame_FlashAddUntillNow = 0;
static uint32_t Frame_TotalSize = 0;
typedef enum{
	FLASH_SLOT_0 = 0x00, //bootloader
	FLASH_SLOT_1,        //
	FLASH_SLOT_2
}defFlashSlot;

typedef enum{
    SOF_SIGNAL = 0x00,
    EOF_SIGNAL
}defTerminateFrame;

typedef enum{
    START_COMMAND = 0x00,
    STOP_COMMAND,
    HEADER_COMMAND,
}defCommandType;

typedef enum{
    COMMAND_TYPE = 0x00,
    HEADER_TYPE,
    DATA_TYPE,
    RESPONSE_TYPE
}defPacketType;


typedef enum{
    RESPONSE_ACK = 0x00,
    RESPONSE_NACK,
    RESPONSE_ABORT
}defResponseType;

typedef enum{
    HANDLE_OK = 0x00,
    HANDLE_FAILED
}defReturnType;

/* Convert from 4 bytes to uin32 type */
typedef union{
	uint8_t DataArray[4];
    uint32 DataUint32;
}uint32To4Bytes;

typedef struct{
	uint8_t DataSize[4];
    uint8_t Reserved1[4];
    uint8_t Reserved2[4];
    uint8_t Reserved3[4];
}defMetaInfo;

typedef struct{
	uint8_t Sof;
	uint8_t Type;
	uint8_t DataLength[2];
    uint8_t Command;
    uint32To4Bytes Crc;
    uint8_t Eof;
}defCommandPacket;

typedef struct{
	uint8_t Sof;
	uint8_t Type;
	uint8_t DataLength[2];
	uint8_t Datainfo[4];
    uint32To4Bytes Crc;
    uint8_t Eof;
}defHeaderPacket;

typedef struct{
	uint8_t Sof;
	uint8_t Type;
	uint8_t DataLenght[2];
	uint8_t* Data;
    uint32To4Bytes Crc;
    uint8_t Eof;
}defDataPacket;

typedef struct{
	uint8_t Sof;
	uint8_t Type;
	uint8_t DataLength[2];
    uint8_t Command;
    uint32To4Bytes Crc;
    uint8_t Eof;
}defResponsePacket;

typedef enum{
	RESET_NORMAL = 0x00,
	RESET_OTA
}defResetCause;

typedef struct{
	defFlashSlot FlashOnGoing;
	defResetCause ResetCause;
}defFlashCfg;

static defFlashCfg	Flash_Cfg = {
	.FlashOnGoing = FLASH_SLOT_0,
	.ResetCause = RESET_NORMAL,
};
uint32_t read_file_and_store_into_flash(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t mode, defFlashSlot slot, uint8_t *signature, uint8_t *publickey, uint8_t *hashvalue);
HAL_StatusTypeDef tree_hash_on_flash(defFlashSlot slot, uint8_t* md, uint32_t size, uint32_t *rd);
HAL_StatusTypeDef tree_hash_on_file(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t* md, uint32_t *size, uint32_t *rd);
HAL_StatusTypeDef ota_and_write_to_sd_card(FATFS *fatfs, FIL *myfile, const char *file_name, uint8_t mode);
void Frame_SendResponseFrame(defResponseType response);
defReturnType Frame_ReadStartFrame();
defReturnType Frame_ReadStopFrame();
defReturnType Frame_ReadHeaderFrame();
defReturnType Frame_ReadDataFrame(uint8_t* data);
defResponseType Frame_CheckSum(uint8_t* res_frame);
defReturnType Frame_WriteToFlash(uint8_t slot, uint8_t* data);
defReturnType Frame_STM32OTA(uint8_t slot);
defReturnType Frame_InitFlash(uint8_t slot); //call only one time.
defReturnType Frame_WriteFlashToSlot(uint8_t slot, uint8_t* data, uint16_t lenght);
uint8_t Frame_ConvertCharToUint8(uint8_t c1, uint8_t c2);
defReturnType Frame_WriteCfg(defFlashCfg *Flash_Cfg);
defReturnType Frame_ReadCfg(defFlashCfg* Cfg);
defReturnType Frame_LoadFlash(uint8_t ToSlot);

#endif 
