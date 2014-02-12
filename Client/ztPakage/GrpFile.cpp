//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : GrpFile.cpp
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��2��
// ˵    �� : ����GRP�ļ��� 
//*******************************************************************************
#include "../Common/precompile.h"
#include "GrpFile.h"
#include "../Common/filestream.h"
#include "../Common/StreamCompress.h"

#include <sstream>

#define GRP_PRE_SIZE 16
#define ROUNDNUM2(value, num) ( ( (value) + (num-1)) & (~(num -1)))
#define MAX_TEXTURE_WIDTH 256

//�洢һ��ͼƬ����
static DWORD s_temBuf[256*256];

inline int GetWidth4(int size)
{
	return ROUNDNUM2(size, 4);
}

CGrpFile::CGrpFile(void)
{
}

CGrpFile::~CGrpFile(void)
{
	if(m_pGrpStream)
	{
		delete m_pGrpStream;
		m_pGrpStream = NULL;
	}
}

//Open a grp file
bool CGrpFile::Open(const char *filename)
{
	m_pGrpStream = new CFileStream;
	m_pGrpStream->Open(filename, Stream::ReadAccess);

	//��ȡgrp�ļ�ͷ
	ZeroMemory(&m_stGrpHeader, sizeof(stGrpHeader));
	m_pGrpStream->Seek(0+GRP_PRE_SIZE, Stream::SeekBegin);
	m_pGrpStream->Read(&m_stGrpHeader, sizeof(stGrpHeader));

	if(strncmp((char*)&m_stGrpHeader.dwMagic, "XZGL", 4) != 0)
	{
		return false;
	}
	
	//��ȡxcp
	
	m_vecGrpLoc.resize(m_stGrpHeader.dwPictureNum);
	m_vecXcpData.resize(m_stGrpHeader.dwPictureNum);

	m_pGrpStream->Seek(m_stGrpHeader.dwHeaderDataOffset+GRP_PRE_SIZE, Stream::SeekBegin);
	m_pGrpStream->Read(&m_vecGrpLoc[0], m_stGrpHeader.dwPictureNum*sizeof(stGrpAniLocation));

	//for( int i = 0; i < m_stGrpHeader.dwPictureNum; ++i)
	{
		stGrpAniLocation& loc = m_vecGrpLoc[0];
		//printf("xcpλ�� offset: %u, size: %u\n", loc.dwFileOffset, loc.dwFileSize);

		OpenXcp3(28,0);

		printf("\n\n-------------------------------------------\n\n");
	}
		
	return true;
}

