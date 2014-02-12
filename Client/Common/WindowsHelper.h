#pragma once

#include<windows.h>
#include<string>

class CWindowsHelper
{
public:
	//ͨ������ID��ȡ���̾��
	static HANDLE GetProcessHandle(DWORD dwProcessID);

	//ͨ����������ȥ���̾��
	static HANDLE GetProcessHandle(LPCSTR pName);

	//ͨ��������ȡ�ý���ID
	static DWORD GetProcessIdByName(LPCSTR pName);

	//ͨ������ID��ȡ���ھ��
	static HWND GetHwndByPID(DWORD dwProcessID);
	
	//ͨ������ID��ȡ���ھ��
	static HWND GetHwndByPID_Enum(DWORD dwProcessID);

	//ͨ������ID��ȡ���ھ��
	static HWND GetHwndByPID_Callback(DWORD dwProcessID);


	//�õ����̵�ǰĿ¼��GetCurrentDirectory��
	static std::string GetCurrentDirectoy();

	//�õ���ǰ�����ļ�������Ŀ¼��via GetModuleFileName��
	static std::string GetModulePath();

	//�õ������������е�����Ŀ¼
	static std::string GetModulePath(HANDLE handle);
	static std::string GetProcessImageFilePath(HANDLE handle);

	//�����ļ���
	static void CreateDirectory(std::string& path);

	//���ĵ�ǰ����Ŀ¼
	static void SetCurrentDirectory(std::string& path);

	//�ļ����Ƿ��Ѵ���
	static bool IsDirExist(const char* path);

	//�ļ��Ƿ��Ѿ�����
	static bool IsFileExist(std::string& path);

	//ͨ���ļ�·���õ��ļ����� �� data\interface\11.grp �õ� 11.grp��
	static std::string GetFileNameInDir(std::string& path);

	//�淶�ļ�·���� �� data/interface\11.grp װ��Ϊ data\interfaces\11.grp��
	static void StandardizeDir(std::string& path);

	//ͨ�����ھ���õ����ڱ���
	static std::string GetWindowTitle(HWND hwnd);
};
