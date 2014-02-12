//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : grpheader.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月2日
// 说    明 : grp文件格式
//*******************************************************************************

#pragma once

#include <vector>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//GRP文件头
struct stGrpHeader
{
	DWORD	dwMagic;	//“XZGL”
	DWORD	dwVersion;
	DWORD	dwPictureNum;		//这是跟图片名有关的，假设grp中只有0.tga 和 10.tga 那么dwPictureNum = 11
	DWORD	dwHeaderDataOffset;  //是指xcp头的首偏移，既下面的stGrpAniInfo,是从文件头开始，包括grpheader
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//GRP数据段



//中间数据： aniInfo + 

struct stPackAniHeader
{
	WORD version;
	BYTE blend;
	BYTE playtype;
	WORD wdBitmapCount;
	WORD wdFrameCount;
	DWORD dwAniSpeed;
	DWORD intervalTime;
	DWORD dwColor;
};

//图像块信息
struct stGrpBlock
{
	BYTE byGrpType;		//图片压缩类型
	union
	{
		BYTE byPixelFormat;	//像素格式
		BYTE byAlphaType;	
	};
	union
	{
		BYTE byAlpha;
		WORD reserve;
	};

	WORD wdWidth;	//图像宽度【未裁剪】
	WORD wdHeight;	//图像高度【未裁剪】

	WORD wdClipLeft;//裁剪后相对于原图的左x [ 裁剪：即去除alpha < 0x0f的值]
	WORD wdClipTop; //上y

	WORD wdClipWidth; //裁剪后的宽度
	WORD wdClipHeight;//裁剪后的高度
	DWORD dwDataOffset;
};

struct stBitmaps
{
	stPackAniHeader aniInfo;
	std::vector<stGrpBlock>	aBlock;
	std::vector<DWORD>	aFrameIndex;		//单张图片为0， 多张动画张数
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//所有xcp的文件头，在GRP数据的最后面 
struct stGrpAniLocation
{
	DWORD dwFileOffset;	//动画文件位置，相对于数据区位置
	DWORD dwFileSize;	//动画文件大小[包含xcp的头数据]
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
GRP文件： GRP头 + N个XCP +　N 个stGrpAniLocation
XCP文件：【单张图片】 stPackAniHeader + 1个stGrpBlock + 真正数据
【动画图片组】stPackAniHeader + N个aFrameIndex +  N个stGrpBlock + 真正数据
*/


struct stGrpImageBlock
{
	DWORD offset;
	WORD left;
	WORD top;
	WORD width;
	WORD height;
};