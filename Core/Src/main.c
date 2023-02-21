/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "ed25519.h"
#include "sha3.h"
#include "ge.h"
#include "sc.h"
#include "Frame.h"
#include "My_SDCard.h"
#include "Ex_Flash.h"
#include "W25Qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SD_HandleTypeDef hsd;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Update_Signature(Certificate *cert, uint8_t* signature){
	for(uint8_t index = 0; index < 64; index++){
		cert->signature[index] = signature[index];
	}
}

void Update_Publickey(Certificate *cert, uint8_t* publickey){
	for(uint8_t index = 0;index < 32; index++){
		cert->publickey[index] = publickey[index];
	}
}

void Update_HashValue(Certificate *cert, uint8_t* hashvalue){
	for(uint8_t index = 0;index < 32; index++){
			cert->hash[index] = hashvalue[index];
	}
}

void Update_BootloaderCert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue){
	Update_Signature(&Bootloader_Cert, signature);
	Update_Publickey(&Bootloader_Cert, publickey);
	Update_HashValue(&Bootloader_Cert, hashvalue);
}

void Update_Application1Cert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue){
	Update_Signature(&Application1_Cert, signature);
	Update_Publickey(&Application1_Cert, publickey);
	Update_HashValue(&Application1_Cert, hashvalue);
}

void Update_Application2Cert(uint8_t* signature, uint8_t* publickey, uint8_t* hashvalue){
	Update_Signature(&Application2_Cert, signature);
	Update_Publickey(&Application2_Cert, publickey);
	Update_HashValue(&Application2_Cert, hashvalue);
}



static void goto_application(uint32_t slotadd)
{
//  printf("Gonna Jump to Application\r\n");

  void (*app_reset_handler)(void) = (void*)(*((volatile uint32_t*) (slotadd + 4U)));

//  __set_MSP(*(volatile uint32_t*) 0x08020000);

  // Turn OFF the Green Led to tell the user that Bootloader is not running
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET );    //Green LED OFF
  app_reset_handler();    //call the app reset handler
}

uint8_t rx_buf[3] = {0,};
uint8_t count = 0;
uint8_t flag  = 0;
uint8_t rx = 0;
uint8_t rx_header[15] = {0,};
uint8_t buftest[] = "NGUYEN VAN NHI";
uint32_t crc = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == huart6.Instance){
			if(!strncmp("OTA",(char*)rx_buf, 3)){
				flag = 1;
			}
			else{
				HAL_UART_Receive_IT(&huart6, rx_buf, 3);
			}
			memset(rx_buf, 0, 3);
	}
}

uint8_t md[64] = {0,};
uint32_t size = 0;
uint32_t round = 0;
uint8_t signature[64], publickey[32], hashvalue[64];

//uint8_t valid_signature_verify_1 = 0;
//uint8_t valid_signature_verify_2 = 0;
//uint8_t valid_signature_verify_3 = 0;
//uint8_t key_exchange_correct = 0;
//uint16_t clock_create_seed = 0;
//uint16_t clock_create_keypair = 0;
//uint16_t clock_sign = 0;
//uint16_t clock_verify = 0;
//uint16_t clock_add_keypair_scalar = 0;
//uint16_t clock_add_pubkey_scalar = 0;
//uint16_t clock_key_exchange = 0;
//int i;
uint8_t read1[15] = {0,};
uint8_t read2[15] = {0,};
uint8_t read3[15] = {0,};
uint8_t read4[15] = {0,};
uint8_t read5[15] = {0,};
uint8_t buffer1[160] = {0,};
uint8_t buffer2[160] = {0,};
uint8_t buffer3[160] = {0,};
//uint8_t buffer[100] = {0,};
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart6, rx_buf, 3);
  HAL_TIM_Base_Start(&htim1);
  uint16_t time = 0;
  uint32_t total_time = 0;

//  Frame_WriteCfg(&Flash_Cfg);
//  goto_application(FLASH_SLOT_1_ADD);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
//  HAL_Delay(500);
//  goto_application(FLASH_SLOT_1_ADD);
  FATFS fatfs;
  FIL myfile;
  uint8_t buffer[100] = {0,};
