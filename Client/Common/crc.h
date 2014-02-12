#pragma once

class Stream;


DWORD GenerateCrc32(void* Buffer,DWORD Length, DWORD crcVal = 0xffffffff);

DWORD GenerateCrc32Stream(Stream* pFile, DWORD allSize, DWORD crcVal = 0xffffffff);