//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : PakFile.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��18��
// ˵    �� : ����PAK�ļ�
//*******************************************************************************
#pragma once

#include "pakheader.h"

class CPakFile
{
public:
	CPakFile(void);
	~CPakFile(void);

	//��һ��pak�ļ�
	bool Open(const char* pakfile);

	//��ѹ��һ���ļ�����datas.pak
	void CompressFile();

	void CompressFile2();

public:

	struct stFileBaseInfo
	{
		char szName[MAX_PATH];
		DWORD fileOffset;
		DWORD size;
		DWORD dwCRC;

		void print();
	};

	typedef std::vector<stFileBaseInfo>::const_iterator iterator;
	iterator Begin() { return m_vecFile.begin(); }
	iterator End() { return m_vecFile.end(); }

private:

	std::string m_strPakName;
	DWORD		m_dwFlags;
	DWORD		m_dwVersion;
	DWORD		m_dwFileNum;

	//�����ļ�

	
	std::vector<stFileBaseInfo>	m_vecFile;
};
