#pragma once

#include "HttpDowner.h"

class DataStream;

class HttpGetService : public WinGetHttpRequest
{
public:
	HttpGetService(DataStream* pOutStream);
	~HttpGetService(void);

	//接收文件
	virtual void OnReceiveContent(void* pBuffer,DWORD dwSize);

	//文件接收结束
	virtual void OnReceiveContentEnd();

private:
	DWORD m_dwReceiveSize;

	DataStream*	m_pOutStream;
};