//  const char *file_name = "APPLICATION_F407.bin";
//  uint8_t mode = FA_READ;
//  defFlashSlot slot = FLASH_SLOT_0;
//
//
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);

//  goto_application(FLASH_SLOT_0_ADD);
//  tree_hash(&fatfs,&myfile,"abc.bin", md, &size, &round);
//  sha3(file_name,strlen(file_name), md, 64);

//  uint8_t *write1 = "NGUYEN VAN NHI";
//  uint8_t *write2 = "TRAN TAN TAI";
//  uint8_t *write3 = "TRAN TAN TUAN";
//  uint8_t *write4 = "PHAN TRUNG TIN";
//  uint8_t *write5 = "DUNG MINH TAM";
//
//  Erase(0x00000, (Erase_Type)SECTOR_ERASE);
//  HAL_Delay(10);
//
//  WriteFlash(0x000000, write1, strlen(write1));
//  WriteFlash(0x000000 + strlen(write1), write2, strlen(write2));
//  WriteFlash(0x000000 + strlen(write1) + strlen(write2), write3, strlen(write3));
//  WriteFlash(0x000000 + strlen(write1) + strlen(write2) + strlen(write3), write4, strlen(write4));
//  WriteFlash(0x000000 + strlen(write1) + strlen(write2) + strlen(write3) + strlen(write4), write5, strlen(write5));
//
//  HAL_Delay(1000);
//  ReadFlash(0x000000, read1, strlen(write1));
//  ReadFlash(0x000000 + strlen(write1), read2, strlen(write2));
//  ReadFlash(0x000000 + strlen(write1) + strlen(write2), read3, strlen(write3));
//  ReadFlash(0x000000 + strlen(write1) + strlen(write2) + strlen(write3), read4, strlen(write4));
//  ReadFlash(0x000000 + strlen(write1) + strlen(write2) + strlen(write3) + strlen(write4), read5, strlen(write5));
//  	  for(uint8_t i = 0;i<64;i++){
//  		  Bootloader_Cert.signature[i] = i + 1;
//  		  Application1_Cert.signature[i] = i + 2;
//  		  Application2_Cert.signature[i] = i + 3;
//  		  Bootloader_Cert.hash[i] = i + 4;
//  		  Application1_Cert.hash[i] = i + 5;
//  		  Application2_Cert.hash[i] = i + 6;
//  	  }
//  	  for (uint8_t i = 0;i<32;i++){
//  		  Bootloader_Cert.publickey[i] = i + 7;
//  		  Application1_Cert.publickey[i] = i + 8;
//  		  Application2_Cert.publickey[i] = i + 9;
//  	  }
//  		W25qxx_Init();
//  		W25qxx_EraseSector(1);
//  		W25qxx_WriteSector(Bootloader_Cert.signature, 1, 0, 64);
//  		W25qxx_WriteSector(Bootloader_Cert.publickey, 1, 64, 32);
//  		W25qxx_WriteSector(Bootloader_Cert.hash, 1, 96, 64);
//  		W25qxx_ReadSector(buffer1, 1, 0, 160);
//  		W25qxx_EraseSector(1);
//  		memset(buff2, 0, sizeof(buff2));
//  		W25qxx_ReadSector(buff2, 1, 0, 8);

//  uint8_t sig[64], pub[32], has[64];
//  W25qxx_Init();
//    	  for(uint8_t i = 0;i<64;i++){
//    		  sig[i] = i + 1;
//    		  has[i] = i + 2;
//
//    	  }
//    	  for (uint8_t i = 0;i<32;i++){
//    		  pub[i] = i + 3;
//    	  }
//
//    	  Update_BootloaderCert(sig, pub, has);
//  W25qxx_EraseSector(1);
//  W25qxx_WriteSector(Bootloader_Cert.signature, 1, 0, 64);
//  W25qxx_WriteSector(Bootloader_Cert.publickey, 1, 64, 32);
//  W25qxx_WriteSector(Bootloader_Cert.hash, 1, 96, 64);
//
//  W25qxx_ReadSector(buffer, 1, 0, 160);
  uint8_t sig[64], pub[32], hashval[64];

