#include "stdafx.h"
#include "TEA.h"
#define DELTA (0x9e3779b9)
void TEA_Encrypt(u32 *Data, u32 *Key, u32 Rounds, u8 Type)
{
	u32 Sum, y, z, p, e;
	Sum = 0;
	switch (Type)
	{
	case TEA_TYPE_ORG:
		
		for (p = 0; p < Rounds; p++)
		{                       /* basic cycle start */
			Sum += DELTA;
			Data[0] += ((Data[1] << 4) + Key[0]) ^ (Data[1] + Sum) ^ ((Data[1] >> 5) + Key[1]);
			Data[1] += ((Data[0] << 4) + Key[2]) ^ (Data[0] + Sum) ^ ((Data[0] >> 5) + Key[3]);
		}
		break;
	case TEA_TYPE_X:
		for (p = 0; p < Rounds; p++)
		{
			Data[0] += (((Data[1] << 4) ^ (Data[1] >> 5)) + Data[1]) ^ (Sum + Key[Sum & 3]);
			Sum += DELTA;
			Data[1] += (((Data[0] << 4) ^ (Data[0] >> 5)) + Data[0]) ^ (Sum + Key[(Sum >> 11) & 3]);
		}
		break;
	case TEA_TYPE_XX:
		z = Data[1];
		do
		{
			Sum += DELTA;
			e = (Sum >> 2) & 3;
			p = 0;
			y = Data[1];
			z = Data[0] += (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((Sum^y) + (Key[(p & 3) ^ e] ^ z)));
			p = 1;
			y = Data[0];
			z = Data[1] += (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((Sum^y) + (Key[(p & 3) ^ e] ^ z)));
		} while (--Rounds);
		break;
	}
}

void TEA_Decrypt(uint32_t *Data, uint32_t *Key, uint32_t Rounds, uint8_t Type)
{
	u32 Sum, y, z, p, e;
	Sum = Rounds * DELTA;
	switch (Type)
	{
	case TEA_TYPE_ORG:
		
		for (p = 0; p < Rounds; p++)
		{                         /* basic cycle start */
			Data[1] -= ((Data[0] << 4) + Key[2]) ^ (Data[0] + Sum) ^ ((Data[0] >> 5) + Key[3]);
			Data[0] -= ((Data[1] << 4) + Key[0]) ^ (Data[1] + Sum) ^ ((Data[1] >> 5) + Key[1]);
			Sum -= DELTA;
		}
		break;
	case TEA_TYPE_X:

		for (p = 0; p < Rounds; p++)
		{
			Data[1] -= (((Data[0] << 4) ^ (Data[0] >> 5)) + Data[0]) ^ (Sum + Key[(Sum >> 11) & 3]);
			Sum -= DELTA;
			Data[0] -= (((Data[1] << 4) ^ (Data[1] >> 5)) + Data[1]) ^ (Sum + Key[Sum & 3]);
		}

		break;
	case TEA_TYPE_XX:
		y = Data[0];
		do
		{
			e = (Sum >> 2) & 3;
			p = 1;
			z = Data[0];
			y = Data[1] -= (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((Sum^y) + (Key[(p & 3) ^ e] ^ z)));
			p = 0;
			z = Data[1];
			y = Data[0] -= (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((Sum^y) + (Key[(p & 3) ^ e] ^ z)));
			Sum -= DELTA;
		} while (--Rounds);
		break;
	}
}