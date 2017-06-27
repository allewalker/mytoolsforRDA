#include "stdafx.h"
#include "AES.h"

#if KEY_COUNT == 1 
#define KEYBITS (128) //!< Use AES128. 
#elif KEY_COUNT == 2 
#define KEYBITS (192) //!< Use AES196. 
#elif KEY_COUNT == 3 
#define KEYBITS (256) //!< Use AES256. 
#else 
#error Use 1, 2 or 3 keys! 

#endif 
#if KEYBITS == 128 
#define ROUNDS (10) //!< Number of rounds. 
#define KEYLENGTH (16) //!< Key length in number of bytes. 
#elif KEYBITS == 192 
#define ROUNDS (12) //!< Number of rounds. 
#define KEYLENGTH (24) //!< // Key length in number of bytes. 
#elif KEYBITS == 256 
#define ROUNDS (14) //!< Number of rounds. 
#define KEYLENGTH (32) //!< Key length in number of bytes. 
#else 
#error Key must be 128, 192 or 256 bits! 
#endif 

#define EXPANDED_KEY_SIZE (BLOCKSIZE * (ROUNDS+1)) //!< 176, 208 or 240 bytes. 

void CalcPowLog(unsigned char *powTbl, unsigned char *logTbl)

{
	unsigned char i = 0;
	unsigned char t = 1;
	do {
		// Use 0x03 as root for exponentiation and logarithms. 
		powTbl[i] = t;
		logTbl[t] = i;
		i++;
		// Muliply t by 3 in GF(2^8). 
		t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);
	} while (t != 1); // Cyclic properties ensure that i < 255. 
	powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc. 

}

void CalcSBox(AES_CtrlStruct *AES, unsigned char * sBox)

{
	unsigned char i, rot;
	unsigned char temp;
	unsigned char Result;
	// Fill all entries of sBox[]. 
	i = 0;
	do {
		//Inverse in GF(2^8). 
		if (i > 0)
		{
			temp = AES->powTbl[255 - AES->logTbl[i]];
		}
		else
		{
			temp = 0;
		}
		// Affine transformation in GF(2).
		Result = temp ^ 0x63; // Start with adding a Vector in GF(2). 
		for (rot = 0; rot < 4; rot++)
		{
			// Rotate left. 
			temp = (temp << 1) | (temp >> 7);
			// Add rotated byte in GF(2). 
			Result ^= temp;
		}
		// Put Result in table. 
		sBox[i] = Result;
	} while (++i != 0);

}



void CalcSBoxInv(unsigned char * sBox, unsigned char * sBoxInv)

{
	unsigned char i = 0;
	unsigned char j = 0;
	// Iterate through all elements in sBoxInv using i.
	do {
		// Search through sBox using j. 
		do {
			// Check if current j is the inverse of current i. 
			if (sBox[j] == i)
			{
				// If so, set sBoxInc and indicate search finished. 
				sBoxInv[i] = j;
				j = 255;
			}
		} while (++j != 0);
	} while (++i != 0);
}

void CycleLeft(unsigned char * Row)

{

	// Cycle 4 bytes in an array left once. 
	unsigned char temp = Row[0];

	Row[0] = Row[1];
	Row[1] = Row[2];
	Row[2] = Row[3];
	Row[3] = temp;

}
void InvMixColumn(unsigned char * Column)

