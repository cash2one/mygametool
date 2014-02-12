//*******************************************************************************
// 版权所有(C) 2013 All right reserved
// 文件名称 : SqliteCommand.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2013年1月15日
// 说    明 : 
//*******************************************************************************
#pragma once


#include "sqlite3.h"
#include "ValueTable.h"

class CSqliteCommand
{
public:
	CSqliteCommand(sqlite3* pSqliteHandle);
	~CSqliteCommand(void);

	//compile an sql statement
	bool Compile(const String& strSql, CValueTable* pRetValue);
	//excute
	bool Excute();
	//clear
	void Clear();
	//return true if command is compiled and ready for excution
	bool IsValid();


	//bind data

	void BindInt(const String attr, int value);
	void BindDouble(const String attr, double value);
	void BindText(const String attr, const char* pszText);
	void BindBlob(const String attr, const void* pData, DWORD dwSize);

private:
	/// set current error string to SQLite's error message
	void SetSqliteError();
	/// add a new row of results
	void ReadRow();
	//get the attribute index
	int IndexOf(const String& strAttr);

private:

	//sqlite3 handle
	sqlite3*		m_pSqliteHandle;

	sqlite3_stmt*	m_pSqliteStatement;

	//store the return values
	CValueTable*	m_pRetValueTable;

	//error message
	String			m_strErrorMsg;

};
