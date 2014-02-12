//*******************************************************************************
// 版权所有(C) 2013 All right reserved
// 文件名称 : SqliteCommand.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2013年1月15日
// 说    明 : 
//*******************************************************************************
#include "SqliteCommand.h"

CSqliteCommand::CSqliteCommand(sqlite3* pSqliteHandle)
:m_pSqliteHandle(pSqliteHandle)
,m_pSqliteStatement(NULL)
{
}

CSqliteCommand::~CSqliteCommand(void)
{
	this->Clear();
}

//compile an sql statement
bool CSqliteCommand::Compile(const String& strSql, CValueTable* pRetValue)
{
	if(!m_pSqliteHandle || strSql.empty())
		return false;

	this->Clear();
	m_pRetValueTable = pRetValue;
	
	int err = sqlite3_prepare(m_pSqliteHandle, strSql.c_str(), -1, &m_pSqliteStatement, 0);
	if(err != SQLITE_OK)
	{
		return false;
	}
	
	assert( NULL != m_pSqliteStatement);
	
	if(m_pRetValueTable)
	{
		int numRetColums = sqlite3_column_count(m_pSqliteStatement);
		if(numRetColums > 0)
		{
			for(int index = 0; index < numRetColums; ++index)
			{
				String strAttr = sqlite3_column_name(m_pSqliteStatement, index);
				m_pRetValueTable->AddRowAttri(strAttr);
			}
		}
	}
	
	return true;
	
}

//excute
bool CSqliteCommand::Excute()
{
	assert(this->IsValid());

	bool done = false;
	while(!done)
	{
		int result = sqlite3_step(m_pSqliteStatement);
		if(SQLITE_DONE == result)
		{
			//all ok and done
			done = true;
		}
		else if(SQLITE_BUSY == result)
		{
			// somebody else is currently locking the database, sleep a very little while
			::Sleep(0.1);
		}
		else if(SQLITE_ROW == result)
		{
			if(m_pRetValueTable)
			{
				this->ReadRow();
			}
			done = true;
		}
		else if(SQLITE_ERROR == result)
		{
			return false;
		}
		else if(SQLITE_MISUSE == result)
		{
			MessageBox(0, "Sqlite3 Library Used Incorrect!", 0, 0);
			return false;
		}
		else
		{
			MessageBox(0, "Unknow Error in Sqlite excution!",0,0);
			return false;
		}
	}

	// reset the command, this clears the bound values
	int err = sqlite3_reset(m_pSqliteStatement);

	return true;
}

/// add a new row of results
void CSqliteCommand::ReadRow()
{
	assert(NULL != m_pSqliteHandle);
	assert(NULL != m_pSqliteStatement);
	assert(NULL != m_pRetValueTable);

	const int numRetColumns = sqlite3_data_count(m_pSqliteStatement);
	for(int index = 0; index < numRetColumns; ++index)
	{
		String strAttr = sqlite3_column_name(m_pSqliteStatement, index);
		int sqliteColumnType = sqlite3_column_type(m_pSqliteStatement, index);
		switch (sqliteColumnType)
		{
		case SQLITE_INTEGER:
			{
				int value = sqlite3_column_int(m_pSqliteStatement, index);
				m_pRetValueTable->SetInt(strAttr, value);
			}
			break;
		case SQLITE_FLOAT:
			{
				double value = sqlite3_column_double(m_pSqliteStatement, index);
				m_pRetValueTable->SetFloat(strAttr, value);
			}
			break;
		case SQLITE_TEXT:
			{
				const char* pValue = (const char*)sqlite3_column_text(m_pSqliteStatement, index);
				DWORD length = strlen(pValue);
				
				m_pRetValueTable->SetText(strAttr, pValue, length);
			}
			break;
		case SQLITE_BLOB:
			{
				const void* ptr = sqlite3_column_blob(m_pSqliteStatement, index);
				DWORD size = sqlite3_column_bytes(m_pSqliteStatement, index);

				m_pRetValueTable->SetBlob(strAttr, ptr, size);
			}
			break;
		default:
			{
				MessageBox(0, "目前没有用过的类型" , 0, 0);
			}
			break;
		}
	}
	
}

//get the attribute index
int CSqliteCommand::IndexOf(const String& strAttr)
{
	if(m_pSqliteStatement)
	{
		int index = sqlite3_bind_parameter_index(m_pSqliteStatement,
			avar(":%s", strAttr.c_str()));

		if(index != 0)
		{
			return index ;
		}
	}
	return -1;
}

/// set current error string to SQLite's error message
void CSqliteCommand::SetSqliteError()
{
	if(m_pSqliteHandle)
	{
		m_strErrorMsg = sqlite3_errmsg(m_pSqliteHandle);
	}
}


//return true if command is compiled and ready for excution
bool CSqliteCommand::IsValid()
{
	if(NULL != m_pSqliteStatement)
	{
		if( 0 == sqlite3_expired(m_pSqliteStatement))
			return true;
	}
	//needs to be compiled
	return false;
}

//clear
void CSqliteCommand::Clear()
{
	if (0 != m_pSqliteStatement)
	{
		sqlite3_finalize(m_pSqliteStatement);
		m_pSqliteStatement = 0;
	}
	m_pRetValueTable = 0;
}


void CSqliteCommand::BindInt(const String attr, int value)
{
	assert( NULL != m_pSqliteStatement);

	int index = this->IndexOf(attr);
	assert( -1 != index);

	int err = sqlite3_bind_int(m_pSqliteStatement, index, value);
	assert( SQLITE_OK == err );
}

void CSqliteCommand::BindDouble(const String attr, double value)
{
	assert( NULL != m_pSqliteStatement);

	int index = this->IndexOf(attr);
	assert( -1 != index);

	int err = sqlite3_bind_double(m_pSqliteStatement, index, value);
	assert( SQLITE_OK == err );
}

void CSqliteCommand::BindText(const String attr, const char* pszText)
{
	assert( NULL != m_pSqliteStatement);

	int index = this->IndexOf(attr);
	assert( -1 != index);

	int err = sqlite3_bind_text(m_pSqliteStatement, index, pszText, -1, SQLITE_TRANSIENT);
	assert( SQLITE_OK == err );
}

void CSqliteCommand::BindBlob(const String attr, const void* pData, DWORD dwSize)
{
	assert( NULL != m_pSqliteStatement);

	int index = this->IndexOf(attr);
	assert( -1 != index);

	int err = sqlite3_bind_blob(m_pSqliteStatement, index, pData, dwSize, SQLITE_TRANSIENT);
	assert( SQLITE_OK == err );
}