//  W25qxx_Init();
//  W25qxx_EraseSector(2);
//  read_file_and_store_into_flash(&fatfs,&myfile,"appa.bin", FA_READ, FLASH_SLOT_1, signature, publickey, hashvalue);
//  W25qxx_WriteSector(signature, 2, 0, 64);
//  W25qxx_WriteSector(publickey, 2, 64, 32);
//  W25qxx_WriteSector(hashvalue, 2, 64 + 32, 64);
//  HAL_Delay(1000);
//  W25qxx_ReadSector(signature, 2, 0, 64);
//  W25qxx_ReadSector(publickey, 2, 64, 32);
//  W25qxx_ReadSector(hashvalue, 2, 96, 64);
//  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
////	  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
//	  if((defResetCause)Flash_Cfg.ResetCause == RESET_NORMAL){
////		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
//		  if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_0) goto_application(FLASH_SLOT_0_ADD);
//		  else if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_1) goto_application(FLASH_SLOT_1_ADD);
//		  else{
//
//		  }
//	  }
//	  else if((defResetCause)Flash_Cfg.ResetCause == RESET_OTA){
//		  if(flag == 1){
//			  {
//				  Flash_Cfg.ResetCause = RESET_NORMAL;
//				  if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_0){
//					  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
//					  if(Frame_STM32OTA(FLASH_SLOT_2) == HANDLE_OK) {
//						  Flash_Cfg.FlashOnGoing = FLASH_SLOT_1;
//						  Frame_WriteCfg(&Flash_Cfg);
//						  Frame_LoadFlash(FLASH_SLOT_1);
//						  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12|GPIO_PIN_15);
////						  HAL_Delay(500);
//						  goto_application(FLASH_SLOT_1_ADD);
//					  }
//					  else{
//						  Frame_WriteCfg(&Flash_Cfg);
//						  goto_application(FLASH_SLOT_0_ADD);
//					  }
//				  }
//				  else if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_1){
//					  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
//					  if(Frame_STM32OTA(FLASH_SLOT_2) == HANDLE_OK) {
//						  Flash_Cfg.FlashOnGoing = FLASH_SLOT_0;
//						  Frame_WriteCfg(&Flash_Cfg);
//						  Frame_LoadFlash(FLASH_SLOT_0);
//						  HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12|GPIO_PIN_15);
//						  goto_application(FLASH_SLOT_0_ADD);
//					  }
//					  else{
//						  Frame_WriteCfg(&Flash_Cfg);
//						  goto_application(FLASH_SLOT_1_ADD);
//					  }
//				  }
//				flag = 0;
//			  }
//		  }
//	  }
//	  else{
//		  goto_application(FLASH_SLOT_0_ADD);
//	  }

