#pragma once

#include<windows.h>
#include<string>

class CWindowsHelper
{
public:
	//通过进程ID获取进程句柄
	static HANDLE GetProcessHandle(DWORD dwProcessID);

	//通过进程名后去进程句柄
	static HANDLE GetProcessHandle(LPCSTR pName);

	//通过进程名取得进程ID
	static DWORD GetProcessIdByName(LPCSTR pName);

	//通过进程ID获取窗口句柄
	static HWND GetHwndByPID(DWORD dwProcessID);
	
	//通过进程ID获取窗口句柄
	static HWND GetHwndByPID_Enum(DWORD dwProcessID);

	//通过进程ID获取窗口句柄
	static HWND GetHwndByPID_Callback(DWORD dwProcessID);


	//得到进程当前目录【GetCurrentDirectory】
	static std::string GetCurrentDirectoy();

	//得到当前进程文件的所在目录【via GetModuleFileName】
	static std::string GetModulePath();

	//得到其他进程运行的所在目录
	static std::string GetModulePath(HANDLE handle);
	static std::string GetProcessImageFilePath(HANDLE handle);

	//创建文件夹
	static void CreateDirectory(std::string& path);

	//更改当前工作目录
	static void SetCurrentDirectory(std::string& path);

	//文件夹是否已存在
	static bool IsDirExist(const char* path);

	//文件是否已经存在
	static bool IsFileExist(std::string& path);

	//通过文件路径得到文件名【 如 data\interface\11.grp 得到 11.grp】
	static std::string GetFileNameInDir(std::string& path);

	//规范文件路径【 如 data/interface\11.grp 装换为 data\interfaces\11.grp】
	static void StandardizeDir(std::string& path);

	//通过窗口句柄得到窗口标题
	static std::string GetWindowTitle(HWND hwnd);
};
