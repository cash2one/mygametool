//*******************************************************************************
// ��Ȩ����(C) 2013 All right reserved
// �ļ����� : ValueTable.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2013��1��14��
// ˵    �� : �洢sqlite3���з���ֵ
//*******************************************************************************


#pragma once

#include "public.h"
#include <map>
#include <vector>


//����ֵ����
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
	void*	pBuffer;		//blob���ⲿָ�롿 �� text
	DWORD	dwBufSize;		//pBuffer size

	DWORD	dwOffset;
	DWORD	dwNeedSize;
};

//ͨ������
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

	//���һ��������
	void AddRowAttri(String& attr);

	//��stSqiliteData��
	//void BindInt(String& attr, int* pIn);
	//void BindDouble(String& attr, double* pIn);

	//blob����һ��ǳ��󣬽�Լ�ڴ棬�Ͳ��ڲ�������
	//void BindBlob(const String attr, const void* pIn, DWORD dwLength);


	//����Ӧ����������ֵ
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

	////��ȡ��Ӧ������ֵ
	////int
	int GetInt(int rowindex, const String attr);
	////double
	double GetDouble(int rowindex, const String attr);
	//text
	String GetText(int rowindex, const String attr);
	////blob
	//void* GetTextOrBlob(int rowindex, String& attr, DWORD& size);

	//����һ�����ݵ�m_vecAllData
	void AddRowDataToList();

	//���
	void Clear();

private:

	//��ѯһ�з���ֵ����[ ��ѯ���������� ] 
	typedef std::map<String, stSqliteData> MapRowAttrList;
	typedef MapRowAttrList::iterator MapRowAttrItr;

	//һ��������
	MapRowAttrList	m_mapRowAttr;

	//��ѯ�������鷵��ֵ[select * from ....]
	typedef std::vector< MapRowAttrList > VecDataList;
	typedef VecDataList::iterator VecDataItr;

	VecDataList	m_vecAllData;
};
