#pragma once
#include "public.h"
#include "sqlite3.h"

class DataStream;

/*资源存储数据库
+++++++++++++++++++++++++++++++++++
+ 列名		类型			说明
+
+ filename (TEXT PRIMARY KEY) (data/interfaces/00000.grp)
+ crc	   (INTEGER)资源CRC
+ size     (INTEGER)资源大小
+ flag     (INTEGER)资源是否与服务器保持最新( 非0为最新)
+ data     (BLOB) 资源数据
++++++++++++++++++++++++++++++++++++
*/

//图包行数据信息
struct stPackRowInfo
{
	const static stPackRowInfo Invalid;
	DWORD m_dwId;
	DWORD m_dwCrc;
	DWORD m_dwSize;
	DWORD m_dwFlag;   //标记
  	
	stPackRowInfo(DWORD dwID = 0,DWORD dwSize = 0,DWORD dwCRC = 0,DWORD dwFlag = 0)
		:m_dwFlag(dwFlag)
		,m_dwId(dwID)
		,m_dwCrc(dwCRC)
		,m_dwSize(dwSize)
	{
		
	}
	bool operator == ( const stPackRowInfo &info )
	{	
		return m_dwFlag == info.m_dwFlag && m_dwId == info.m_dwId && m_dwCrc == info.m_dwCrc && m_dwSize == info.m_dwSize; 
	}
	bool operator != ( const stPackRowInfo &info )
	{	
		return !this->operator==(info);
	}
};

class DataBase
{
public:

	//打开或创建数据库
	bool Open(const String &strDatabase,const String& strTable);

	//创建或打开表
	bool OpenTable(const String& strTable);

	//当前DataBase是否有效
	bool IsValid() { return m_pDb != NULL ;}	

	//判断某个表是否存在
	bool IsTableExist(const String& strTableName);

	//记录项是否存在
	bool IsRowExist(DWORD key);

	//插入数据或更新 【 更新也用这个 】
	bool Insert(const stPackRowInfo& info,void* pInBuffer);

	//删除表中一项数据
	bool DeleteRow(DWORD key);

	//更新行
	bool UpdateRow(const stPackRowInfo& info, void* pInBuffer);
	
	//更新标记
	bool UpdateFlag(DWORD key, int flag);

	//读取数据到pOutBuffer
	bool Read(DWORD dwID, DWORD dataOffset, void* pOutBuffer, size_t &size);

	//得到某一列信息
	stPackRowInfo GetPackRowInfo(DWORD key);

	String GetDatabaseName(){return m_strDatabase;}

	String GetTableName(){return m_strTable;}

	//关闭数据库
	void Close();

public:

	//创建DataBase对象
	static DataBase* Create();

	//销毁 DataBase对象
	static void Destory(DataBase* pDB);

private:

	sqlite3* m_pDb;

	//数据库名
	String   m_strDatabase;

	//表名
	String   m_strTable;
};




//标记数据库表(数据库名+表名)
struct DBKey
{
	std::string strKey1;
	std::string strKey2;

	DBKey(const String & str1, const String & str2) : strKey1(str1), strKey2(str2) { }
};


//数据库表管理器类
class SqliteDatabase
{
public:
	SqliteDatabase(void);
	~SqliteDatabase(void);

	DataBase* Open(const String& strDatabase, const String& strTable);

	void Close(DataBase* pDb);

	void CloseAll();

public:

	typedef std::map<DBKey, DataBase*>	DBMap;
	typedef DBMap::iterator				DBMapItr;

	DBMap	m_mapDB;
};

//Singleton
inline SqliteDatabase& GetDBMgr()
{
	static SqliteDatabase mgr;
	return mgr;
}