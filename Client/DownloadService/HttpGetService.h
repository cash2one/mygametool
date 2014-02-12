#pragma once

#include "HttpDowner.h"

class DataStream;

class HttpGetService : public WinGetHttpRequest
{
public:
	HttpGetService(DataStream* pOutStream);
	~HttpGetService(void);

	//�����ļ�
	virtual void OnReceiveContent(void* pBuffer,DWORD dwSize);

	//�ļ����ս���
	virtual void OnReceiveContentEnd();

private:
	DWORD m_dwReceiveSize;

	DataStream*	m_pOutStream;
};
