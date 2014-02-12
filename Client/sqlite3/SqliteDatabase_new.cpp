
#include "SqliteDatabase_new.h"
#include "SystemUtil.h"
#include "LogManager.h"

#include "ValueTable.h"
#include "SqliteCommand.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif


const stPackRowInfo stPackRowInfo::Invalid = stPackRowInfo();

//////////////////////////////////////////////////////////////////////////
int getColumnValue(  sqlite3_stmt* stmt,   int col,   void * value);

inline bool operator <(const DBKey& key1, const DBKey& key2)
{
	return key1.strKey1 < key2.strKey1
		|| key1.strKey2 < key2.strKey2;
}

SqliteDatabase::SqliteDatabase(void)
{
}

SqliteDatabase::~SqliteDatabase(void)
{
	CloseAll();
}

DataBase* SqliteDatabase::Open(const String& strDatabase,const String& strTable)
{
	DBMapItr it = m_mapDB.find(DBKey(strDatabase, strTable));
	if( it != m_mapDB.end() )
		return it->second;

	SystemUtil::MakeDir(strDatabase);

	DataBase* pDB = DataBase::Create();
	
	//创建表
	if(pDB->Open(strDatabase, strTable))
	{
		m_mapDB.insert(std::make_pair<DBKey, DataBase*>(DBKey(strDatabase, strTable), pDB));
		return pDB;
	}
	
	//创建失败
	DataBase::Destory(pDB);
	return NULL;
}

void SqliteDatabase::Close(DataBase* pDb)
{
	if(pDb)
	{
		m_mapDB.erase(DBKey(pDb->GetDatabaseName(),pDb->GetTableName()));

		DataBase::Destory(pDb);

		pDb = NULL;

		
	}
}

void SqliteDatabase::CloseAll()
{
	while(m_mapDB.size())
	{
		DataBase::Destory(m_mapDB.begin()->second);
		m_mapDB.erase(m_mapDB.begin());
	}
}

//////////////////////////////////////////////////////////////////////////
//创建一个DataBase对象
DataBase* DataBase::Create()
{
	return new DataBase();
}

	//销毁 DataBase对象
void DataBase::Destory(DataBase* pDB)
{
	if(pDB)
	{
		pDB->Close();
	}
	SAFE_DELETE(pDB);
}
//////////////////////////////////////////////////////////////////////////

//打开一个table
bool DataBase::Open(const String &strDatabase, const String &strTable)
{
	if(strDatabase.empty() || strTable.empty())
		return false;

	//创建或者打开数据库
	int ret = SQLITE_OK;
	ret = sqlite3_open(strDatabase.c_str(), &m_pDb);

	if(ret != SQLITE_OK)
	{
		m_pDb = NULL;
		return false;
	}

	//给数据库添加auto_vacuum命令
	//删除数据项时会压缩数据库空间【尽量少删除数据，效率比较低】
	const char* pszAutoVacuum = avar("PRAGMA auto_vacuum = 1");
	ret = sqlite3_exec(m_pDb, pszAutoVacuum, NULL, NULL, 0);
	if(ret != SQLITE_OK)
	{
		sqlite3_close(m_pDb);
		m_pDb = NULL;
		return false;
	}

	m_strDatabase = strDatabase;

	return OpenTable(strTable);
}


