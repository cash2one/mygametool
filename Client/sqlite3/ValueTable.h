//*******************************************************************************
// 版权所有(C) 2013 All right reserved
// 文件名称 : ValueTable.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2013年1月14日
// 说    明 : 存储sqlite3运行返回值
//*******************************************************************************


#pragma once

#include "public.h"
#include <map>
#include <vector>


//返回值类型
enum RetValueType
{
	RetType_Null,			//type null
	RetType_Int,			//SQLITE_INTEGER
	RetType_Double,			//SQLITE_FLOAT
	RetType_String,			//SQLITE_TEXT
	RetType_Blob,			//SQLITE_BLOB
};


struct stBlobText
{
	void*	pBuffer;		//blob【外部指针】 和 text
	DWORD	dwBufSize;		//pBuffer size

	DWORD	dwOffset;
	DWORD	dwNeedSize;
};

//通用数据
struct stSqliteData
{
	union
	{
		int			uInt;
		double		uDouble;
	};
		
	stBlobText		blob;
	bool			bString;
	
	stSqliteData()
	{
		memset(this, 0, sizeof(*this));
	}
};

class CValueTable
{
public:
	CValueTable(void);
	~CValueTable(void);

	//添加一行属性名
	void AddRowAttri(String& attr);

	//给stSqiliteData绑定
	//void BindInt(String& attr, int* pIn);
	//void BindDouble(String& attr, double* pIn);

	//blob数据一般非常大，节约内存，就不内部分配了
	//void BindBlob(const String attr, const void* pIn, DWORD dwLength);


	//给对应的属性名赋值
	//int
	void SetInt(const String attr, int& value);
	//dobule
	void SetFloat(const String attr, double& value);
	//text
	void SetText(const String attr, const char* p, DWORD dwSize);
	//blob
	void SetBlob(const String attr, const void* p, DWORD dwBlobSize);

	void PrepareBlob(const String attr,  void* p, DWORD dwSize);
	DWORD GetBlobSize(const String attr);

	////读取对应属性名值
	////int
	int GetInt(int rowindex, const String attr);
	////double
	double GetDouble(int rowindex, const String attr);
	//text
	String GetText(int rowindex, const String attr);
	////blob
	//void* GetTextOrBlob(int rowindex, String& attr, DWORD& size);

	//插入一行数据到m_vecAllData
	void AddRowDataToList();

	//清空
	void Clear();

private:

	//查询一行返回值返回[ 查询返回列属性 ] 
	typedef std::map<String, stSqliteData> MapRowAttrList;
	typedef MapRowAttrList::iterator MapRowAttrItr;

	//一行属性名
	MapRowAttrList	m_mapRowAttr;

	//查询出来多组返回值[select * from ....]
	typedef std::vector< MapRowAttrList > VecDataList;
	typedef VecDataList::iterator VecDataItr;

	VecDataList	m_vecAllData;
};
