//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : P2PService.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月10日
// 说    明 : 
//*******************************************************************************
#include "precompile.h"
#include <assert.h>
#include "P2PService.h"
#include "WindowsHelper.h"

__pfnOnAlreadyDownFunc CP2PService::ms_pAlreadyDownFunc = NULL;
std::string	CP2PService::ms_downloadExeName = "DownloadService.exe";


CP2PService::CP2PService(void)
:m_pHandle(NULL)
{
}

CP2PService::~CP2PService(void)
{
}

//初始化
bool CP2PService::Init()
{
	m_pHandle = NULL;
	
	return InnerInit();
}

bool CP2PService::InnerInit()
{
	//根据进程名和进程所在目录找
	DWORD processid	= CWindowsHelper::GetProcessIdByName(ms_downloadExeName.c_str());
	bool bRet = true;

	if(0 == processid)
	{
		STARTUPINFO sInfo;
		PROCESS_INFORMATION pInfo;
		::ZeroMemory(&sInfo,sizeof(sInfo));
		sInfo.cb=sizeof(sInfo);
		::ZeroMemory(&pInfo,sizeof(pInfo));

		std::string strPath = CWindowsHelper::GetModulePath();
		strPath += "\\";
		strPath += ms_downloadExeName;

		//开启p2pserver.exe进程
		bRet = ::CreateProcess( strPath.c_str(),NULL, NULL, NULL, 
			true, 0, NULL,NULL,&sInfo,&pInfo);

		processid = pInfo.dwProcessId;
	}

	if(bRet && processid != 0)
	{
		//while(!m_pHandle)
		{
			//m_pHandle =  CWindowsHelper::GetHwndByPID(processid);
			m_pHandle = CWindowsHelper::GetHwndByPID_Callback(processid);
			//::Sleep(10);
		}
	}
	

	return m_pHandle != NULL;
}

//链接到p2pserver.exe
void CP2PService::ConnectP2pServer(stP2PConfig* pConfig)
{
	assert(m_pHandle);

	COPYDATASTRUCT cds; 
	cds.dwData = _p2p_connect; 
	cds.cbData = sizeof(stP2PConfig); 
	cds.lpData = pConfig; 
	::SendMessage((HWND)m_pHandle, WM_COPYDATA, 0, (LPARAM)&cds);
}

//p2p给p2pserver.exe添加一条记录 【p2p使用】
void CP2PService::RegisterAResItem(stP2PResItem* pItem)
{
	assert(m_pHandle);

	COPYDATASTRUCT cds; 
	cds.dwData = _p2p_checkupdate; 
	cds.cbData = sizeof(stP2PResItem); 
	cds.lpData = pItem; 
	::SendMessage((HWND)m_pHandle, WM_COPYDATA, 0, (LPARAM)&cds);
}

//客户端请求未下载的资源, 通知p2pserver立即下载
void CP2PService::RequestRes(stClientResItem* pItem)
{
	assert(m_pHandle);

	COPYDATASTRUCT cds; 
	cds.dwData = _p2p_immediately_down; 
	cds.cbData = sizeof(stClientResItem); 
	cds.lpData = pItem; 
	::SendMessage((HWND)m_pHandle, WM_COPYDATA, 0, (LPARAM)&cds);
}

//通知p2pserver开始后台下载【client使用】
void CP2PService::BeginDownload()
{
	assert(m_pHandle);

	COPYDATASTRUCT cds; 
	cds.dwData = _p2p_begindownload; 
	cds.cbData = 0;
	cds.lpData = 0; 
	::SendMessage((HWND)m_pHandle, WM_COPYDATA, 0, (LPARAM)&cds);
}

//退出p2pserver链接【p2p,client退出时调用】
void CP2PService::Release(HWND myHwnd)
{
	assert(m_pHandle);

	COPYDATASTRUCT cds; 
	cds.dwData = _p2p_release; 
	cds.cbData = (DWORD)myHwnd;
	cds.lpData = 0; 
	::SendMessage((HWND)m_pHandle, WM_COPYDATA, 0, (LPARAM)&cds);
}

void CP2PService::PreTranslateMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_COPYDATA)
	{
		COPYDATASTRUCT* pdata =  (COPYDATASTRUCT*)(lParam);
		if(pdata && ms_pAlreadyDownFunc)
		{
			stClientResItem item;
			memset(&item, 0, sizeof(item));
			memcpy(&item, pdata->lpData, sizeof(item));

			ms_pAlreadyDownFunc(item.respath, item.dwGroup);
		}
	}
}