bool DataBase::OpenTable(const String& strTable)
{
	if(!IsValid())
		return false;
	if(!this->IsTableExist(strTable))
	{
		//创建表
		//sql: create table
		const char* pszCreateTable = avar("CREATE TABLE IF NOT EXISTS %s(id INTEGER PRIMARY KEY, crc INTEGER, size INTEGER, flag  INTEGER, data BLOB)", strTable.c_str());

		char* szError;
		int ret = SQLITE_OK;
		ret = sqlite3_exec(m_pDb, pszCreateTable, NULL, NULL, &szError);
		if(ret != SQLITE_OK)
		{
			sqlite3_free(szError);
			sqlite3_close(m_pDb);
			m_pDb = NULL;
			return false;
		}


		//id INTEGER PRIMARY KEY 已经创建索引，所以这里不用创建索引了
		////给表创建索引
		//const char* pszCreateIndex = avar("CREATE INDEX table_key ON %s( filename )", strTable.c_str());
		//ret = sqlite3_exec(m_pDb, pszCreateIndex, NULL, NULL, &szError);
		//if(ret != SQLITE_OK)
		//{
		//	sqlite3_close(m_pDb);
		//	m_pDb = NULL;
		//	return false;
		//}
	}

	m_strTable = strTable;

	return true;
}


