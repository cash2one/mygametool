//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : GrpFile.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月2日
// 说    明 : 加载GRP文件类 
//*******************************************************************************
#include "../Common/precompile.h"
#include "GrpFile.h"
#include "../Common/filestream.h"
#include "../Common/StreamCompress.h"

#include <sstream>

#define GRP_PRE_SIZE 16
#define ROUNDNUM2(value, num) ( ( (value) + (num-1)) & (~(num -1)))
#define MAX_TEXTURE_WIDTH 256

//存储一张图片数据
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

	//读取grp文件头
	ZeroMemory(&m_stGrpHeader, sizeof(stGrpHeader));
	m_pGrpStream->Seek(0+GRP_PRE_SIZE, Stream::SeekBegin);
	m_pGrpStream->Read(&m_stGrpHeader, sizeof(stGrpHeader));

	if(strncmp((char*)&m_stGrpHeader.dwMagic, "XZGL", 4) != 0)
	{
		return false;
	}
	
	//读取xcp
	
	m_vecGrpLoc.resize(m_stGrpHeader.dwPictureNum);
	m_vecXcpData.resize(m_stGrpHeader.dwPictureNum);

	m_pGrpStream->Seek(m_stGrpHeader.dwHeaderDataOffset+GRP_PRE_SIZE, Stream::SeekBegin);
	m_pGrpStream->Read(&m_vecGrpLoc[0], m_stGrpHeader.dwPictureNum*sizeof(stGrpAniLocation));

	//for( int i = 0; i < m_stGrpHeader.dwPictureNum; ++i)
	{
		stGrpAniLocation& loc = m_vecGrpLoc[0];
		//printf("xcp位置 offset: %u, size: %u\n", loc.dwFileOffset, loc.dwFileSize);

		OpenXcp3(28,0);

		printf("\n\n-------------------------------------------\n\n");
	}
		
	return true;
}

