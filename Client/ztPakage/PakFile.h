//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : PakFile.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月18日
// 说    明 : 加载PAK文件
//*******************************************************************************
#pragma once

#include "pakheader.h"

class CPakFile
{
public:
	CPakFile(void);
	~CPakFile(void);

	//打开一个pak文件
	bool Open(const char* pakfile);

	//解压缩一般文件，如datas.pak
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

	//所有文件

	
	std::vector<stFileBaseInfo>	m_vecFile;
};
