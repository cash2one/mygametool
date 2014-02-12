#include "WindowsHelper.h"

#include<Tlhelp32.h>
#include<tchar.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

//ͨ������ID��ȡ���̾��
HANDLE CWindowsHelper::GetProcessHandle(DWORD dwProcessID)
{
	return ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
}

//ͨ����������ȥ���̾��
HANDLE CWindowsHelper::GetProcessHandle(LPCSTR pName)
{
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) 
		return NULL;

	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 
	{
		if (!_tcscmp(pe.szExeFile, pName)) 
		{
			CloseHandle(hSnapshot);
			return GetProcessHandle(pe.th32ProcessID);
		}
	}
	return NULL;
}


//ͨ��������ȡ�ý���ID
DWORD CWindowsHelper::GetProcessIdByName(LPCSTR pName)
{
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) 
		return NULL;

	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	std::string strSelfPath = GetModulePath();

	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe)) 
	{
		HANDLE handle = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, pe.th32ProcessID);
		std::string strOtherPath = GetModulePath(handle);

		if (!_tcscmp(pe.szExeFile, pName)
			&& strSelfPath == strOtherPath) 
		{
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
	}
	return 0;
}

//ͨ������ID��ȡ���ھ��
HWND CWindowsHelper::GetHwndByPID(DWORD dwProcessID)
{
	HWND h = GetTopWindow(0);

	while ( h )
	{
		DWORD pid = 0;
		DWORD dwTheardId = GetWindowThreadProcessId( h,&pid);

		if (dwTheardId != 0)
		{
			if ( pid == dwProcessID)
			{
			// here h is the handle to the window
				return h;
			}
		}
		h = GetNextWindow( h , GW_HWNDNEXT);
	}

	return NULL;
}

//ͨ������ID��ȡ���ھ��
HWND CWindowsHelper::GetHwndByPID_Enum(DWORD dwProcessID)
{
// 	WINDOWINFO WinInfo[255];  
// 	HWND _hwnd=0;  
// 
// 	DWORD aa = EnumWindowInfo(WinInfo);
// 	for(int i=0;i<aa;i++)  
// 	{  
// 		if (WinInfo[i].dwProcessId==ProcessId)  
// 		{  
// 			_hwnd=WinInfo[i].hwnd;  
// 			if (0 ==::GetWindowLong(WinInfo1[i].hwnd1,GWL_HWNDPARENT))  
// 			{  
// 				return WinInfo[i].hwnd1;  
// 			}  
// 		}  
// 	}  
// 	return _hwnd;  
	return NULL;
}


//�ص�������

typedef struct tagWNDINFO 
{ 
	DWORD dwProcessId; 
	HWND hWnd; 
} WNDINFO, *LPWNDINFO; 

BOOL CALLBACK myEnumProc(HWND hWnd,LPARAM lParam) 
{ 
	DWORD dwProcessId; 
	GetWindowThreadProcessId(hWnd, &dwProcessId); 
	LPWNDINFO pInfo = (LPWNDINFO)lParam; 
	if(dwProcessId == pInfo->dwProcessId) 
	{ 
		pInfo->hWnd = hWnd; 
		return FALSE; 
	} 
	return TRUE; 
} 

//ͨ������ID��ȡ���ھ��
HWND CWindowsHelper::GetHwndByPID_Callback(DWORD dwProcessID)
{
	WNDINFO wi; 
	wi.dwProcessId = dwProcessID; 
	wi.hWnd = NULL; 
	EnumWindows(myEnumProc,(LPARAM)&wi); 
	return wi.hWnd; 
}


//�õ����̵�ǰĿ¼��GetCurrentDirectory��
std::string CWindowsHelper::GetCurrentDirectoy()
{
	char buffer[MAX_PATH] = {0};
	::GetCurrentDirectory(MAX_PATH, buffer);
	return buffer;
}

//�õ���ǰ�����ļ�������Ŀ¼��via GetModuleFileName��
std::string CWindowsHelper::GetModulePath()
{
	char buffer[MAX_PATH] = {0};
	::GetModuleFileName(NULL, buffer, MAX_PATH);
	char* p = strrchr(buffer, '\\');
	if(p)	*p = '\0';
	return buffer;
}


//�õ������������е�����Ŀ¼
std::string CWindowsHelper::GetModulePath(HANDLE handle)
{
	char buffer[MAX_PATH] = {0};
	::GetModuleFileNameEx(handle, NULL, buffer, MAX_PATH);
	char* p = strrchr(buffer, '\\');
	if(p)	*p = '\0';
	return buffer;
}
std::string CWindowsHelper::GetProcessImageFilePath(HANDLE handle)
{
	char buffer[MAX_PATH] = {0};
	::GetProcessImageFileName(handle, buffer, MAX_PATH);
	char* p = strrchr(buffer, '\\');
	if(p)	*p = '\0';
	return buffer;
}

//�ļ����Ƿ��Ѵ���
bool CWindowsHelper::IsDirExist(const char* path)
{
	DWORD dwFA = ::GetFileAttributes(path);
	return ((dwFA != 0xffffffff) && (dwFA & FILE_ATTRIBUTE_DIRECTORY));
}

//�ļ��Ƿ��Ѿ�����
bool CWindowsHelper::IsFileExist(std::string& path)
{
	DWORD dwFA = ::GetFileAttributes(path.c_str());
	return ((dwFA != INVALID_FILE_ATTRIBUTES) && (dwFA & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

//�����ļ���
void CWindowsHelper::CreateDirectory(std::string& path)
{
	std::string dir(path, 0, path.find_last_of("\\"));
	//CreateDirectory���ܴ������Ŀ¼
	const char* pCh = dir.c_str();
	if(!IsDirExist(pCh))
	{
		::CreateDirectory(pCh, NULL);
	}
	char buffer[MAX_PATH] = {0};
	for(int i = 0; i < strlen(pCh); ++i)
	{
		memset(buffer, 0, MAX_PATH);
		strcpy(buffer, pCh);
		if(pCh[i] == '\\')
		{
			buffer[i] = '\0';
			if(!IsDirExist(buffer))
			{
				::CreateDirectory(buffer, NULL);
			}
		}
	}
	bool bRet = ::CreateDirectory(dir.c_str(), 0);
	DWORD error = ::GetLastError();
	
}

//���ĵ�ǰ����Ŀ¼
void CWindowsHelper::SetCurrentDirectory(std::string& path)
{
	//��������path��һ���ļ��� ��Ҫ���ļ���ȥ��
	std::string dir(path, 0, path.find_last_of("\\"));
	//dir = GetModulePath() + "\\" + dir;
	CreateDirectory(dir);
	//::SetCurrentDirectory(dir.c_str());
}

//ͨ���ļ�·���õ��ļ����� �� data/interface/11.grp �õ� 11.grp��
std::string CWindowsHelper::GetFileNameInDir(std::string& path)
{
	return std::string(path, path.find_last_of("\\"), std::string::npos);
}


//�淶�ļ�·���� �� data/interface\11.grp װ��Ϊ data\interfaces\11.grp��
void CWindowsHelper::StandardizeDir(std::string& path)
{
	char pBuffer[MAX_PATH] = {0};
	_snprintf(pBuffer, MAX_PATH, path.c_str());
	char* p = pBuffer;
	while(p && *p)
	{
		if(*p == '/') *p = '\\';
		++p;
	}

	path = pBuffer;
}