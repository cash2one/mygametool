//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : P2PService.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��10��
// ˵    �� : p2p��client��p2pserver.exe(�ݶ�)�Ľ����ӿ�
//*******************************************************************************

#pragma once

#ifndef P2P_MAX_PATH
#define P2P_MAX_PATH 260
#endif

#include <string>

//////////////////////////////////////////////////////////////////////////
//���ݽṹ

//p2p_connect���������ú�̨����
struct stP2PConfig
{
	char		workpath[P2P_MAX_PATH];			//��ǰ���г����Ŀ¼
	char		remoteurl[P2P_MAX_PATH];		//Զ�����ط�����ǰ���·��,����http://updateztmfnew02.ztgame.com.cn/autopatchztnew02/
	char		backupurl[P2P_MAX_PATH];		//����Զ������·����û����Ϊ��
	HWND		hWnd;							//��Ϸ���ھ��
	bool		autoexit;						//û�н������ӵ�p2pserver.exe���Ƿ��Զ��˳�
};

//ע��һ����Դ��Ϣ
struct stP2PResItem
{
	char	respath[P2P_MAX_PATH];		//��Դ����·��(���·��)���ر�������zhengtu.exe, data/interface/0000.grp��
	char	remotepath[P2P_MAX_PATH];	//��Դ���������·�����ر�������zhengtu.exe.co, data/interface.pak/interface/0000.grp.co��
	char	respak[P2P_MAX_PATH];		//��Դ���ش���ĵ��ļ�,Ϊ���򲻴������ interfece.pak, zhengtu.exe��
	bool	must;						//�Ƿ��ڿͻ�������֮ǰ�������أ��ر�
	DWORD	size;						//��Դ��С���ر�
	DWORD	hash;						//У��[crc or md5]
	DWORD	realsize;					//��ѹ������ļ���С
	DWORD	comsize;					//ѹ������ļ���С
};

//�ͻ�������ĳ����Դ��ǰ���أ�ֻ����Դ������
//ע����Դ�� ����client����֮ǰp2p�Ѿ�ע���p2pserver.exe
struct stClientResItem
{
	HWND	myhwnd;						//�Լ���hwnd���������سɹ������client
	char	respath[P2P_MAX_PATH];		//���ݿ�������interfaces��
	DWORD	dwGroup;					//dwGroup.grp
};

enum P2P_COMMAND_TYPE
{
	//������Ϸ��̨����p2pserver.exe   IN: P2P_CONFIG �� OUT: NULL
	_p2p_connect,

	//������Դ�б�p2p����ʱֻ����һ�Ρ�ÿ����һ�ξͲ���һ�������б�
	//IN: 
	_p2p_checkupdate,

	//��������ĳ����Դ��client��Ҫĳ����Դ��ǰ���ء�
	//IN : P2P_RES_ITEM*, OUT: NULL
	_p2p_immediately_down,

	//֪ͨp2pserver.exe��ʼ��̨���أ�Ŀǰ��client����ʱ��֪ͨ
	_p2p_begindownload,

	//���߿ͻ���ĳ��grp�Ѿ����سɹ�
	_p2p_alreadydown,

	//��p2pserver�Ͽ�����
	_p2p_release,

};

//������ϻص�����
typedef void (__stdcall *__pfnOnAlreadyDownFunc)(const std::string& strpak, DWORD dwGoup);

//////////////////////////////////////////////////////////////////////////
class CP2PService
{
public:
	CP2PService(void);
	~CP2PService(void);

	//��ʼ������ʼ���ɹ����ܽ����Ժ�Ĳ�����
	bool Init();

	//���ӵ�p2pserver.exe
	void ConnectP2pServer(stP2PConfig* pConfig);

	//p2p��p2pserver.exe���һ����¼ ��p2pʹ��-------Ŀǰ�汾����Ҫ��
	void RegisterAResItem(stP2PResItem* pItem);

	//�ͻ�������δ���ص���Դ, ֪ͨp2pserver��������
	void RequestRes(stClientResItem* pItem);

	//֪ͨp2pserver��ʼ��̨���ء�clientʹ�á�
	void BeginDownload();

	//�˳�p2pserver���ӡ�p2p,client�˳�ʱ���á�
	void Release(HWND myHwnd);

	//CP2PService�Ƿ���Ч
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
