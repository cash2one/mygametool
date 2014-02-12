//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : Win32File.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月18日
// 说    明 : win32 file class
//*******************************************************************************
#pragma once

class CWin32File
{
public:

	//the status of our file handle
	enum Status
	{
		Ok,				//ok
		IOError,		//Read or Write error
		EOS,			//End of ths stream
		IllegalCall,
		Closed,
		UnknowError,
	};

	enum AccessMode
	{
		eRead,
		eWrite,
		eReadWrite,
		eWriteAppend,
	};

	enum Capacity
	{
		FileRead       = 1<<0,
		FileWrite	   = 1<<1
	};
public:
	CWin32File(void);
	~CWin32File(void);
	
	Status Open(const char* filename, const AccessMode openmode, bool randomAccess = false);
	Status Close();
	Status Read(void* dst, DWORD dwSize, DWORD* bytesRead);
	Status Write(const void* src, DWORD dwSize, DWORD* bytesWrite);

	DWORD GetPosition() const;
	Status SetPosition(DWORD position, bool absolutePos);
	DWORD GetSize() const;

private:
	Status	SetStatus();
	Status	SetStatus(Status status){ return m_eCurStatus = status; }
	bool HasCapacity(Capacity cap) const;
private:
	HANDLE	m_handle;
	Status	m_eCurStatus;
	DWORD	m_dwCapacity;
};
