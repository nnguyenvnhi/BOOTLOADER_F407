/***********************************************************************************
**  PROJECT      : VNPT - IoT                                                     **
**                                                                                **
**  FILENAME     : ChaCha20.cpp                                                   **
**                                                                                **
**  VERSION      : 1.0.0                                                          **
**                                                                                **
**  DATE         : 2022-05-02                                                     **
**					                                          **
**  AUTHOR       : @VNPT TEAMS       	                                          **
**                                                                                **
**  VENDOR       : @GRAPETECH				                          **
**                                                                                **
**  DESCRIPTION  : ChaCha20 Driver implementation file                            **
**                                                                                **
**  MAY BE CHANGED BY USER : NGUYEN TIEN DAT, NGUYEN VAN NHI                      **
**                                                                                **
***********************************************************************************/

/***********************************************************************************
**                                 INCLUDE SECTION			          **
***********************************************************************************/
#include <stdio.h>
#include "ChaCha20.h"

/***********************************************************************************
**                             LOCAL FUNCTION PROTOTYPE		                  **
***********************************************************************************/
FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_SetCell(uint32 cell[],
					               uint32 constant[],
	                                               const uint32 key[],
			                               uint32 blockNumber[],
			                               uint32 nonce[]);

FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_Round(uint8 keyStream[],
						     uint32 cell[]);

FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_FinalXor(uint8 keyStream[],
						    uint8 cell[],
							uint8 textInput[],
							uint16 size,
							uint8 textOutput[]);

FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_DeFinalXor(uint8 keyStream[],
						    uint8 cell[],
							uint8 cipherText[],
							uint8 plainText[]);
/***********************************************************************************
**			      GLOBAL FUNCTION MPLEMENT                            **
***********************************************************************************/
/***********************************************************************************
** Function Name         : ChaCha20_Xor
**
** Description           :
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Reentrant
**
** Input Parameters      : keyStream -
**			   cell -
**			   textInput -
**			   textOutput -
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return Parameter      : None
**
** Preconditions         : None
**
** Global Variables Used : None
**
** Functions Invoked     : ChaCha20_SetCell, ChaCha20_FinalXor
***********************************************************************************/
FUNC(void, CHACHA20_PUBLIC_FUNCTION) Chacha20_Xor(uint8 keyStream[],
						  uint32 cell[],
						  uint8 textInput[],
						  uint16 size,
						  uint8 textOutput[])
{
	// C: call setCell
	ChaCha20_SetCell(cell, Gb_sulConstant, Gb_sculChachaKey, \
	                 Gb_sulBlockNumber, Gb_sulNonce);

	ChaCha20_FinalXor((uint8*)keyStream, (uint8*)cell, \
	                  (uint8*)textInput, size, (uint8*)textOutput);
} /* END ChaCha20_Xor */




/***********************************************************************************
**                             LOCAL FUNCTION MPLEMENT  		          **
***********************************************************************************/
/***********************************************************************************
** Function Name         : ChaCha20_SetCell
**
** Description           :
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Reentrant
**
** Input Parameters      : cell -
**			   constant -
**			   key -
**			   blockNumber -
**			   nonce -
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return Parameter      : None
**
** Preconditions         : None
**
** Global Variables Used : None
**
** Functions Invoked     : None
***********************************************************************************/
FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_SetCell(uint32 cell[],
						       uint32 constant[],
						       const uint32 key[],
						       uint32 blockNumber[],
						       uint32 nonce[])
{

	// L: setConstant, i = 0 -> 3
	for (uint8 i = 0; i < 4; i++)
		// C: Assign cell[i] = constant[i]
		cell[i] = constant[i];

	// L: setkey, i = 4 -> 11
	for (uint8 i = 4; i < 12; i++)

		// C: Assign cell[i] = key[i - 4]
		cell[i] = key[i - 4];

	// L: setBlockNumber, i = 12 -> 13
	for (uint8 i = 12; i < 14; i++)

		// C: Assign cell[i] = blockNumber[i - 12]
		cell[i] = blockNumber[i - 12];

	// L: setBlockNumber, i = 14 -> 15
	for (uint8 i = 14; i < 16; i++)

		// C: Assign cell[i] = nonce[i - 12]
		cell[i] = nonce[i - 14];
}/* END ChaCha20_SetCell */



/***********************************************************************************
** Function Name         : ChaCha20_Round
**
** Description           :
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Reentrant
**
** Input Parameters      : keyStream -
**		           cell -
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return Parameter      : None
**
** Preconditions         : None
**
** Global Variables Used : None
**
** Functions Invoked     : None
***********************************************************************************/
FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_Round(uint8 keyStream[],
						     uint32 cell[])
{
	// C-DE: x-array for storing 16 cells KSG input
	uint32 x[16];

	//L: for storing 16 cells KSG input
	for (uint8 i = 0; i < 16; i++)
		x[i] = cell[i];

	//Loop for mixing in Round function-------------
	for (uint8 i = 0; i < ROUNDS; i += 2)
	{
		//Odd rounds------------------------------
		QR(x[0], x[4], x[8], x[12]);
		QR(x[1], x[5], x[9], x[13]);
		QR(x[2], x[6], x[10], x[14]);
		QR(x[3], x[7], x[11], x[15]);
		//Even rounds-------------------------
		QR(x[0], x[5], x[10], x[15]);
		QR(x[1], x[6], x[11], x[12]);
		QR(x[2], x[7], x[8], x[13]);
		QR(x[3], x[4], x[9], x[14]);
	}

	//Loop for adding KSG table input with output table of Round function
	for (uint8 i = 0, b = 0; i < 16; i++)
	{
		x[i] += cell[i];

		// Store keystream
		keyStream[b++] = x[i] & 0xFF;
		keyStream[b++] = (x[i] >> 8) & 0xFF;
		keyStream[b++] = (x[i] >> 16) & 0xFF;
		keyStream[b++] = (x[i] >> 24) & 0xFF;
	}
}/* END ChaCha20_Round */


/***********************************************************************************
** Function Name         : ChaCha20_FinalXor
**
** Description           :
**
** Sync/Async            : Synchronous
**
** Reentrancy            : Reentrant
**
** Input Parameters      : keyStream -
**						   cell -
**						   textInput -
**						   textOutput -
**
** InOut Parameters      : None
**
** Output Parameters     : None
**
** Return Parameter      : None
**
** Preconditions         : None
**
** Global Variables Used : None
**
** Functions Invoked     : ChaCha20_Round
***********************************************************************************/
FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_FinalXor(uint8 keyStream[],
							uint8 cell[],
							uint8 textInput[],
							uint16 size,
							uint8 textOutput[])
{
	// C: roundFunction
	ChaCha20_Round(keyStream, (uint32*)cell);

	// L: textInput and textOutput
	uint16 j = 0;
	uint16 round = size/64;
		   round += (size%64!=0)?1:0;
//	while (textInput[j])
//	{
	for(int16 r = 0; r < round; r++){
		// L: keyStream
		for (uint8 i = 0; i < 64&&j<size; i++)
		{
			// C: xor cell8 and textInput8
			textOutput[j] = (keyStream[i] ^ textInput[j]);
			j++;
		}
	}

//	}
}/* END ChaCha20_FinalXor */
FUNC(void, CHACHA20_PRIVATE_FUNCTION) ChaCha20_DeFinalXor(uint8 keyStream[],
						    uint8 cell[],
							uint8 cipherText[],
							uint8 plainText[])
{

}
