#pragma once

typedef unsigned char byte;
typedef int s32;

namespace bin
{

typedef byte EncryptKey[8];

void DesEncryptData(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2);
void DesDecryptData(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2);
void DesEncryptData(byte* pInput, s32 nSize);
void DesDecryptData(byte* pInput, s32 nSize);
void DesEncryptHeader(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2);
void DesDecryptHeader(byte* pInput, s32 nSize, EncryptKey key0, EncryptKey key1, EncryptKey key2);
void DesEncryptHeader(byte* pInput, s32 nSize);
void DesDecryptHeader(byte* pInput, s32 nSize);
const char* DefaultPassword();
void DefaultPasswordToDefaultEncryptKey(EncryptKey key0, EncryptKey key1, EncryptKey key2);

};