{

	unsigned char r0, r1, r2, r3;
	r0 = Column[1] ^ Column[2] ^ Column[3];
	r1 = Column[0] ^ Column[2] ^ Column[3];
	r2 = Column[0] ^ Column[1] ^ Column[3];
	r3 = Column[0] ^ Column[1] ^ Column[2];

	Column[0] = (Column[0] << 1) ^ (Column[0] & 0x80 ? BPOLY : 0);
	Column[1] = (Column[1] << 1) ^ (Column[1] & 0x80 ? BPOLY : 0);
	Column[2] = (Column[2] << 1) ^ (Column[2] & 0x80 ? BPOLY : 0);
	Column[3] = (Column[3] << 1) ^ (Column[3] & 0x80 ? BPOLY : 0);

	r0 ^= Column[0] ^ Column[1];
	r1 ^= Column[1] ^ Column[2];
	r2 ^= Column[2] ^ Column[3];
	r3 ^= Column[0] ^ Column[3];

	Column[0] = (Column[0] << 1) ^ (Column[0] & 0x80 ? BPOLY : 0);
	Column[1] = (Column[1] << 1) ^ (Column[1] & 0x80 ? BPOLY : 0);
	Column[2] = (Column[2] << 1) ^ (Column[2] & 0x80 ? BPOLY : 0);
	Column[3] = (Column[3] << 1) ^ (Column[3] & 0x80 ? BPOLY : 0);

	r0 ^= Column[0] ^ Column[2];
	r1 ^= Column[1] ^ Column[3];
	r2 ^= Column[0] ^ Column[2];
	r3 ^= Column[1] ^ Column[3];

	Column[0] = (Column[0] << 1) ^ (Column[0] & 0x80 ? BPOLY : 0);
	Column[1] = (Column[1] << 1) ^ (Column[1] & 0x80 ? BPOLY : 0);
	Column[2] = (Column[2] << 1) ^ (Column[2] & 0x80 ? BPOLY : 0);
	Column[3] = (Column[3] << 1) ^ (Column[3] & 0x80 ? BPOLY : 0);

	Column[0] ^= Column[1] ^ Column[2] ^ Column[3];
	r0 ^= Column[0];
	r1 ^= Column[0];
	r2 ^= Column[0];
	r3 ^= Column[0];

	Column[0] = r0;
	Column[1] = r1;
	Column[2] = r2;
	Column[3] = r3;
}

void SubBytes(AES_CtrlStruct *AES, unsigned char * bytes, unsigned char count)

{
	do {
		*bytes = AES->sBox[*bytes]; // Substitute every byte in State. 
		bytes++;
	} while (--count);

}

void InvSubBytesAndXOR(AES_CtrlStruct *AES, unsigned char * bytes, unsigned char * key, unsigned char count)

{
	do {
		// *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in State and add key.
		*bytes = AES->Block2[*bytes] ^ *key; // Use block2 directly. Increases speed. 
		bytes++;
		key++;
	} while (--count);
}

void InvShiftRows(unsigned char * State)

{
	unsigned char temp;
	// Note: State is arranged Column by Column. 
	// Cycle second Row right one time. 
	temp = State[1 + 3 * 4];
	State[1 + 3 * 4] = State[1 + 2 * 4];
	State[1 + 2 * 4] = State[1 + 1 * 4];
	State[1 + 1 * 4] = State[1 + 0 * 4];
	State[1 + 0 * 4] = temp;
	// Cycle third Row right two times. 
	temp = State[2 + 0 * 4];
	State[2 + 0 * 4] = State[2 + 2 * 4];
	State[2 + 2 * 4] = temp;
	temp = State[2 + 1 * 4];
	State[2 + 1 * 4] = State[2 + 3 * 4];
	State[2 + 3 * 4] = temp;
	// Cycle fourth Row right three times, ie. left once. 
	temp = State[3 + 0 * 4];
	State[3 + 0 * 4] = State[3 + 1 * 4];
	State[3 + 1 * 4] = State[3 + 2 * 4];
	State[3 + 2 * 4] = State[3 + 3 * 4];
	State[3 + 3 * 4] = temp;
}



void InvMixColumns(unsigned char * State)

{
	InvMixColumn(State + 0 * 4);
	InvMixColumn(State + 1 * 4);
	InvMixColumn(State + 2 * 4);
	InvMixColumn(State + 3 * 4);
}



void XORBytes(unsigned char * bytes1, unsigned char * bytes2, unsigned char count)

{
	do {
		*bytes1 ^= *bytes2; // Add in GF(2), ie. XOR. 
		bytes1++;
		bytes2++;
	} while (--count);
}

