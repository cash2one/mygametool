#include "precompile.h"
#include "crc.h"


#include "Stream.h"

typedef unsigned long DWORD;

static DWORD crcTable[256];
static bool crcTableValid = false;

void calculateCRCTable()
{
	DWORD Val;
	for (DWORD i = 0; i < 256; i++)
	{
		Val = i;
		for (DWORD k = 0; k < 8; k++)
		{
			if (Val & 1)
				Val = 0xedb88320L ^ (Val >> 1);
			else
				Val = Val >> 1;
		}
		crcTable[i] = Val;
	}
	crcTableValid = true;
}


DWORD Crc32(void* Buffer,DWORD Length, DWORD crcVal = 0xffffffff)
{
	if(!crcTableValid)
		calculateCRCTable();

	unsigned char* buf = (unsigned char*)Buffer;

	for (DWORD i = 0; i < Length; i++)
	{
		crcVal = crcTable[(crcVal ^ buf[i]) & 0xff] ^ (crcVal >> 8);
	}

	//return (crcVal ^ 0xffffffffL);
	return crcVal;
}

DWORD GenerateCrc32Stream(Stream* pStream, DWORD allSize, DWORD crcVal)
{
	if( !crcTableValid)
		calculateCRCTable();

	DWORD crc = crcVal;

	static unsigned char pData[4096] = {0};
	DWORD segCount  = (allSize + 4095) / 4096;

	for(unsigned long j = 0; j < segCount; ++j)
	{
		DWORD left = allSize - (j*4096);
		long slen = left > 4096 ? 4096 : left;

		pStream->Read(pData, slen);

		crc = Crc32(pData, slen, crc);
	}

	return (crc ^ 0xffffffffL);

}

DWORD GenerateCrc32(void* Buffer,DWORD Length, DWORD crcVal)
{	
	DWORD crc = Crc32(Buffer, Length, crcVal);

	return crc ^ 0xffffffffL;
}

/*
int main(int argc,char* argv[])
{
	char* szFileName = NULL;
	if (argc<2)
	{
		printf("Usage:%s <filename> \r\n",argv[0]);
		return -1;
	}
	szFileName = argv[1];
	FILE *fp = fopen(szFileName,"rb");
	if(NULL == fp) 
	{
		printf("Open %s error:%d \r\n",szFileName);
		return 0;
	}

	//计算文件大小
	fseek(fp, 0, SEEK_END);
	DWORD dwSize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	DWORD crc32 = 0;
	if(dwSize < 20*1024*1024)  //20M以下为小文件
	{
		char* pBuffer = new char[dwSize];
		fread(pBuffer, dwSize, 1, fp);
		crc32 = GenerateCrc32(pBuffer, dwSize);
		delete[] pBuffer;
	}
	else
	{
		crc32 = GenerateCrc32File(fp, dwSize);
	}

	crc32 ^= 0xffffffffL;
	fclose(fp);

	printf("%s,         CRC32 = 0x%08X\n\n", szFileName, crc32);

	return 0;
}
*/