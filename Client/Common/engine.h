//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : engine.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月12日
// 说    明 : 
//*******************************************************************************

#pragma once

#include <time.h>


enum EngineLogLevel
{
	LogLevelDebug,
	LogLevelTest,
	LogLevelRelease,
};

inline void __Engine_WriteLog(const char* szString)
{
	time_t ti = time(NULL);
	tm* t = localtime(&ti);
	char szTime[MAX_PATH] = {0};
	strftime(szTime, MAX_PATH, "[%y-%m-%d %H:%m:%S]", t);

#if defined _DEBUG
	std::cout<< szTime <<std::endl;
#endif
}

inline void Engine_WriteLog(const char* szString, EngineLogLevel level = LogLevelDebug)
{
#ifdef _DEBUG
	__Engine_WriteLog(szString);
#else
	if(level > LogLevelTest)
		__Engine_WriteLog(szString);
#endif
}

inline void Engine_WriteLogV(const char* szFmt, va_list argptr, EngineLogLevel level = LogLevelDebug)
{
	char szString[1024] = {0};
	_vsnprintf(szString, 1024, szFmt, argptr);
	szString[1023] = 0;
	Engine_WriteLog(szString, level);
}

//debug
inline void System_LogDebug(const char* szFmt, ...)
{
#ifdef _DEBUG
	va_list args;
	va_start( args, szFmt);
	Engine_WriteLogV(szFmt, args, LogLevelDebug);
	va_end(args);
#endif
}

//release
inline void System_LogRelease(const char* szFmt, ...)
{
	va_list args;
	va_start( args, szFmt);
	Engine_WriteLogV(szFmt, args, LogLevelDebug);
	va_end(args);
}