//*******************************************************************************
// 版权所有(C) 2013 All right reserved
// 文件名称 : ValueTable.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2013年1月14日
// 说    明 : 存储sqlite3查询返回值
//*******************************************************************************


#include "ValueTable.h"

#define ATTRIBUTE_IS_EXIST(attr)	\
{	\
	MapRowAttrItr it = m_mapRowAttr.find(attr); \
	if(it == m_mapRowAttr.end())	\
		return;	\
}

CValueTable::CValueTable(void)
{
}

CValueTable::~CValueTable(void)
{
	this->Clear();
}

//添加一行属性名
void CValueTable::AddRowAttri(String& attr)
{
	if(!attr.empty())
	{
		m_mapRowAttr.insert(MapRowAttrList::value_type(attr, stSqliteData()));
	}
}
//给stSqiliteData绑定
// void CValueTable::BindInt(String& attr, int* pIn)
// {
// 	ATTRIBUTE_IS_EXIST(attr);
// 
// 	m_mapRowAttr[attr].pInt = pIn;
// }
// 
// void CValueTable::BindDouble(String& attr, double* pIn)
// {
// 	ATTRIBUTE_IS_EXIST(attr);
// 
// 	m_mapRowAttr[attr].pDouble = pIn;
// }

// void CValueTable::BindBlob(const String attr, void* pIn, DWORD dwLength)
// {
// 	ATTRIBUTE_IS_EXIST(attr);
// 
// 	m_mapRowAttr[attr].pBuffer = pIn;
// 	m_mapRowAttr[attr].dwBufSize = dwLength;
// }


//给对应的属性名赋值

//int
void CValueTable::SetInt(const String attr, int& value)
{
	ATTRIBUTE_IS_EXIST(attr);

	m_mapRowAttr[attr].uInt = value ;
}

//dobule
void CValueTable::SetFloat(const String attr, double& value)
{
	ATTRIBUTE_IS_EXIST(attr);

	m_mapRowAttr[attr].uDouble = value ;
}

//text
void CValueTable::SetText(const String attr, const char* p, DWORD dwSize)
{
	ATTRIBUTE_IS_EXIST(attr);

	stSqliteData& data = m_mapRowAttr[attr];
	char* pNew = new char[dwSize+1];
	strncpy(pNew, p, dwSize);
	pNew[dwSize] = '\0';

	data.blob.pBuffer = pNew;
	data.blob.dwBufSize = dwSize;
	data.bString = true;
}

//blob
//p : 数据库数据指针
//dwSize: 数据库数据大小
void CValueTable::SetBlob(const String attr, const void* p, DWORD dwBlobSize)
{
	ATTRIBUTE_IS_EXIST(attr);

	stSqliteData& data = m_mapRowAttr[attr];
	if(NULL == data.blob.pBuffer)
	{
		MessageBox(0, "要获取blob数据，请先使用PrepareBlob函数", 0, 0);
		return;
	}
	
	

	if(data.blob.dwOffset >= dwBlobSize)
	{
		data.blob.dwBufSize = 0;
		data.blob.pBuffer = 0;
		return;
	}
	if(dwBlobSize < data.blob.dwOffset + data.blob.dwNeedSize)
		data.blob.dwBufSize = dwBlobSize - data.blob.dwOffset;

	BYTE* pp = (BYTE*)p+data.blob.dwOffset;
	memcpy(data.blob.pBuffer, pp, data.blob.dwBufSize);

}

void CValueTable::PrepareBlob(const String attr,  void* p, DWORD dwSize)
{
	ATTRIBUTE_IS_EXIST(attr);

	stSqliteData& data = m_mapRowAttr[attr];
	data.blob.pBuffer = p;
	data.blob.dwNeedSize = dwSize;
	data.bString = false;
}

DWORD CValueTable::GetBlobSize(const String attr)
{
	MapRowAttrItr it = m_mapRowAttr.find(attr); 
	if(it == m_mapRowAttr.end())	
		return 0;
	
	stSqliteData& data = m_mapRowAttr[attr];
	if(data.bString)
		return 0;

	return data.blob.dwBufSize;
}


////读取对应属性名值
////int
int CValueTable::GetInt(int rowindex, const String attr)
{
	if(m_vecAllData.size() <= 0 )
		return 0;

	MapRowAttrItr it = m_vecAllData[rowindex].find(attr);
	if( it != m_mapRowAttr.end())
	{
		return it->second.uInt;
	}
	return 0;
}

//double
double CValueTable::GetDouble(int rowindex, const String attr)
{
	if(m_vecAllData.size() <= 0 )
		return 0.0;

	MapRowAttrItr it = m_vecAllData[rowindex].find(attr);
	if( it != m_mapRowAttr.end())
	{
		return it->second.uDouble;
	}
	return 0.0;
}

//text
String CValueTable::GetText(int rowindex, const String attr)
{
	if(m_vecAllData.size() <= 0 )
		return "";

	MapRowAttrItr it = m_vecAllData[rowindex].find(attr);
	if( it != m_mapRowAttr.end())
	{
		String strRet = (char*)it->second.blob.pBuffer;
		return strRet;
	}
	return "";
}

//
////text, blob
//void* CValueTable::GetTextOrBlob(int rowindex, String& attr, DWORD& size)
//{
//	if(m_vecAllData.size() <= 0 )
//	{
//		size = 0;
//		return 0;
//	}
//
//	MapRowAttrItr it = m_vecAllData[rowindex].find(attr);
//	if( != m_mapRowAttr.end())
//	{
//		size = it->second.uDwBufSize;
//		return it->second.uBuffer;
//	}
//
//	size = 0;
//	return NULL;
//}

//插入一行数据到m_vecAllData
void CValueTable::AddRowDataToList()
{
	if(m_mapRowAttr.empty())
		return;

	m_vecAllData.push_back(m_mapRowAttr);
}

//清空
void CValueTable::Clear()
{
	VecDataItr it1 = m_vecAllData.begin();
	for( ; it1 != m_vecAllData.end(); ++it1)
	{
		MapRowAttrList& map = *it1;
		MapRowAttrItr it2 = map.begin();
		for( ; it2 != map.end(); ++it2)
		{
			if(it2->second.bString)
			{
				char* p = (char*)it2->second.blob.pBuffer;
				delete[] p;
				p = NULL;
			}
		}
	}
	m_vecAllData.clear();
}