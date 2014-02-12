// ListPakFile.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "precompile.h"
#include "QtDialog.h"

int _tmain(int argc, _TCHAR* argv[])
{
	QApplication app(argc, argv);


	QtListDialog dialog;
	dialog.show();


	return app.exec();
}

