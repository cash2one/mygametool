#include "server.h"
#include <windows.h>
#include <algorithm>
#include "DataStream.h"
#include "WindowsHelper.h"
#include "SqliteDatabase.h"
#include "SystemUtil.h"
#include "HttpDowner.h"


#include "tool.h"
#include "Config.h"
#include "Flist.h"
#include "FlistManager.h"


#include "HttpGetService.h"
#include "EncDec.h"
#include <boost/thread/mutex.hpp> 
//#include "Config.h"
#include "Engine.h"


boost::mutex ItemInserMutex;

//接收WM_COPYDATA消息
void OnCopyData(LPARAM lParam)
{
	COPYDATASTRUCT* pCopyData = (COPYDATASTRUCT*)(lParam);
	if(!pCopyData)
		return;

	switch(pCopyData->dwData)
	{
	case _p2p_connect:
		{
			GetP2pServer().OnConnect(pCopyData->lpData);
		}
		break;
	case _p2p_immediately_down:
		{
			GetP2pServer().ImmediatelyDown(pCopyData->lpData);
		}
		break;
	case _p2p_begindownload:
		{
			GetP2pServer().BeginDownload();
		}
	case _p2p_release:
		{
			GetP2pServer().OnRelease((HWND)pCopyData->cbData);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
////////////////////////friend functions
//下载线程
unsigned int DownloadProcT( void* pParam)
{
	CP2pServer* pServer = (CP2pServer*)(pParam);
	if(pServer->m_downloadThread.IsEndThread())
		return 0;
	else
		return pServer->DownloadProc();
}


CP2pServer::CP2pServer()
:m_pDowner(NULL)
{
	memset(&m_config, 0, sizeof(m_config));
	m_bDownload = false;
}

//更新后台配置
void CP2pServer::OnConnect(void* pdata)
{
#ifdef _DEBUG
	MessageBox(0,"_p2p_connect success", "success",0);
#endif

	memcpy(&m_config, pdata, sizeof(m_config));
	m_setConHWND.insert((HWND)m_config.hWnd);

	HWND hwnd = m_config.hWnd;
	std::string strTitle = CWindowsHelper::GetWindowTitle(hwnd);
	PrintDebugInfo(avar("[HWND: 0x%08x] %s Connect Success!!", hwnd,strTitle.c_str()));
}


//立即下载某条资源
void CP2pServer::ImmediatelyDown(void* pdata)
{
	stClientResItem item;
	memcpy(&item, pdata, sizeof(stClientResItem));
	HWND hwnd = item.myhwnd;
	PrintDebugInfo(avar("收到Client[HWND: 0x%08x]请求", hwnd));

	boost::mutex::scoped_lock lock(ItemInserMutex);
	m_mapTmpList[hwnd].push_back(item);
}

//开始下载
void CP2pServer::BeginDownload()
{
	if(!m_bDownload)
	{
		m_bDownload = true;
		m_downloadThread.BeginThread( (CThread::t_start_address)&DownloadProcT, 0, (void*)this);
	}

	PrintDebugInfo(avar("********下载完成,后台下载进程自动退出********\n\n"));
	QuitSelf();
}


//失去一个连接(_p2p_release)
void CP2pServer::OnRelease(HWND hwnd)
{
	std::set<HWND>::iterator it = m_setConHWND.find(hwnd);
	if(it != m_setConHWND.end())
	{
		std::string strTitle = CWindowsHelper::GetWindowTitle(hwnd);
		PrintDebugInfo(avar("====[HWND: 0x%08x] %s 退出====", hwnd, strTitle.c_str()));

		m_setConHWND.erase(it);
		if(m_config.autoexit != 0 && m_setConHWND.empty())
		{
			//若连接数为0 自动退出
			PrintDebugInfo(avar("********连接数为0,后台下载进程自动退出********\n\n"));
			QuitSelf();
		}
	}
}

//下载线程函数
unsigned int CP2pServer::DownloadProc()
{
	m_pDowner = new HttpDowner("192.168.122.168", 80);
	MemoryDataStream outstream;
	std::vector<BYTE> vecDst;

	//设置工作目录
	if(m_config.workpath[0] != 0 && strlen(m_config.workpath))
	{
		::SetCurrentDirectory(m_config.workpath);
	}

	//需要更新的列表
	GetFListManager()->LoadDifFlist(gc_pFlistDifName);
	const GameNeedPacks vecNeedDownList = GetFListManager()->GetGameDifPacksOfNotMust();

	GameNeedPacks::const_iterator it = vecNeedDownList.begin();
	for( ; it != vecNeedDownList.end(); ++it)
	{
		//本地pak名
		std::string strpak = //GetPackSavePath(it->m_strPackPath);
			avar("Image\\%s.ipk", it->m_strPackName.c_str());
		CWindowsHelper::CreateDirectory(strpak);
		DataBase* pTable = GetDBMgr().Open(strpak.c_str(), it->m_strPackName);
		if(!pTable)		
		{
			continue;
		}

		GameNeedPackItems::const_iterator grpItr = it->m_vecPackItems.begin();
		for( ; grpItr != it->m_vecPackItems.end(); ++grpItr)
		{
			//服务器文件名
			std::string strGrp = //GetPackItemDownloadPath(it->m_strPackPath, grpItr->m_dwPackItemID);
			avar("/Client/Image/%s/%d.grp", it->m_strPackName.c_str(),grpItr->m_dwPackItemID) ;

			if(m_pDowner->DownloadFile(strGrp, new HttpGetService(&outstream), true))
			{
				if(outstream.Size() == 0)
					continue;

				//解压数据
				vecDst.clear();
				vecDst.resize(outstream.Size());
				EncDec::LZMAUncompress(vecDst, (BYTE*)outstream.GetInnerMemory()[0], outstream.Size());

				//插入数据库
				stPackRowInfo rowinfo;
				rowinfo.m_dwId = grpItr->m_dwPackItemID;
				rowinfo.m_dwCrc = SystemUtil::GeneratorCRC32((BYTE*)&vecDst[0], vecDst.size());
				rowinfo.m_dwFlag = 1;
				rowinfo.m_dwSize = vecDst.size();
				pTable->Insert(rowinfo, (BYTE*)&vecDst[0]);

				//下载完成一个就去检测客户端有没有请求数据
				DownloadClientRequestList();
				outstream.Close();
				::Sleep(10);
			}
			
		}
	}

	static DWORD dwBegin = timeGetTime();
	while( timeGetTime() - dwBegin < 60*1000)
	{
		DownloadClientRequestList();
		::Sleep(10);
	}

	MessageBox(0, "下载完成", 0, 0);
	return 0 ;
}


//给client发送下载成功的消息
void CP2pServer::SendMessageToClient(HWND hwnd, const stClientResItem* pItem)
{
	COPYDATASTRUCT data;
	data.cbData = sizeof(stClientResItem);
	data.lpData = (void*)pItem;
	LRESULT ret = ::SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)&data);
}

//输出信息
void CP2pServer::PrintDebugInfo(const char* pszInfo)
{
#ifdef _DEBUG

	std::ofstream fd("dwonload_debuginfo.txt", std::ios_base::out | std::ios_base::app);
	fd << pszInfo <<std::endl;
	fd.flush();
	fd.close();

#endif
}


void CP2pServer::AddClientReqList()
{
	boost::mutex::scoped_lock lock(ItemInserMutex);
	MapResItemList::iterator it = m_mapTmpList.begin();
	for( ; it != m_mapTmpList.end(); ++it)
	{
		HWND hwnd = it->first;
		std::vector<stClientResItem>& vecItem = m_mapClientRequestList[hwnd];
		vecItem = it->second;
	}
	m_mapTmpList.clear();
}

void CP2pServer::DownloadClientRequestList()
{
	if(!m_pDowner)
		return;

	AddClientReqList();

	MemoryDataStream outstream;
	std::vector<BYTE> vecDst;

	MapResItemList::const_iterator cit = m_mapClientRequestList.begin();
	for( ; cit != m_mapClientRequestList.end(); ++cit)
	{
		HWND hwnd = cit->first;
		size_t count = cit->second.size();
		for(size_t index = 0; index < count; ++index)
		{
			const stClientResItem& item = cit->second[index];

			string strpak = //GetPackSavePath(item.respath);
				"Image\\" + std::string(item.respath) + ".ipk";

			CWindowsHelper::CreateDirectory(strpak);
			DataBase* pTable = GetDBMgr().Open(strpak.c_str(), item.respath);

			if(!pTable)		
				continue;

				//服务器文件名
			std::string strGrp = //GetPackItemDownloadPath(item.respath, item.dwGroup);
				avar("/Client/Image/%s/%d.grp", item.respath,item.dwGroup) ;
			if(m_pDowner->DownloadFile(strGrp, new HttpGetService(&outstream), true))
			{
				if(outstream.Size() == 0)
					continue;

				//解压数据
				vecDst.resize(outstream.Size());
				EncDec::LZMAUncompress(vecDst, (BYTE*)outstream.GetInnerMemory()[0], outstream.Size());

				//插入数据库
				stPackRowInfo rowinfo;
				rowinfo.m_dwId = item.dwGroup;
				rowinfo.m_dwCrc = SystemUtil::GeneratorCRC32((BYTE*)&vecDst[0], vecDst.size());
				rowinfo.m_dwFlag = 1;
				rowinfo.m_dwSize = vecDst.size();
				pTable->Insert(rowinfo, (BYTE*)&vecDst[0]);
				SendMessageToClient(hwnd, &item);

				outstream.Close();
			}
		}
	}

	m_mapClientRequestList.clear();

}


//程序退出
void CP2pServer::QuitSelf()
{
	SAFE_DELETE(m_pDowner);

	if(m_downloadThread.WaitEndThread())
	{
		DWORD dwCurProID = ::GetCurrentProcessId();
		HANDLE hProHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwCurProID);
		::TerminateProcess(hProHandle, 0);
	}
}