void KeyExpansion(AES_CtrlStruct *AES, unsigned char * ExpandedKey)

{
	unsigned char temp[4];
	unsigned char i;
	unsigned char Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant. 
	unsigned char * key = AES->AESKey;
	// Copy key to start of expanded key. 
	i = KEYLENGTH;
	do {
		*ExpandedKey = *key;
		ExpandedKey++;
		key++;
	} while (--i);


	// Prepare last 4 bytes of key in temp. 
	ExpandedKey -= 4;
	temp[0] = *(ExpandedKey++);
	temp[1] = *(ExpandedKey++);
	temp[2] = *(ExpandedKey++);
	temp[3] = *(ExpandedKey++);
	// Expand key. 
	i = KEYLENGTH;
	while (i < BLOCKSIZE*(ROUNDS + 1))
	{
		// Are we at the start of a multiple of the key size?
		if ((i % KEYLENGTH) == 0)
		{
			CycleLeft(temp); // Cycle left once. 
			SubBytes(AES, temp, 4); // Substitute each byte. 
			XORBytes(temp, Rcon, 4); // Add constant in GF(2). 
			*Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);
		}
		// Keysize larger than 24 bytes, ie. larger that 192 bits? 
#if KEYLENGTH > 24 

		// Are we right past a block size? 
		else if ((i % KEYLENGTH) == BLOCKSIZE) {
			SubBytes(temp, 4); // Substitute each byte. 
		}
#endif 
		// Add bytes in GF(2) one KEYLENGTH away. 
		XORBytes(temp, ExpandedKey - KEYLENGTH, 4);
		// Copy Result to current 4 bytes. 
		*(ExpandedKey++) = temp[0];
		*(ExpandedKey++) = temp[1];
		*(ExpandedKey++) = temp[2];
		*(ExpandedKey++) = temp[3];
		i += 4; // Next 4 bytes. 
	}

}

void InvCipher(AES_CtrlStruct *AES, unsigned char * block, unsigned char * ExpandedKey)

{
	unsigned char round = ROUNDS - 1;
	ExpandedKey += BLOCKSIZE * ROUNDS;
	XORBytes(block, ExpandedKey, 16);
	ExpandedKey -= BLOCKSIZE;
	do {
		InvShiftRows(block);
		InvSubBytesAndXOR(AES, block, ExpandedKey, 16);
		ExpandedKey -= BLOCKSIZE;
		InvMixColumns(block);
	} while (--round);
	InvShiftRows(block);
	InvSubBytesAndXOR(AES, block, ExpandedKey, 16);
}



void AES_DecInit(AES_CtrlStruct *AES)

{
	AES->powTbl = AES->Block1;
	AES->logTbl = AES->Block2;
	CalcPowLog(AES->powTbl, AES->logTbl);
	AES->sBox = AES->Block3;
	CalcSBox(AES, AES->sBox);
	AES->ExpandedKey = AES->Block1;
	KeyExpansion(AES, AES->ExpandedKey);
	AES->sBoxInv = AES->Block2; // Must be block2. 
	CalcSBoxInv(AES->sBox, AES->sBoxInv);

}


void AES_Decrypt(AES_CtrlStruct *AES, unsigned char * buffer, unsigned char * chainBlock)

{
	unsigned char temp[BLOCKSIZE];
	memcpy(temp, buffer, BLOCKSIZE);
	InvCipher(AES, buffer, AES->ExpandedKey);
	XORBytes(buffer, chainBlock, BLOCKSIZE);
	memcpy(chainBlock, buffer, BLOCKSIZE);
}

unsigned char Multiply(unsigned char num, unsigned char factor)

{
	unsigned char mask = 1;
	unsigned char Result = 0;
	while (mask != 0)
	{
		// Check bit of factor given by mask. 
		if (mask & factor)
		{
			// Add current multiple of num in GF(2). 
			Result ^= num;
		}
		// Shift mask to indicate next bit. 
		mask <<= 1;
		// Double num. 
		num = (num << 1) ^ (num & 0x80 ? BPOLY : 0);
	}
	return Result;

}

