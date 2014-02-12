#pragma once

#include "P2PService.h"
#include "HttpDowner.h"
#include "thread.h"

#include <map>
#include <string>
#include <vector>
#include <set>

#include <fstream>

//接收WM_COPYDATA消息
void OnCopyData(LPARAM lParam);


class HttpDowner;

//后台更新核心类
class CP2pServer
{
	friend unsigned int DownloadProcT( void* pParam);

public:
	CP2pServer();

	//有client链接
	void OnConnect(void* pdata);

	//插入一条下载资源
	//void InsertResItem(HWND hwnd, void* pdata);

	//立即下载某条资源
	void ImmediatelyDown(void* pdata);

	//失去一个连接(_p2p_release)
	void OnRelease(HWND hwnd);
	
	//给client发送消息
	void SendMessageToClient(HWND hwnd, const stClientResItem* msg);

	//开始下载
	void BeginDownload();

private:
	
	//下载线程函数
	unsigned int DownloadProc();

	//输出信息
	void PrintDebugInfo(const char* pszInfo);

private:

	//p2p后台配置
	stP2PConfig		m_config;

	//当前连接上p2pserver的进程HWND列表【connct +1, release -1】
	std::set<HWND>	m_setConHWND;
	
	//下载线程
	CThread			m_downloadThread;

	//线程是否已经在下载
	bool			m_bDownload;

	//http下载
	HttpDowner*	m_pDowner;

	//需要下载的客户端请求列表
	typedef std::map<HWND, std::vector<stClientResItem> >	MapResItemList;
	MapResItemList	m_mapClientRequestList;
	MapResItemList	m_mapTmpList;


private:
	//下客户端请求文件
	void DownloadClientRequestList();
	
	void AddClientReqList();

	//程序退出
	void QuitSelf();
	
};



inline CP2pServer& GetP2pServer()
{
	static CP2pServer server;
	return server;
}