//
	  HAL_UART_Transmit(&huart2, "\r*****THIS IS BOOTLOADER PROGRAM*****\n\r", 39, 1000);
	  Frame_ReadCfg(&Flash_Cfg);
	  if((defResetCause)Flash_Cfg.ResetCause == RESET_NORMAL){
		  if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_1){
			  W25qxx_Init();
			  W25qxx_ReadSector(sig, 2, 0, 64);
			  W25qxx_ReadSector(pub, 2, 64, 32);
			  W25qxx_ReadSector(hashval, 2, 64 + 32, 64);
			  int size_file = read_file_and_store_into_flash(&fatfs,&myfile,"appa.bin", FA_READ, FLASH_SLOT_1, signature, publickey, hashvalue);
			  tree_hash_on_flash(FLASH_SLOT_1, md,size_file, &round);
			  if (ed25519_verify(sig, md, 64, pub)) {
				  HAL_UART_Transmit(&huart2, "\r*****APPLICATION1 IS RUNNING*****\n\r", 39, 1000);
				  goto_application(FLASH_SLOT_1_ADD);
			  }
		  }

		  if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_2){
			  W25qxx_Init();
			  W25qxx_ReadSector(sig, 3, 0, 64);
			  W25qxx_ReadSector(pub, 3, 64, 32);
			  W25qxx_ReadSector(hashval, 3, 64 + 32, 64);
			  int size_file = read_file_and_store_into_flash(&fatfs,&myfile,"appb.bin", FA_READ, FLASH_SLOT_2, signature, publickey, hashvalue);
			  tree_hash_on_flash(FLASH_SLOT_2, md,size_file, &round);
			  if (ed25519_verify(sig, md, 64, pub)) {
				  HAL_UART_Transmit(&huart2, "\r*****APPLICATION2 IS RUNNING*****\n\r", 39, 1000);
				  goto_application(FLASH_SLOT_2_ADD);
			  }
		  }

	  }
	  else if((defResetCause)Flash_Cfg.ResetCause == RESET_OTA){
		  while(flag == 0);
		  if(flag == 1){
			  {
				  HAL_UART_Transmit(&huart2, "\r       UPDATING OTA...\n\r", 25, 1000);
				  Flash_Cfg.ResetCause = RESET_NORMAL;
				  if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_1){
					  TIM1->CNT = 0;
					  if(ota_and_write_to_sd_card(&fatfs, &myfile, "appb.bin", FA_WRITE|FA_CREATE_ALWAYS) == HAL_OK) {
						  time = TIM1->CNT;
						  total_time += time;
						  HAL_UART_Transmit(&huart2, "\r       OTA SUCCESSFULL !!!\n\r", 29, 1000);
						  sprintf(buffer,"\r            -TIME: %.2f\n\r", (float)time/5.0);
						  HAL_UART_Transmit(&huart2, buffer, 25, 1000);
						  memset(buffer, 0, 100);

						  TIM1->CNT = 0;
						  int size_file = read_file_and_store_into_flash(&fatfs,&myfile,"appb.bin", FA_READ, FLASH_SLOT_2, signature, publickey, hashvalue);
						  time = TIM1->CNT;
						  total_time += time;
						  HAL_UART_Transmit(&huart2, "\r       -READ APPLICATION2 AND STORE IN FLASH\n\r", 50, 1000);
						  sprintf(buffer, "\r              +SIZE: %d BYTES\n\r              +TIME: %.2f(MS)\n\r", size_file, (float)time/5.0);
						  HAL_UART_Transmit(&huart2, buffer, 100, 1000);
						  memset(buffer, 0, 100);

						  TIM1->CNT = 0;
						  tree_hash_on_flash(FLASH_SLOT_2, md,size_file, &round);
						  time = TIM1->CNT;
						  total_time += time;
						  HAL_UART_Transmit(&huart2, "\r       -TREE HASH IN FLASH\n\r", 28, 1000);
						  sprintf(buffer, "\r              +TIME: %.2f(MS)\n\r", (float)time/5.0);
						  HAL_UART_Transmit(&huart2, buffer, 40, 1000);
						  memset(buffer, 0, 100);

						  TIM1->CNT = 0;
						  if (ed25519_verify(signature, md, 64, publickey)) {

							  W25qxx_Init();
							  W25qxx_EraseSector(3);
							  Update_Application2Cert(signature, publickey, hashvalue);
							  W25qxx_WriteSector(Application2_Cert.signature, 3, 0, 64);
							  W25qxx_WriteSector(Application2_Cert.publickey, 3, 0 + 64, 32);
							  W25qxx_WriteSector(Application2_Cert.hash, 3, 0 + 32 + 64, 64);


							  time = TIM1->CNT;
							  total_time += time;
							  HAL_UART_Transmit(&huart2, "\r       -VERIFY APPLICATION2 SUCCESSFULL\n\r", 60, 1000);
							  sprintf(buffer, "\r              +TIME: %.2f(MS)\n\r", (float)time/5.0);
							  HAL_UART_Transmit(&huart2, buffer, 40, 1000);
							  sprintf(buffer, "\r       -TOTAL TIME: %.2f(MS)\n\r", (float)total_time/5.0);
							  HAL_UART_Transmit(&huart2, buffer, 40, 1000);

							  HAL_UART_Transmit(&huart2, "\r*****APPLICATION2 IS RUNNING*****\n\r", 50, 1000);

							  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
							  Flash_Cfg.FlashOnGoing = FLASH_SLOT_2;
							  Frame_WriteCfg(&Flash_Cfg);
							  goto_application(FLASH_SLOT_2_ADD);
						  } else {
							  Frame_WriteCfg(&Flash_Cfg);
							  goto_application(FLASH_SLOT_1_ADD);
						  }

					  }
					  else{
						  Frame_WriteCfg(&Flash_Cfg);
						  goto_application(FLASH_SLOT_1_ADD);
					  }
				  }
				  else if(Flash_Cfg.FlashOnGoing == FLASH_SLOT_2){
					  TIM1->CNT = 0;
					  if(ota_and_write_to_sd_card(&fatfs, &myfile, "appa.bin", FA_WRITE|FA_CREATE_ALWAYS) == HAL_OK) {
						  time = TIM1->CNT;
						  total_time += time;
						  HAL_UART_Transmit(&huart2, "\r       OTA SUCCESSFULL !!!\n\r", 29, 1000);
						  sprintf(buffer,"\r            -TIME: %.2f\n\r", (float)time/5.0);
						  HAL_UART_Transmit(&huart2, buffer, 30, 1000);
						  memset(buffer, 0, 100);

						  TIM1->CNT = 0;
						  int size_file = read_file_and_store_into_flash(&fatfs,&myfile,"appa.bin", FA_READ, FLASH_SLOT_1, signature, publickey, hashvalue);
						  time = TIM1->CNT;
						  total_time += time;
						  HAL_UART_Transmit(&huart2, "\r       -READ APPLICATION1 AND STORE IN FLASH\n\r", 50, 1000);
						  sprintf(buffer, "\r              +SIZE: %d BYTES\n\r              +TIME: %.2f(MS)\n\r", size_file, (float)time/5.0);
						  HAL_UART_Transmit(&huart2, buffer, 100, 1000);
						  memset(buffer, 0, 100);


						  TIM1->CNT = 0;
						  tree_hash_on_flash(FLASH_SLOT_1, md,size_file, &round);
						  time = TIM1->CNT;
						  total_time += time;
						  HAL_UART_Transmit(&huart2, "\r       -TREE HASH IN FLASH\n\r", 28, 1000);
						  sprintf(buffer, "\r              +TIME: %.2f(MS)\n\r", (float)time/5.0);
						  HAL_UART_Transmit(&huart2, buffer, 40, 1000);
						  memset(buffer, 0, 100);

						  TIM1->CNT = 0;
						  if (ed25519_verify(signature, md, 64, publickey)) {

							  W25qxx_Init();
							  W25qxx_EraseSector(2);
							  Update_Application1Cert(signature, publickey, hashvalue);
							  W25qxx_WriteSector(Application1_Cert.signature, 2, 0, 64);
							  W25qxx_WriteSector(Application1_Cert.publickey, 2, 0 + 64, 32);
							  W25qxx_WriteSector(Application1_Cert.hash, 2, 0 + 32 + 64, 64);

							  time = TIM1->CNT;
							  total_time += time;
							  HAL_UART_Transmit(&huart2, "\r       -VERIFY APPLICATION2 SUCCESSFULL\n\r", 60, 1000);
							  sprintf(buffer, "\r              +TIME: %.2f(MS)\n\r", (float)time/5.0);
							  HAL_UART_Transmit(&huart2, buffer, 40, 1000);
							  sprintf(buffer, "\r       -TOTAL TIME: %.2f(MS)\n\r", (float)total_time/5.0);
							  HAL_UART_Transmit(&huart2, buffer, 40, 1000);

							  HAL_UART_Transmit(&huart2, "\r*****APPLICATION1 IS RUNNING*****\n\r", 50, 1000);


							  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
							  Flash_Cfg.FlashOnGoing = FLASH_SLOT_1;
							  Frame_WriteCfg(&Flash_Cfg);
							  goto_application(FLASH_SLOT_1_ADD);
						  } else {
							  Frame_WriteCfg(&Flash_Cfg);
							  goto_application(FLASH_SLOT_2_ADD);
						  }
					  }
					  else{
						  Frame_WriteCfg(&Flash_Cfg);
						  goto_application(FLASH_SLOT_2_ADD);
					  }
				  }
				flag = 0;
			  }
		  }
	  }
	  else{
		  goto_application(FLASH_SLOT_1_ADD);
	  }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 2;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 33600-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE14 PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

