//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : FileStream.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��2��
// ˵    �� : �����ļ�������[�ڲ�ʹ��FILE]
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
