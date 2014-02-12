//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : GrpFile.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��2��
// ˵    �� : ����GRP�ļ���
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

	//��ȡgrp�е�index��ͼƬ���ݡ�����ͼƬ��
	bool OpenXcp(int index);

	//��ȡgrp�е�index��ͼƬ���ݡ�����ͼƬ��
	bool OpenXcp2(int index);

	//��ȡgrp�е�xcpIndex��ͼƬ��ĵ�frame��ͼƬ�����Ƕ�����ʱ frame=0��
	bool OpenXcp3(int xcpIndex, int frame);


	BYTE* GetBuffer(){ return &m_vecDst[0]; }
	int GetWidth() { return m_nWidth; }
	int GetHeight() { return m_nHeight; }

	//�õ�
private:

	//RLE��ѹ��һ������
	void RLE_DecodeLine(BYTE* &pSrc, size_t width, BYTE* pDst);

	int GetBlockCount(int size);
private:
	
	//GRP������
	CFileStream*	m_pGrpStream;

	//grp header
	stGrpHeader m_stGrpHeader;

	//grp��xcp
	std::vector<stGrpAniLocation>	m_vecGrpLoc;

	std::vector<BYTE> m_vecDst;

	int	m_nWidth, m_nHeight;


// 	//ÿ���зֵ�ͼƬ��
// 	struct stXcpGrpBlock
// 	{
// 		stGrpBlock	grpBlock;							//ImageBlock������ͼƬ
// 		std::vector<stGrpImageBlock> vecImageBlockList;	//����ͼƬ�м���block���
// 	};

	//һ��xcp�ṹ
	struct stXcpData
	{
		stPackAniHeader m_xcpHeader;
		std::vector<stGrpBlock>	m_vecGrpBlockList;		//��xcpHeader.wdFrameCount��ͼƬ
	};
	
	//һ��GRP�ṹ
 	typedef std::vector<stXcpData>	XcpDataList;
 	XcpDataList	m_vecXcpData;

};
