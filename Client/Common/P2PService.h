//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : P2PService.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月10日
// 说    明 : p2p和client与p2pserver.exe(暂定)的交互接口
//*******************************************************************************

#pragma once

#ifndef P2P_MAX_PATH
#define P2P_MAX_PATH 260
#endif

#include <string>

//////////////////////////////////////////////////////////////////////////
//数据结构

//p2p_connect参数，配置后台进程
struct stP2PConfig
{
	char		workpath[P2P_MAX_PATH];			//当前运行程序的目录
	char		remoteurl[P2P_MAX_PATH];		//远程下载服务器前半段路径,比如http://updateztmfnew02.ztgame.com.cn/autopatchztnew02/
	char		backupurl[P2P_MAX_PATH];		//备份远程下载路径，没有设为空
	HWND		hWnd;							//游戏窗口句柄
	bool		autoexit;						//没有进程链接到p2pserver.exe，是否自动退出
};

//注册一个资源信息
struct stP2PResItem
{
	char	respath[P2P_MAX_PATH];		//资源本地路径(相对路径)，必备【比如zhengtu.exe, data/interface/0000.grp】
	char	remotepath[P2P_MAX_PATH];	//资源服务器相对路径，必备【例如zhengtu.exe.co, data/interface.pak/interface/0000.grp.co】
	char	respak[P2P_MAX_PATH];		//资源本地打包的的文件,为空则不打包【如 interfece.pak, zhengtu.exe】
	bool	must;						//是否在客户端启动之前必须下载，必备
	DWORD	size;						//资源大小，必备
	DWORD	hash;						//校验[crc or md5]
	DWORD	realsize;					//解压缩后的文件大小
	DWORD	comsize;					//压缩后的文件大小
};

//客户端请求某个资源提前下载，只需资源名即可
//注意资源名 需在client启动之前p2p已经注册给p2pserver.exe
struct stClientResItem
{
	HWND	myhwnd;						//自己的hwnd，方便下载成功后告诉client
	char	respath[P2P_MAX_PATH];		//数据库名【如interfaces】
	DWORD	dwGroup;					//dwGroup.grp
};

enum P2P_COMMAND_TYPE
{
	//链接游戏后台进程p2pserver.exe   IN: P2P_CONFIG ， OUT: NULL
	_p2p_connect,

	//更新资源列表，p2p启动时只调用一次【每调用一次就插入一条更新列表】
	//IN: 
	_p2p_checkupdate,

	//立即下载某项资源【client需要某个资源提前下载】
	//IN : P2P_RES_ITEM*, OUT: NULL
	_p2p_immediately_down,

	//通知p2pserver.exe开始后台下载，目前在client启动时候通知
	_p2p_begindownload,

	//告诉客户端某个grp已经下载成功
	_p2p_alreadydown,

	//与p2pserver断开链接
	_p2p_release,

};

//下载完毕回调函数
typedef void (__stdcall *__pfnOnAlreadyDownFunc)(const std::string& strpak, DWORD dwGoup);

//////////////////////////////////////////////////////////////////////////
class CP2PService
{
public:
	CP2PService(void);
	~CP2PService(void);

	//初始化【初始化成功才能进行以后的操作】
	bool Init();

	//链接到p2pserver.exe
	void ConnectP2pServer(stP2PConfig* pConfig);

	//p2p给p2pserver.exe添加一条记录 【p2p使用-------目前版本不需要】
	void RegisterAResItem(stP2PResItem* pItem);

	//客户端请求未下载的资源, 通知p2pserver立即下载
	void RequestRes(stClientResItem* pItem);

	//通知p2pserver开始后台下载【client使用】
	void BeginDownload();

	//退出p2pserver链接【p2p,client退出时调用】
	void Release(HWND myHwnd);

	//CP2PService是否有效
	bool IsValid(){ return m_pHandle != NULL; }

	static void SetCallBack(__pfnOnAlreadyDownFunc func) { ms_pAlreadyDownFunc = func; }

	static void PreTranslateMsg(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	bool InnerInit();

private:
	
	void*	m_pHandle;
	static __pfnOnAlreadyDownFunc	ms_pAlreadyDownFunc;
	static std::string					ms_downloadExeName;
};
