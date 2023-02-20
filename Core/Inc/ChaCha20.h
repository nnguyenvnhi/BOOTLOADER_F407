/***********************************************************************************
**  PROJECT      : VNPT - IoT                                                     **
**                                                                                **
**  FILENAME     : ChaCha20.h                                                     **
**                                                                                **
**  VERSION      : 1.0.0                                                          **
**                                                                                **
**  DATE         : 2022-05-02                                                     **
**									          **
**  AUTHOR       : @VNPT TEAMS       	                                          **
**                                                                                **
**  VENDOR       : @GRAPETECH				                          **
**                                                                                **
**  DESCRIPTION  : ChaCha20 Driver header definition file                         **
**                                                                                **
**  MAY BE CHANGED BY USER : TIEN DAT NGUYEN, NGUYEN VAN NHI                      **
**                                                                                **
***********************************************************************************/
#ifndef _ChaCha20_
#define _ChaCha20_


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************
**                                 INCLUDE SECTION			          **
***********************************************************************************/
#include <string.h>
#include "Compiler.h"
#include "Std_Types.h"


/***********************************************************************************
**                                  DEFINE SECTION			          **
***********************************************************************************/
/* Number of Rounds for Round Function */
#define ROUNDS 20

/* Quarter Round function */
#define QR(a, b, c, d) (\
			a += b,  d ^= a,  d = ROTL(d,16),\
			c += d,  b ^= c,  b = ROTL(b,12),\
			a += b,  d ^= a,  d = ROTL(d, 8),\
			c += d,  b ^= c,  b = ROTL(b, 7) )

/* Rotate left function */
#define ROTL(a,b) (((a) << (b)) | ((a) >> (32 - (b))))

/* keySize, constantSize, nonceSize,	blockSize, keyStreamSize, cellSize */
#define keySize 8
#define constantSize 4
#define nonceSize 2
#define blockNumberSize 2
#define keyStreamSize 64
#define cellSize 16


/***********************************************************************************
**                                  GLOBAL DATA                                   **
***********************************************************************************/

VAR(static uint32, CHACHA20_GLOBAL_VAR) Gb_sulConstant[constantSize] =
				{ 0xCDD73CF5, 0xC1ADC3FD, 0x1A3CCFE5, 0x20434F4E };

VAR(static const uint32, CHACHA20_GLOBAL_VAR) Gb_sculChachaKey[keySize] =
				{ 0x564E5054, 0x20506861, 0x73652033, 0x20536563,
				0x72657420, 0x6B657920, 0x62792054, 0x68616E68 };

VAR(static uint32, CHACHA20_GLOBAL_VAR) Gb_sulBlockNumber[blockNumberSize] =
							{ 0x247E48ED, 0x0537E91E };

VAR(static uint32, CHACHA20_GLOBAL_VAR) Gb_sulNonce[nonceSize] =
							{ 0xC26D2B4F, 0x8AF1CC40 };

VAR(static uint32, CHACHA20_GLOBAL_VAR) Gb_sulCell[cellSize] = {0, };

VAR(static uint8, CHACHA20_GLOBAL_VAR) Gb_scKeyStream[keyStreamSize] =
            			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            			 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


/***********************************************************************************
**                            GLOBAL FUNCTION PROTOTYPE			          **
***********************************************************************************/
FUNC(void, CHACHA20_PUBLIC_FUNCTION) Chacha20_Xor(uint8 keyStream[],
						  uint32 cell[],
						  uint8 textInput[],
						  uint16 size,
						  uint8 textOutput[]);

#ifdef __cplusplus
}
#endif

#endif // !_ChaCha20_