unsigned char DotProduct(unsigned char * Vector1, unsigned char * Vector2)

{
	unsigned char Result = 0;
	Result ^= Multiply(*Vector1++, *Vector2++);
	Result ^= Multiply(*Vector1++, *Vector2++);
	Result ^= Multiply(*Vector1++, *Vector2++);
	Result ^= Multiply(*Vector1, *Vector2);
	return Result;

}



void MixColumn(unsigned char * Column)

{

	unsigned char Row[8] = { 0x02, 0x03, 0x01, 0x01, 0x02, 0x03, 0x01, 0x01 };
	// Prepare first Row of matrix twice, to eliminate need for cycling. 
	unsigned char Result[4];
	// Take dot products of each matrix Row and the Column Vector. 
	Result[0] = DotProduct(Row + 0, Column);
	Result[1] = DotProduct(Row + 3, Column);
	Result[2] = DotProduct(Row + 2, Column);
	Result[3] = DotProduct(Row + 1, Column);
	// Copy temporary Result to original Column. 
	Column[0] = Result[0];
	Column[1] = Result[1];
	Column[2] = Result[2];
	Column[3] = Result[3];
}



void MixColumns(unsigned char * State)

{
	MixColumn(State + 0 * 4);
	MixColumn(State + 1 * 4);
	MixColumn(State + 2 * 4);
	MixColumn(State + 3 * 4);
}



void ShiftRows(unsigned char * State)
{
	unsigned char temp;
	// Note: State is arranged Column by Column. 
	// Cycle second Row left one time. 
	temp = State[1 + 0 * 4];
	State[1 + 0 * 4] = State[1 + 1 * 4];
	State[1 + 1 * 4] = State[1 + 2 * 4];
	State[1 + 2 * 4] = State[1 + 3 * 4];
	State[1 + 3 * 4] = temp;
	// Cycle third Row left two times. 
	temp = State[2 + 0 * 4];
	State[2 + 0 * 4] = State[2 + 2 * 4];
	State[2 + 2 * 4] = temp;
	temp = State[2 + 1 * 4];
	State[2 + 1 * 4] = State[2 + 3 * 4];
	State[2 + 3 * 4] = temp;
	// Cycle fourth Row left three times, ie. right once. 
	temp = State[3 + 3 * 4];
	State[3 + 3 * 4] = State[3 + 2 * 4];
	State[3 + 2 * 4] = State[3 + 1 * 4];
	State[3 + 1 * 4] = State[3 + 0 * 4];
	State[3 + 0 * 4] = temp;
}

void Cipher(AES_CtrlStruct *AES, unsigned char * block, unsigned char * ExpandedKey)
{
	unsigned char round = ROUNDS - 1;
	XORBytes(block, ExpandedKey, 16);
	ExpandedKey += BLOCKSIZE;
	do {
		SubBytes(AES, block, 16);
		ShiftRows(block);
		MixColumns(block);
		XORBytes(block, ExpandedKey, 16);
		ExpandedKey += BLOCKSIZE;
	} while (--round);

	SubBytes(AES, block, 16);
	ShiftRows(block);
	XORBytes(block, ExpandedKey, 16);
}



void AES_EncInit(AES_CtrlStruct *AES)

{
	AES->powTbl = AES->Block1;
	AES->logTbl = AES->Block3;
	CalcPowLog(AES->powTbl, AES->logTbl);
	AES->sBox = AES->Block2;
	CalcSBox(AES, AES->sBox);
	AES->ExpandedKey = AES->Block1;
	KeyExpansion(AES, AES->ExpandedKey);
}


void AES_Encrypt(AES_CtrlStruct *AES, unsigned char * buffer, unsigned char * chainBlock)

{
	XORBytes(buffer, chainBlock, BLOCKSIZE);
	Cipher(AES, buffer, AES->ExpandedKey);
	memcpy(chainBlock, buffer, BLOCKSIZE);
}
