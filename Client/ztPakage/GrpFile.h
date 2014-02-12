//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : GrpFile.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月2日
// 说    明 : 加载GRP文件类
//*******************************************************************************
#pragma once

#include <vector>
#include "grpheader.h"


class CFileStream;

class CGrpFile
{
public:
	CGrpFile(void);
	~CGrpFile(void);

	//open a grp file
	bool Open(const char* filename);

	//读取grp中第index个图片数据【单张图片】
	bool OpenXcp(int index);

	//读取grp中第index个图片数据【单张图片】
	bool OpenXcp2(int index);

	//读取grp中第xcpIndex个图片组的第frame张图片【不是动画组时 frame=0】
	bool OpenXcp3(int xcpIndex, int frame);


	BYTE* GetBuffer(){ return &m_vecDst[0]; }
	int GetWidth() { return m_nWidth; }
	int GetHeight() { return m_nHeight; }

	//得到
private:

	//RLE解压缩一行数据
	void RLE_DecodeLine(BYTE* &pSrc, size_t width, BYTE* pDst);

	int GetBlockCount(int size);
private:
	
	//GRP数据流
	CFileStream*	m_pGrpStream;

	//grp header
	stGrpHeader m_stGrpHeader;

	//grp中xcp
	std::vector<stGrpAniLocation>	m_vecGrpLoc;

	std::vector<BYTE> m_vecDst;

	int	m_nWidth, m_nHeight;


// 	//每个切分的图片块
// 	struct stXcpGrpBlock
// 	{
// 		stGrpBlock	grpBlock;							//ImageBlock所属的图片
// 		std::vector<stGrpImageBlock> vecImageBlockList;	//单张图片有几个block组成
// 	};

	//一个xcp结构
	struct stXcpData
	{
		stPackAniHeader m_xcpHeader;
		std::vector<stGrpBlock>	m_vecGrpBlockList;		//有xcpHeader.wdFrameCount张图片
	};
	
	//一个GRP结构
 	typedef std::vector<stXcpData>	XcpDataList;
 	XcpDataList	m_vecXcpData;

};
