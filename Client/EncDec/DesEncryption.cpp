//
#include "DesEncryption.h"

//
#include "MiniDes.h"
#include "Md5/Md5.h"
#include "spr.h"

//
#include <cassert>
#include <cstring>


namespace bin
{

void DesEncryptData(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2)
{
	// Must be times of 8
	assert((nSize&0x7) == 0);

	DesKeySchedule desKey0;
	DesKeySchedule desKey1;
	DesKeySchedule desKey2;

    DesSetKey((DesCBlock*)key0, desKey0);
	DesSetKey((DesCBlock*)key1, desKey1);
	DesSetKey((DesCBlock*)key2, desKey2);

	byte* pData = pInput;
	for(int i=0; i<nSize; i+=8)
	{
		DesEncrypt3((unsigned long*)pData, desKey0, desKey1, desKey2, s_dataDesSPtrans);

		pData += 8;
	}
}

void DesDecryptData(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2)
{
	// Must be times of 8
	assert((nSize&0x7) == 0);

	DesKeySchedule desKey0;
	DesKeySchedule desKey1;
	DesKeySchedule desKey2;

    DesSetKey((DesCBlock*)key0, desKey0);
	DesSetKey((DesCBlock*)key1, desKey1);
	DesSetKey((DesCBlock*)key2, desKey2);

	byte* pData = pInput;
	for(int i=0; i<nSize; i+=8)
	{
		DesDecrypt3((unsigned long*)pData, desKey0, desKey1, desKey2, s_dataDesSPtrans);

		pData += 8;
	}
}

const char* DefaultPassword()
{
	static char s_szPassword[] = "xuzhao20041108";

	return s_szPassword;
}

void DefaultPasswordToDefaultEncryptKey(EncryptKey key0, EncryptKey key1, EncryptKey key2)
{
	//byte md5[16] = {0};

	//MD5String(DefaultPassword(), md5);
	MD5 md5(DefaultPassword());
	unsigned char* ptr = md5.GetResult();

	memcpy(key0, ptr, sizeof(EncryptKey));
	memcpy(key1, ptr+8, sizeof(EncryptKey));
	memcpy(key2, ptr+4, sizeof(EncryptKey));
}


void DesEncryptData(byte* pInput, s32 nSize)
{
	EncryptKey key0;
	EncryptKey key1;
	EncryptKey key2;

	DefaultPasswordToDefaultEncryptKey(key0, key1, key2);

	DesEncryptData(pInput, nSize, key0, key1, key2);
}

void DesDecryptData(byte* pInput, s32 nSize)
{
	EncryptKey key0;
	EncryptKey key1;
	EncryptKey key2;

	DefaultPasswordToDefaultEncryptKey(key0, key1, key2);

	DesDecryptData(pInput, nSize, key0, key1, key2);
}

void DesEncryptHeader(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2)
{
	// Must be times of 8
	assert((nSize&0x7) == 0);

	DesKeySchedule desKey0;
	DesKeySchedule desKey1;
	DesKeySchedule desKey2;

    DesSetKey((DesCBlock*)key0, desKey0);
	DesSetKey((DesCBlock*)key1, desKey1);
	DesSetKey((DesCBlock*)key2, desKey2);

	byte* pData = pInput;
	for(int i=0; i<nSize; i+=8)
	{
		DesEncrypt3((unsigned long*)pData, desKey0, desKey1, desKey2, s_headerDesSPtrans);

		pData += 8;
	}
}

void DesDecryptHeader(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2)
{
	// Must be times of 8
	assert((nSize&0x7) == 0);

	DesKeySchedule desKey0;
	DesKeySchedule desKey1;
	DesKeySchedule desKey2;

    DesSetKey((DesCBlock*)key0, desKey0);
	DesSetKey((DesCBlock*)key1, desKey1);
	DesSetKey((DesCBlock*)key2, desKey2);

	byte* pData = pInput;
	for(int i=0; i<nSize; i+=8)
	{
		DesDecrypt3((unsigned long*)pData, desKey0, desKey1, desKey2, s_headerDesSPtrans);

		pData += 8;
	}
}

void DesEncryptHeader(byte* pInput, s32 nSize)
{
	EncryptKey key0;
	EncryptKey key1;
	EncryptKey key2;

	DefaultPasswordToDefaultEncryptKey(key0, key1, key2);

	DesEncryptHeader(pInput, nSize, key0, key1, key2);
}

void DesDecryptHeader(byte* pInput, s32 nSize)
{
	EncryptKey key0;
	EncryptKey key1;
	EncryptKey key2;

	DefaultPasswordToDefaultEncryptKey(key0, key1, key2);

	DesDecryptHeader(pInput, nSize, key0, key1, key2);
}

};
