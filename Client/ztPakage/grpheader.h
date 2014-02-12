//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : grpheader.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��2��
// ˵    �� : grp�ļ���ʽ
//*******************************************************************************

#pragma once

#include <vector>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//GRP�ļ�ͷ
struct stGrpHeader
{
	DWORD	dwMagic;	//��XZGL��
	DWORD	dwVersion;
	DWORD	dwPictureNum;		//���Ǹ�ͼƬ���йصģ�����grp��ֻ��0.tga �� 10.tga ��ôdwPictureNum = 11
	DWORD	dwHeaderDataOffset;  //��ָxcpͷ����ƫ�ƣ��������stGrpAniInfo,�Ǵ��ļ�ͷ��ʼ������grpheader
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//GRP���ݶ�



//�м����ݣ� aniInfo + 

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

//ͼ�����Ϣ
struct stGrpBlock
{
	BYTE byGrpType;		//ͼƬѹ������
	union
	{
		BYTE byPixelFormat;	//���ظ�ʽ
		BYTE byAlphaType;	
	};
	union
	{
		BYTE byAlpha;
		WORD reserve;
	};

	WORD wdWidth;	//ͼ���ȡ�δ�ü���
	WORD wdHeight;	//ͼ��߶ȡ�δ�ü���

	WORD wdClipLeft;//�ü��������ԭͼ����x [ �ü�����ȥ��alpha < 0x0f��ֵ]
	WORD wdClipTop; //��y

	WORD wdClipWidth; //�ü���Ŀ��
	WORD wdClipHeight;//�ü���ĸ߶�
	DWORD dwDataOffset;
};

struct stBitmaps
{
	stPackAniHeader aniInfo;
	std::vector<stGrpBlock>	aBlock;
	std::vector<DWORD>	aFrameIndex;		//����ͼƬΪ0�� ���Ŷ�������
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//����xcp���ļ�ͷ����GRP���ݵ������ 
struct stGrpAniLocation
{
	DWORD dwFileOffset;	//�����ļ�λ�ã������������λ��
	DWORD dwFileSize;	//�����ļ���С[����xcp��ͷ����]
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/*
GRP�ļ��� GRPͷ + N��XCP +��N ��stGrpAniLocation
XCP�ļ���������ͼƬ�� stPackAniHeader + 1��stGrpBlock + ��������
������ͼƬ�顿stPackAniHeader + N��aFrameIndex +  N��stGrpBlock + ��������
*/


struct stGrpImageBlock
{
	DWORD offset;
	WORD left;
	WORD top;
	WORD width;
	WORD height;
};