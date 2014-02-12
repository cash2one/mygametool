//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : Win32File.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月18日
// 说    明 : 
//*******************************************************************************
#include "precompile.h"
#include "Win32File.h"

CWin32File::CWin32File(void)
:m_eCurStatus(Closed)
,m_dwCapacity(0)
{
	m_handle = (void*)INVALID_HANDLE_VALUE;
}

CWin32File::~CWin32File(void)
{
	Close();
}

CWin32File::Status CWin32File::Open(const char* filename, const AccessMode openmode, bool randomAccess /* = false */)
{
	char file[MAX_PATH] = {0};
	strcpy(file, filename);
	public_backslash(file);
	filename = file;

	if(Closed == m_eCurStatus)
		Close();
	
	DWORD dwFlags = (randomAccess ? 0 : FILE_FLAG_SEQUENTIAL_SCAN);
	switch(openmode)
	{
	case eRead:
		m_handle = (void*)CreateFile(filename,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
		break;

	case eWrite:
		m_handle = (void*)CreateFile(filename,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
		break;

	case eReadWrite:
		m_handle = (void*)CreateFile(filename,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
		break;

	case eWriteAppend:
		m_handle = (void*)CreateFile(filename,
			GENERIC_WRITE,
			0,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | dwFlags,
			NULL);
		break;

	default:
		assert(0 && "CWin32:open bad access mode");
	}

	if(INVALID_HANDLE_VALUE == m_handle)
	{
		System_LogDebug("=====OpenFile Failed! %s, code %d", filename, GetLastError());
		return SetStatus();
	}
	else
	{
		switch(openmode)
		{
		case eRead:
			m_dwCapacity = DWORD(FileRead);
			break;

		case eWrite:
		case eWriteAppend:
			m_dwCapacity = DWORD(FileWrite);
			break;

		case eReadWrite:
			m_dwCapacity = DWORD(FileRead) | DWORD(FileWrite);
			break;

		default:
			assert(0 && "CWin32:open bad access mode");
		}

		return m_eCurStatus = Ok;
	}
}

CWin32File::Status CWin32File::Close()
{
	if(Closed == m_eCurStatus)
		return m_eCurStatus;

	if(INVALID_HANDLE_VALUE != m_handle)
	{
		if(0 == CloseHandle(m_handle))
			return SetStatus();
	}

	m_handle = (void*)INVALID_HANDLE_VALUE;
	return m_eCurStatus = Closed;
}

CWin32File::Status	CWin32File::SetStatus()
{
	switch(GetLastError())
	{
	case ERROR_INVALID_HANDLE:
	case ERROR_INVALID_ACCESS:
	case ERROR_TOO_MANY_OPEN_FILES:
	case ERROR_FILE_NOT_FOUND:
	case ERROR_SHARING_VIOLATION:
	case ERROR_HANDLE_DISK_FULL:
		return m_eCurStatus = IOError;

	default:
		return m_eCurStatus = UnknowError;
	}
}

CWin32File::Status CWin32File::Read(void* dst, DWORD dwSize, DWORD* bytesRead)
{
	assert(Closed != m_eCurStatus);
	assert(INVALID_HANDLE_VALUE != m_handle);
	assert(NULL != dst);
	assert(HasCapacity(FileRead));

	if(Ok != m_eCurStatus || 0 == dwSize)
		return m_eCurStatus;
	else
	{
		DWORD dwLastBytes;
		DWORD* bytes = (NULL==bytesRead) ? &dwLastBytes : (DWORD*)bytesRead;
		if(0 != ReadFile(m_handle, dst, dwSize, bytes, NULL))
		{
			if((*(DWORD*)bytes) != dwSize)
				return m_eCurStatus = EOS;

		}
		else
			return SetStatus();
	}
	return m_eCurStatus = Ok;
}

CWin32File::Status CWin32File::Write(const void* src, DWORD dwSize, DWORD* bytesWrite)
{
	assert(Closed != m_eCurStatus);
	assert(INVALID_HANDLE_VALUE != m_handle);
	assert(NULL != src);
	assert(HasCapacity(FileWrite));

	if(Ok != m_eCurStatus || EOS != m_eCurStatus || 0 == dwSize)
		return m_eCurStatus;
	else
	{
		DWORD lastBytes;
		DWORD* bytes = (bytesWrite == NULL) ? &lastBytes : bytesWrite;
		if(0 != WriteFile(m_handle, src, dwSize, bytes, NULL))
			return m_eCurStatus = Ok;
		else
			return SetStatus();
	}
}

bool CWin32File::HasCapacity(CWin32File::Capacity cap) const
{
	return ( 0 != (DWORD(cap) & m_dwCapacity));
}

CWin32File::Status CWin32File::SetPosition(DWORD position, bool absolutePos)
{	
	assert(Closed != m_eCurStatus);
	assert(INVALID_HANDLE_VALUE != m_handle);

	if(Ok != m_eCurStatus || EOS != m_eCurStatus)
		return m_eCurStatus;

	DWORD finalPos;
	if(absolutePos)
	{
		assert(0 <= position);
		finalPos = SetFilePointer(m_handle, position, NULL, FILE_BEGIN);
	}
	else
	{
		//assert(GetPosition() >= (DWORD)abs((long)position )
		finalPos = SetFilePointer(m_handle, position, NULL, FILE_CURRENT);
	}

	if(0xFFFFFFFF == finalPos)
		return SetStatus();
	else if(finalPos >= GetSize())
		return m_eCurStatus;
	else
		return m_eCurStatus = Ok;
}

DWORD CWin32File::GetPosition() const
{
	assert(Closed != m_eCurStatus);
	assert(INVALID_HANDLE_VALUE != m_handle);

	return SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);
}

DWORD CWin32File::GetSize() const 
{
	assert(INVALID_HANDLE_VALUE != m_handle);
	if(Ok == m_eCurStatus || EOS == m_eCurStatus)
	{
		DWORD high;
		return GetFileSize(m_handle, &high);
	}
	else 
		return 0;
}