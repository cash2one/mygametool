#pragma once

#include "P2PService.h"
#include "HttpDowner.h"
#include "thread.h"

#include <map>
#include <string>
#include <vector>
#include <set>

#include <fstream>

//����WM_COPYDATA��Ϣ
void OnCopyData(LPARAM lParam);


class HttpDowner;

//��̨���º�����
class CP2pServer
{
	friend unsigned int DownloadProcT( void* pParam);

public:
	CP2pServer();

	//��client����
	void OnConnect(void* pdata);

	//����һ��������Դ
	//void InsertResItem(HWND hwnd, void* pdata);

	//��������ĳ����Դ
	void ImmediatelyDown(void* pdata);

	//ʧȥһ������(_p2p_release)
	void OnRelease(HWND hwnd);
	
	//��client������Ϣ
	void SendMessageToClient(HWND hwnd, const stClientResItem* msg);

	//��ʼ����
	void BeginDownload();

private:
	
	//�����̺߳���
	unsigned int DownloadProc();

	//�����Ϣ
	void PrintDebugInfo(const char* pszInfo);

private:

	//p2p��̨����
	stP2PConfig		m_config;

	//��ǰ������p2pserver�Ľ���HWND�б�connct +1, release -1��
	std::set<HWND>	m_setConHWND;
	
	//�����߳�
	CThread			m_downloadThread;

	//�߳��Ƿ��Ѿ�������
	bool			m_bDownload;

	//http����
	HttpDowner*	m_pDowner;

	//��Ҫ���صĿͻ��������б�
	typedef std::map<HWND, std::vector<stClientResItem> >	MapResItemList;
	MapResItemList	m_mapClientRequestList;
	MapResItemList	m_mapTmpList;


private:
	//�¿ͻ��������ļ�
	void DownloadClientRequestList();
	
	void AddClientReqList();

	//�����˳�
	void QuitSelf();
	
};



inline CP2pServer& GetP2pServer()
{
	static CP2pServer server;
	return server;
}