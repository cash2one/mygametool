#pragma once

/* If this is set to 'unsigned int' on a DEC Alpha, this gives about a
* %20 speed up (longs are 8 bytes, int's are 4). */
#ifndef DES_LONG
#define DES_LONG unsigned long
#endif

typedef unsigned char DesCBlock[8];

struct SDesKeys
{
	union	
	{
		DesCBlock _;
		/* make sure things are correct size on machines with
		* 8 byte longs */
		DES_LONG pad[2];
	} ks;
#undef _
#define _	ks._
};

typedef SDesKeys DesKeySchedule[16];
typedef const DES_LONG DesSPtrans[8][64];

int DesSetKey(DesCBlock* pKey, DesKeySchedule schedule);
void DesEncrypt(DES_LONG* pData, DesKeySchedule ks, DesSPtrans desSPtrans, int enc);
void DesEncrypt2(DES_LONG* pData, DesKeySchedule ks, DesSPtrans desSPtrans, int enc);
void DesEncrypt3(DES_LONG* pData, DesKeySchedule ks1, DesKeySchedule ks2, DesKeySchedule ks3, DesSPtrans desSPtrans);
void DesDecrypt3(DES_LONG* pData, DesKeySchedule ks1, DesKeySchedule ks2, DesKeySchedule ks3, DesSPtrans desSPtrans);
