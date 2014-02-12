#pragma once
#include "public.h"
#include "sqlite3.h"

class DataStream;

/*��Դ�洢���ݿ�
+++++++++++++++++++++++++++++++++++
+ ����		����			˵��
+
+ filename (TEXT PRIMARY KEY) (data/interfaces/00000.grp)
+ crc	   (INTEGER)��ԴCRC
+ size     (INTEGER)��Դ��С
+ flag     (INTEGER)��Դ�Ƿ����������������( ��0Ϊ����)
+ data     (BLOB) ��Դ����
++++++++++++++++++++++++++++++++++++
*/

//ͼ����������Ϣ
struct stPackRowInfo
{
	const static stPackRowInfo Invalid;
	DWORD m_dwId;
	DWORD m_dwCrc;
	DWORD m_dwSize;
	DWORD m_dwFlag;   //���
  	
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

	//�򿪻򴴽����ݿ�
	bool Open(const String &strDatabase,const String& strTable);

	//������򿪱�
	bool OpenTable(const String& strTable);

	//��ǰDataBase�Ƿ���Ч
	bool IsValid() { return m_pDb != NULL ;}	

	//�ж�ĳ�����Ƿ����
	bool IsTableExist(const String& strTableName);

	//��¼���Ƿ����
	bool IsRowExist(DWORD key);

	//�������ݻ���� �� ����Ҳ����� ��
	bool Insert(const stPackRowInfo& info,void* pInBuffer);

	//ɾ������һ������
	bool DeleteRow(DWORD key);

	//������
	bool UpdateRow(const stPackRowInfo& info, void* pInBuffer);
	
	//���±��
	bool UpdateFlag(DWORD key, int flag);

	//��ȡ���ݵ�pOutBuffer
	bool Read(DWORD dwID, DWORD dataOffset, void* pOutBuffer, size_t &size);

	//�õ�ĳһ����Ϣ
	stPackRowInfo GetPackRowInfo(DWORD key);

	String GetDatabaseName(){return m_strDatabase;}

	String GetTableName(){return m_strTable;}

	//�ر����ݿ�
	void Close();

public:

	//����DataBase����
	static DataBase* Create();

	//���� DataBase����
	static void Destory(DataBase* pDB);

private:

	sqlite3* m_pDb;

	//���ݿ���
	String   m_strDatabase;

	//����
	String   m_strTable;
};




//������ݿ��(���ݿ���+����)
struct DBKey
{
	std::string strKey1;
	std::string strKey2;

	DBKey(const String & str1, const String & str2) : strKey1(str1), strKey2(str2) { }
};


//���ݿ���������
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