//读取grp中第index个图片数据【单张图片】
bool CGrpFile::OpenXcp(int index)
{
	if(index >= m_vecGrpLoc.size())
		return false;

	//单张图片只有 【单张图片】 stPackAniHeader + 1个stGrpBlock + 真正数据
	stPackAniHeader aniHeader;
	stGrpBlock		grpBlock;

	stGrpAniLocation& loc = m_vecGrpLoc[index];
	
	Stream::Offset offset = sizeof(stGrpHeader) + loc.dwFileOffset + GRP_PRE_SIZE;
	m_pGrpStream->Seek(offset, Stream::SeekBegin);
	
	m_pGrpStream->Read(&aniHeader, sizeof(stPackAniHeader));
	m_pGrpStream->Read(&grpBlock, sizeof(stGrpBlock));

	//////////////////////////////////////////////////////////////////////////
	std::ostringstream os;
	os << "未裁剪width: " << grpBlock.wdWidth << " height: "<<grpBlock.wdHeight
		<< " 裁剪后width: " << grpBlock.wdClipWidth << " height: "<<grpBlock.wdClipHeight
		<< " Offset: " <<grpBlock.dwDataOffset<<std::endl;

// 	os << "图片数据：offset: " <<imageBlock.offset 
// 		<< "left: " <<imageBlock.left
// 		<< " top: " <<imageBlock.top
// 		<< " width: "<<imageBlock.width
// 		<< " height: "<<imageBlock.height<<std::endl;

	printf(os.str().c_str());
	//////////////////////////////////////////////////////////////////////////
	
	//当前图片是否切成多块【256*256】【行优先】
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
	//开始读取数据了
	m_pGrpStream->Seek(grpBlock.dwDataOffset, Stream::SeekCurrent);

	//当前xcp的数据段大小【不包含数据头】
	DWORD dwXcpSize = loc.dwFileSize - sizeof(stPackAniHeader)-sizeof(stGrpBlock)-vecImageBlock.size()*sizeof(stGrpImageBlock);

	m_pGrpStream->Seek(vecImageBlock[0].offset, Stream::SeekCurrent);
	//读取每个切块的数据
	int vecSize = vecImageBlock.size();
	for(int i = 0 ; i < vecSize; ++i)
	{
		Stream::Size dataSize = 0;

		if( vecSize == 1)
			dataSize = dwXcpSize;
		//最后一个block
		else if(i == vecSize-1)
			dataSize = dwXcpSize - vecImageBlock[i].offset;
		else
			dataSize = vecImageBlock[i+1].offset-vecImageBlock[i].offset;

		//图片切成的小图片
		stGrpImageBlock& imageBlock = vecImageBlock[i];


		BYTE* pByte = new BYTE[dataSize];
		ZeroMemory(pByte, dataSize);

		//printf("*******pos: %u\n", m_pGrpStream->GetPosition());
		m_pGrpStream->Read(pByte, dataSize);


		//开始解压数据
		SIZE sizeTexBlock = { GetWidth4(imageBlock.width), GetWidth4(imageBlock.height)};
		DWORD dwTexRowCount = sizeTexBlock.cy/4;
		DWORD dwTexRowSize = (sizeTexBlock.cx/4) * 16;      //16是因为 D3DFMT_DXT3
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

//RLE解压缩一行数据
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
		if(flag & mask_rle)			//个数大于128
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


//读取grp中第index个图片数据【单张图片】
bool CGrpFile::OpenXcp2(int index)
{
	if(index >= m_vecGrpLoc.size())
		return false;

	stGrpAniLocation& loc = m_vecGrpLoc[index];
	stXcpData& xcpData = m_vecXcpData[index];

	Stream::Offset offset = sizeof(stGrpHeader) + loc.dwFileOffset + GRP_PRE_SIZE;
	m_pGrpStream->Seek(offset, Stream::SeekBegin);

	//读取stPackAniHeader
	m_pGrpStream->Read(&xcpData.m_xcpHeader, sizeof(stPackAniHeader));

	//看看xcp中的图片数【动画组有多张图片】
	DWORD dwFrameCount = xcpData.m_xcpHeader.wdFrameCount;
	xcpData.m_vecGrpBlockList.resize(dwFrameCount);
	printf("xcp-%d 有 %u 张图片", index, dwFrameCount);


	//紧接着就要读取stGrpBlock了， 一个图片对应一个stGrpBlock
	for(DWORD dwIndex = 0; dwIndex < dwFrameCount; ++dwIndex)
	{
		stGrpBlock& block = xcpData.m_vecGrpBlockList[dwIndex];
		m_pGrpStream->Read(&block, sizeof(stGrpBlock));
	}

	return true;
}



//读取xcp中第index个图片组的第frame张图片【不是动画组时 frame=0】
bool CGrpFile::OpenXcp3(int xcpIndex, int frame)
{
	if(xcpIndex >= m_vecXcpData.size())
		return false;

	//xcp组
	stGrpAniLocation& loc = m_vecGrpLoc[xcpIndex];
	stXcpData& xcpData = m_vecXcpData[xcpIndex];

	

	//开始定位xcp的位置
	Stream::Offset offset = sizeof(stGrpHeader) + loc.dwFileOffset + GRP_PRE_SIZE;
	m_pGrpStream->Seek(offset, Stream::SeekBegin);
	//偏移N个stGrpBlock的位置，找到ImageBlock的位置

	m_pGrpStream->Read(&xcpData.m_xcpHeader, sizeof(stPackAniHeader));
	DWORD FrameCount = xcpData.m_xcpHeader.wdFrameCount;
	xcpData.m_vecGrpBlockList.resize(FrameCount);
	m_pGrpStream->Read(&xcpData.m_vecGrpBlockList[0], sizeof(stGrpBlock)*FrameCount);



	//图片组
	//看看当前图片组有几张图片
	if(xcpData.m_vecGrpBlockList.size() == 1)
		frame = 0;
	else if(frame >= xcpData.m_vecGrpBlockList.size())
		frame = xcpData.m_vecGrpBlockList.size()-1;

	stGrpBlock& grpBlock = xcpData.m_vecGrpBlockList[frame];

	
	
	//好，现在开始查找图片数据了
	//找到图片的位置
	m_pGrpStream->Seek(grpBlock.dwDataOffset, Stream::SeekCurrent);


	//当前图片是否切成多块【256*256】【行优先】
	int dwTotalWidthCnt = GetBlockCount(grpBlock.wdClipWidth);
	int dwTotalHeightCnt = GetBlockCount(grpBlock.wdClipHeight);

	std::vector<stGrpImageBlock> vecImageBlock;
	vecImageBlock.resize(dwTotalHeightCnt * dwTotalWidthCnt);

	m_pGrpStream->Read(&vecImageBlock[0], sizeof(stGrpImageBlock)*vecImageBlock.size());


	//当前xcp的数据段大小【不包含数据头】
	DWORD dwXcpSize = loc.dwFileSize - sizeof(stPackAniHeader)-sizeof(stGrpBlock)*xcpData.m_vecGrpBlockList.size()
		- vecImageBlock.size()*sizeof(stGrpImageBlock);

	if(dwXcpSize <= 0 )
	{
		return false;
	}

	//读取每个切块的数据
	int vecSize = vecImageBlock.size();
	for(int i = 0 ; i < vecSize; ++i)
	{
		Stream::Size dataSize = 0;

		if( vecSize == 1)
			dataSize = dwXcpSize;
		//最后一个block
		else if(i == vecSize-1)
			dataSize = dwXcpSize - vecImageBlock[i].offset;
		else
			dataSize = vecImageBlock[i+1].offset-vecImageBlock[i].offset;

		if(dataSize <= 0)
			continue;

		//图片切成的小图片
		stGrpImageBlock& imageBlock = vecImageBlock[i];


		BYTE* pByte = new BYTE[dataSize];
		ZeroMemory(pByte, dataSize);

		//printf("*******pos: %u\n", m_pGrpStream->GetPosition());
		m_pGrpStream->Read(pByte, dataSize);


		//开始解压数据
		SIZE sizeTexBlock = { GetWidth4(imageBlock.width), GetWidth4(imageBlock.height)};
		DWORD dwTexRowCount = sizeTexBlock.cy/4;
		DWORD dwTexRowSize = (sizeTexBlock.cx/4) * 16;      //16是因为 D3DFMT_DXT3
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