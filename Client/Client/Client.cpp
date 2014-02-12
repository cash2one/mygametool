// Client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../ztPakage/GrpFile.h"
#include "../ztPakage/PakFile.h"

int _tmain(int argc, _TCHAR* argv[])
{

	CPakFile pak;
	pak.Open("C:\\Users\\fuzhun\\Desktop\\datas1.pak");

	pak.CompressFile();


	system("pause");
	return 0;
}

