//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : FileStream.cpp
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��2��
// ˵    �� : 
//*******************************************************************************
#include "precompile.h"
#include "FileStream.h"

namespace
{
	static const char* s_pszAccessMode[] = 
	{
		"rb",
		"wb",
		"r+",
		"a+"
	};
}
CFileStream::CFileStream(void)
:m_pFile(NULL)
{

}

CFileStream::~CFileStream(void)
{
	if(m_pFile)
	{
		::fclose(m_pFile);
		m_pFile = NULL;
	}
}

bool CFileStream::Open(const char* pszFile, AccessMode accessmode /*= ReadAndWriteAccess*/)
{
	m_eAccessMode = accessmode;
	
	m_pFile = ::fopen(pszFile, s_pszAccessMode[accessmode]);

	return m_pFile != NULL;
}

Stream::Size CFileStream::GetSize()
{
	Size size = 0;
	Size old = ::ftell(m_pFile);

	::fseek(m_pFile, 0, SEEK_END);
	size = ::ftell(m_pFile);
	::fseek(m_pFile, old, SEEK_SET);

	return size;
}

Stream::Position CFileStream::GetPosition()
{
	return ::ftell(m_pFile);
}

void CFileStream::Seek(Stream::Offset offset, Stream::SeekOrigin origin)
{
	if(origin == Stream::SeekBegin)
		::fseek(m_pFile, offset, SEEK_SET);
	else if(origin == Stream::SeekCurrent)
		::fseek(m_pFile, offset, SEEK_CUR);
	else if(origin == Stream::SeekEnd)
		::fseek(m_pFile, offset, SeekEnd);
	else
		::fseek(m_pFile, offset,SEEK_SET);
}

void CFileStream::Flush()
{
	::fflush(m_pFile);
}

void CFileStream::Write(const void *ptr, Stream::Size numBytes)
{
	::fwrite(ptr, numBytes, 1, m_pFile);
}

Stream::Size CFileStream::Read(void *ptr, Stream::Size numBytes)
{
	::fread(ptr, numBytes, 1, m_pFile);

	return numBytes;
}