//��ȡgrp�е�index��ͼƬ���ݡ�����ͼƬ��
bool CGrpFile::OpenXcp(int index)
{
	if(index >= m_vecGrpLoc.size())
		return false;

	//����ͼƬֻ�� ������ͼƬ�� stPackAniHeader + 1��stGrpBlock + ��������
	stPackAniHeader aniHeader;
	stGrpBlock		grpBlock;

	stGrpAniLocation& loc = m_vecGrpLoc[index];
	
	Stream::Offset offset = sizeof(stGrpHeader) + loc.dwFileOffset + GRP_PRE_SIZE;
	m_pGrpStream->Seek(offset, Stream::SeekBegin);
	
	m_pGrpStream->Read(&aniHeader, sizeof(stPackAniHeader));
	m_pGrpStream->Read(&grpBlock, sizeof(stGrpBlock));

	//////////////////////////////////////////////////////////////////////////
	std::ostringstream os;
	os << "δ�ü�width: " << grpBlock.wdWidth << " height: "<<grpBlock.wdHeight
		<< " �ü���width: " << grpBlock.wdClipWidth << " height: "<<grpBlock.wdClipHeight
		<< " Offset: " <<grpBlock.dwDataOffset<<std::endl;

// 	os << "ͼƬ���ݣ�offset: " <<imageBlock.offset 
// 		<< "left: " <<imageBlock.left
// 		<< " top: " <<imageBlock.top
// 		<< " width: "<<imageBlock.width
// 		<< " height: "<<imageBlock.height<<std::endl;

	printf(os.str().c_str());
	//////////////////////////////////////////////////////////////////////////
	
	//��ǰͼƬ�Ƿ��гɶ�顾256*256���������ȡ�
	int dwTotalWidthCnt = GetBlockCount(grpBlock.wdClipWidth);
	int dwTotalHeightCnt = GetBlockCount(grpBlock.wdClipHeight);

	std::vector<stGrpImageBlock> vecImageBlock;
	vecImageBlock.resize(dwTotalHeightCnt * dwTotalWidthCnt);

	m_pGrpStream->Read(&vecImageBlock[0], sizeof(stGrpImageBlock)*vecImageBlock.size());

	//////////////////////////////////////////////////////////////////////////
	std::ostringstream os2;

	for( int i= 0; i < vecImageBlock.size(); ++i)
	{
		stGrpImageBlock& block = vecImageBlock[i];
		os2<< " offset: "<< block.offset <<" [ " <<block.left <<" , "<< block.top <<" , "<<block.width <<" , "<<block.height;
		os2<< " ] "<<std::endl<<std::endl;
	}

	printf(os2.str().c_str());
	
	//////////////////////////////////////////////////////////////////////////
// 
	//��ʼ��ȡ������
	m_pGrpStream->Seek(grpBlock.dwDataOffset, Stream::SeekCurrent);

	//��ǰxcp�����ݶδ�С������������ͷ��
	DWORD dwXcpSize = loc.dwFileSize - sizeof(stPackAniHeader)-sizeof(stGrpBlock)-vecImageBlock.size()*sizeof(stGrpImageBlock);

	m_pGrpStream->Seek(vecImageBlock[0].offset, Stream::SeekCurrent);
	//��ȡÿ���п������
	int vecSize = vecImageBlock.size();
	for(int i = 0 ; i < vecSize; ++i)
	{
		Stream::Size dataSize = 0;

		if( vecSize == 1)
			dataSize = dwXcpSize;
		//���һ��block
		else if(i == vecSize-1)
			dataSize = dwXcpSize - vecImageBlock[i].offset;
		else
			dataSize = vecImageBlock[i+1].offset-vecImageBlock[i].offset;

		//ͼƬ�гɵ�СͼƬ
		stGrpImageBlock& imageBlock = vecImageBlock[i];


		BYTE* pByte = new BYTE[dataSize];
		ZeroMemory(pByte, dataSize);

		//printf("*******pos: %u\n", m_pGrpStream->GetPosition());
		m_pGrpStream->Read(pByte, dataSize);


		//��ʼ��ѹ����
		SIZE sizeTexBlock = { GetWidth4(imageBlock.width), GetWidth4(imageBlock.height)};
		DWORD dwTexRowCount = sizeTexBlock.cy/4;
		DWORD dwTexRowSize = (sizeTexBlock.cx/4) * 16;      //16����Ϊ D3DFMT_DXT3
		RECT rcDst = { 0, 0, sizeTexBlock.cx, sizeTexBlock.cy};


		m_vecDst.clear();
		m_vecDst.resize(dwTexRowCount*dwTexRowSize);
		BYTE* pDst = (BYTE*)s_temBuf;
		BYTE* pSrc = pByte;

		ZeroMemory(pDst, sizeof(s_temBuf));

		for(int jj = 0; jj < dwTexRowCount; ++jj)
		{
			
			RLE_DecodeLine(pSrc, dwTexRowSize, pDst);
			pDst += 256*4;
		}

		char buffer[100] = {0};
		sprintf(buffer, "vv-%d.vvv", i);

		FILE* p2 = fopen(buffer, "wb");
		if(p2)
		{
			BYTE* pTemp = (BYTE*)s_temBuf;
			fwrite(pTemp, 256*256*4, 1, p2);
			fclose(p2);

		}


		delete[] pByte;
		
	}

	return true;
}

int CGrpFile::GetBlockCount(int size)
{
	if(size % MAX_TEXTURE_WIDTH)
		return size/MAX_TEXTURE_WIDTH + 1 ;
	return size/MAX_TEXTURE_WIDTH;
}

//RLE��ѹ��һ������
void CGrpFile::RLE_DecodeLine(BYTE* &pSrc, size_t width, BYTE* pDst)
{
	if(!pSrc)	return;

	const size_t c1 = sizeof(BYTE)*8 -1;
	const size_t c2 = sizeof(BYTE)*8;
	const size_t max_line = (1<<c1);
	const size_t mask_rle = (1<<c1);

	for(size_t i = 0; i < width; )
	{
		size_t flag = *pSrc++;
		size_t len = (flag & (~mask_rle)) +1;
		if(flag & mask_rle)			//��������128
		{
			BYTE value = *pSrc++;
			memset(pDst, value, len);

			pDst += len;
			i += len;
		}
		else
		{
			memcpy(pDst, pSrc, len);

			pDst += len;
			pSrc += len;
			i += len;
		}

	}

}


//��ȡgrp�е�index��ͼƬ���ݡ�����ͼƬ��
bool CGrpFile::OpenXcp2(int index)
{
	if(index >= m_vecGrpLoc.size())
		return false;

	stGrpAniLocation& loc = m_vecGrpLoc[index];
	stXcpData& xcpData = m_vecXcpData[index];

	Stream::Offset offset = sizeof(stGrpHeader) + loc.dwFileOffset + GRP_PRE_SIZE;
	m_pGrpStream->Seek(offset, Stream::SeekBegin);

	//��ȡstPackAniHeader
	m_pGrpStream->Read(&xcpData.m_xcpHeader, sizeof(stPackAniHeader));

	//����xcp�е�ͼƬ�����������ж���ͼƬ��
	DWORD dwFrameCount = xcpData.m_xcpHeader.wdFrameCount;
	xcpData.m_vecGrpBlockList.resize(dwFrameCount);
	printf("xcp-%d �� %u ��ͼƬ", index, dwFrameCount);


	//�����ž�Ҫ��ȡstGrpBlock�ˣ� һ��ͼƬ��Ӧһ��stGrpBlock
	for(DWORD dwIndex = 0; dwIndex < dwFrameCount; ++dwIndex)
	{
		stGrpBlock& block = xcpData.m_vecGrpBlockList[dwIndex];
		m_pGrpStream->Read(&block, sizeof(stGrpBlock));
	}

	return true;
}



