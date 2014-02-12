//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : FileStream.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月2日
// 说    明 : 加载文件数据流[内部使用FILE]
//*******************************************************************************
#pragma once
#include "stream.h"

class CFileStream :
	public Stream
{
public:
	CFileStream(void);

	~CFileStream(void);

	//Open a file stream
	bool Open(const char* pszFile, AccessMode accessmode = ReadAndWriteAccess);

	virtual Size GetSize();

	virtual Position GetPosition();

	virtual void Write(const void* ptr, Size numBytes);

	virtual Size Read(void* ptr, Size numBytes);

	virtual void Seek(Offset offset, SeekOrigin origin);

	virtual void Flush();

private:

	FILE*	m_pFile;

};
