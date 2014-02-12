#include "public.h"
#include "HttpGetService.h"
#include "DataStream.h"


HttpGetService::HttpGetService(DataStream* pOutStream)
:m_pOutStream(pOutStream)
,m_dwReceiveSize(0)
{
}

HttpGetService::~HttpGetService(void)
{
}

//�����ļ�
void HttpGetService::OnReceiveContent(void* pBuffer,DWORD dwSize)
{
	if(m_pOutStream)
	{
		m_pOutStream->Write(pBuffer,dwSize);
		m_dwReceiveSize += dwSize;
	}
}

//�ļ����ս���
void HttpGetService::OnReceiveContentEnd()
{
}