//��ȡxcp�е�index��ͼƬ��ĵ�frame��ͼƬ�����Ƕ�����ʱ frame=0��
bool CGrpFile::OpenXcp3(int xcpIndex, int frame)
{
	if(xcpIndex >= m_vecXcpData.size())
		return false;

	//xcp��
	stGrpAniLocation& loc = m_vecGrpLoc[xcpIndex];
	stXcpData& xcpData = m_vecXcpData[xcpIndex];

	

	//��ʼ��λxcp��λ��
	Stream::Offset offset = sizeof(stGrpHeader) + loc.dwFileOffset + GRP_PRE_SIZE;
	m_pGrpStream->Seek(offset, Stream::SeekBegin);
	//ƫ��N��stGrpBlock��λ�ã��ҵ�ImageBlock��λ��

	m_pGrpStream->Read(&xcpData.m_xcpHeader, sizeof(stPackAniHeader));
	DWORD FrameCount = xcpData.m_xcpHeader.wdFrameCount;
	xcpData.m_vecGrpBlockList.resize(FrameCount);
	m_pGrpStream->Read(&xcpData.m_vecGrpBlockList[0], sizeof(stGrpBlock)*FrameCount);



	//ͼƬ��
	//������ǰͼƬ���м���ͼƬ
	if(xcpData.m_vecGrpBlockList.size() == 1)
		frame = 0;
	else if(frame >= xcpData.m_vecGrpBlockList.size())
		frame = xcpData.m_vecGrpBlockList.size()-1;

	stGrpBlock& grpBlock = xcpData.m_vecGrpBlockList[frame];

	
	
	//�ã����ڿ�ʼ����ͼƬ������
	//�ҵ�ͼƬ��λ��
	m_pGrpStream->Seek(grpBlock.dwDataOffset, Stream::SeekCurrent);


	//��ǰͼƬ�Ƿ��гɶ�顾256*256���������ȡ�
	int dwTotalWidthCnt = GetBlockCount(grpBlock.wdClipWidth);
	int dwTotalHeightCnt = GetBlockCount(grpBlock.wdClipHeight);

	std::vector<stGrpImageBlock> vecImageBlock;
	vecImageBlock.resize(dwTotalHeightCnt * dwTotalWidthCnt);

	m_pGrpStream->Read(&vecImageBlock[0], sizeof(stGrpImageBlock)*vecImageBlock.size());


	//��ǰxcp�����ݶδ�С������������ͷ��
	DWORD dwXcpSize = loc.dwFileSize - sizeof(stPackAniHeader)-sizeof(stGrpBlock)*xcpData.m_vecGrpBlockList.size()
		- vecImageBlock.size()*sizeof(stGrpImageBlock);

	if(dwXcpSize <= 0 )
	{
		return false;
	}

	//��ȡÿ���п������
	int vecSize = vecImageBlock.size();
	for(int i = 0 ; i < vecSize; ++i)
	{
		Stream::Size dataSize = 0;

		if( vecSize == 1)
			dataSize = dwXcpSize;
		//���һ��block
		else if(i == vecSize-1)
			dataSize = dwXcpSize - vecImageBlock[i].offset;
		else
			dataSize = vecImageBlock[i+1].offset-vecImageBlock[i].offset;

		if(dataSize <= 0)
			continue;

		//ͼƬ�гɵ�СͼƬ
		stGrpImageBlock& imageBlock = vecImageBlock[i];


		BYTE* pByte = new BYTE[dataSize];
		ZeroMemory(pByte, dataSize);

		//printf("*******pos: %u\n", m_pGrpStream->GetPosition());
		m_pGrpStream->Read(pByte, dataSize);


		//��ʼ��ѹ����
		SIZE sizeTexBlock = { GetWidth4(imageBlock.width), GetWidth4(imageBlock.height)};
		DWORD dwTexRowCount = sizeTexBlock.cy/4;
		DWORD dwTexRowSize = (sizeTexBlock.cx/4) * 16;      //16����Ϊ D3DFMT_DXT3
		RECT rcDst = { 0, 0, sizeTexBlock.cx, sizeTexBlock.cy};


		m_vecDst.clear();
		m_vecDst.resize(dwTexRowCount*dwTexRowSize);
		BYTE* pDst = (BYTE*)s_temBuf;
		BYTE* pSrc = pByte;

		ZeroMemory(pDst, sizeof(s_temBuf));

		for(int jj = 0; jj < dwTexRowCount; ++jj)
		{

			RLE_DecodeLine(pSrc, dwTexRowSize, pDst);
			pDst += 256*4;
		}

		char buffer[100] = {0};
		sprintf(buffer, "nnn-%d.nnn", i);

		FILE* p2 = fopen(buffer, "wb");
		if(p2)
		{
			BYTE* pTemp = (BYTE*)s_temBuf;
			fwrite(pTemp, 256*256*4, 1, p2);
			fclose(p2);

		}


		delete[] pByte;

	}

	return true;
}