//创建表之前先查一下表是否已经存在
//表已经存在就不用再创建索引了
bool DataBase::IsTableExist(const String& strTable)
{
	if(!IsValid())
		return false;

	//sql select count
	String strSelect = avar("SELECT COUNT(*) tableCnt FROM sqlite_master WHERE type = 'table' AND name='%s'", strTable.c_str());

	CSqliteCommand sqlCommand(m_pDb);
	CValueTable	retValue;
	sqlCommand.Compile(strSelect, &retValue);
	sqlCommand.Excute();
	
	if(retValue.GetInt(0, "tableCnt") >= 1)
		return true;
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
//DataBase插入数据
bool DataBase::Insert(const stPackRowInfo& rowInfo,void* pInBuffer)
{
	if(!IsValid())
		return false;

	if(this->IsRowExist(rowInfo.m_dwId))
	{
		//若数据已经存在，需要更新
		return UpdateRow(rowInfo,pInBuffer);
	}
	
	//计算CRC
	unsigned long crc = rowInfo.m_dwCrc;
	if(crc == 0)
	{
		crc = SystemUtil::GeneratorCRC32((BYTE*)pInBuffer, rowInfo.m_dwSize);
	}


	//sql insert table
	String strInsert = avar("INSERT INTO %s values(:id,:crc,:size,:flag,:data)", m_strTable.c_str());
	

	CSqliteCommand sqlCommand(m_pDb);

	sqlCommand.Compile(strInsert, NULL);
	sqlCommand.BindInt("id", rowInfo.m_dwId);
	sqlCommand.BindInt("crc", crc);
	sqlCommand.BindInt("size", rowInfo.m_dwSize);
	sqlCommand.BindInt("flag", 1);
	sqlCommand.BindBlob("data", pInBuffer, rowInfo.m_dwSize);
	return sqlCommand.Excute();

}


//////////////////////////////////////////////////////////////////////////
//记录项是否存在
bool DataBase::IsRowExist(DWORD key)
{
	if(!IsValid())
		return false;

	//sql select count
	String strSelect = avar("SELECT COUNT(*) linecount FROM %s WHERE id = '%d'",m_strTable.c_str(), key);

	CSqliteCommand sqlCommand(m_pDb);
	CValueTable	retValues;

	sqlCommand.Compile(strSelect, &retValues);
	sqlCommand.Excute();

	if(retValues.GetInt(0, "linecount") >= 1)
		return true;

	return false;
}


//读取数据
bool DataBase::Read(DWORD key, DWORD dataOffset, void* pOutBuffer, size_t& size)
{
	if(!IsValid())
	{
		LogResource( LOG_DEBUG, avar("grp内容读取错误，key不正确 key:%u, dataOffset:%u, size:%u", key, dataOffset, size) );
		size = 0;
		return false;
	}

	//if(this->IsRowExist(key))
	{
		//sql select
		String strRead = avar("SELECT data FROM %s WHERE id = '%d' LIMIT 1",m_strTable.c_str(), key);

		CSqliteCommand sqlCommand(m_pDb);
		CValueTable	retValus;

		sqlCommand.Compile(strRead, &retValus);
		retValus.PrepareBlob("data", pOutBuffer, size);
		sqlCommand.Excute();

		size = retValus.GetBlobSize("data");
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//更新一整行数据
bool DataBase::UpdateRow(const stPackRowInfo& rowInfo,void* pInBuffer)
{
	if(!IsValid())
		return false;

	DWORD crc = rowInfo.m_dwCrc;
	if(crc == 0)
	{
		crc = SystemUtil::GeneratorCRC32((BYTE*)pInBuffer, rowInfo.m_dwSize);
	}

	//sql update
	String strUpdate = avar("UPDATE %s SET  crc=:crc, size=:size, flag=:flg, data =:data WHERE id ='%d'",m_strTable.c_str(),rowInfo.m_dwId);

	CSqliteCommand sqlCommnd(m_pDb);
	sqlCommnd.Compile(strUpdate, NULL);
	sqlCommnd.BindInt("crc", crc);
	sqlCommnd.BindInt("size", rowInfo.m_dwSize);
	sqlCommnd.BindInt("flag", 1);
	sqlCommnd.BindBlob("data", pInBuffer, rowInfo.m_dwSize);
	sqlCommnd.Excute();
	
	return true;
}


//更新标记
bool DataBase::UpdateFlag(DWORD key, int flag)
{
	if(!IsValid())
		return false;

	//sql update flag
	String strUpdateFlag = avar("UPDATE %s SET flag=:flag WHERE id ='%d'", m_strTable.c_str(), key);

	CSqliteCommand sqlCommand(m_pDb);
	sqlCommand.Compile(strUpdateFlag, NULL);
	sqlCommand.BindInt("flag", flag);
	sqlCommand.Excute();
	
	return true;
}


//////////////////////////////////////////////////////////////////////////
////得到某一列信息

stPackRowInfo DataBase::GetPackRowInfo(DWORD key)
{
	if(!IsValid())
		return false;

	stPackRowInfo info = stPackRowInfo::Invalid;

	//if(this->IsRowExist(key))
	{
		info.m_dwId = key;
		//sql select all
		String strSelectAll = avar("SELECT crc, size, flag FROM %s WHERE id = '%d' LIMIT 1",m_strTable.c_str(),key);

		CSqliteCommand sqlCommand(m_pDb);
		CValueTable retValues;
		sqlCommand.Compile(strSelectAll, &retValues);
		sqlCommand.Excute();

		info.m_dwCrc = retValues.GetInt(0, "crc");
		info.m_dwSize = retValues.GetInt(0, "size");
		info.m_dwFlag = retValues.GetInt(0, "flag");
	}
	return info;
}

//删除表中某一项纪录
bool DataBase::DeleteRow(DWORD key)
{
	if(!IsValid())
		return false;

	if(!this->IsRowExist(key))
		return true;

	//sql delete
	String strDelete = avar("DELETE FROM %s WHERE id = %d", m_strTable.c_str(), key);

	CSqliteCommand sqlCommand(m_pDb);
	sqlCommand.Compile(strDelete, NULL);
	sqlCommand.Excute();
	
	return true;
}



///////////////////////////////////////////////////////////////////////////
//关闭数据库
void DataBase::Close()
{
	if ( m_pDb )
	{
		sqlite3_close(m_pDb);
		m_pDb = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
int getColumnValue(  sqlite3_stmt* stmt,   int col,   void * value)
{
	int type = sqlite3_column_type(stmt, col);

	switch(type)
	{
	case SQLITE_NULL: 
		return 0;
	case SQLITE_INTEGER:
		*((int *)value) = sqlite3_column_int(stmt, col);
		break;
	case SQLITE_FLOAT:
		*((double *)value) = sqlite3_column_double(stmt, col);
		break;
	case SQLITE_TEXT:
	case SQLITE_BLOB:
		strcpy((char*)value, (const char*)sqlite3_column_text(stmt, col));
		break;
	default:
		return -1;	
	}
	return 1;
}