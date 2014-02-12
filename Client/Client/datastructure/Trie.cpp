//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : Trie.cpp
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��11��28��
// ˵    �� : 
//*******************************************************************************
#include "StdAfx.h"
#include "Trie.h"
#include <fstream>
#include <Windows.h>

CTrie::CTrie(void)
: m_bEnd(false)
{
	for(int i = 0; i < 256; ++i)
		m_pSon[i] = NULL;
}



//���һ�����δ�
void CTrie::AddFilterWord(CTrie* root, const char* pszWord)
{
	if(!root || !pszWord)	return;
	
	CTrie* pTmp = root;
	const char* pBegin = pszWord;
	while(*pBegin)
	{
		if(pTmp->m_pSon[(unsigned char)*pBegin] == NULL)
			pTmp->m_pSon[(unsigned char)*pBegin] = new CTrie();
		
		pTmp = pTmp->m_pSon[(unsigned char)*pBegin];

		pBegin++;
	}
	
	//�Դ���ֹΪһ�����δ�
	pTmp->m_bEnd = true;
}

//�ж�ĳһ���ַ����Ƿ������δ�
bool CTrie::Search(CTrie* root, const char* pszWord)
{
	if(!root || !pszWord)
		return false;

	CTrie* pTemp = root;
	const char* pBegin = pszWord;

	while(*pBegin)
	{
		if(pTemp->m_pSon[(unsigned char)*pBegin] != NULL)
			pTemp = pTemp->m_pSon[(unsigned char)*pBegin];
		else
			return false;

		pBegin++;
	}

	//�����������������true
	if(pTemp && pTemp->m_bEnd && !(*pBegin))
		return true;

	return false;
}

//���trie
void CTrie::DeleteAll(CTrie* root)
{
	for(int i = 0; i < 256; ++i)
	{
		DeleteAll(root->m_pSon[i]);
	}

	if(root)
	{
		delete root;
		root = 0;
	}
}

//���������ֿ��ļ�
void G_Filter_Read_Data(CTrie* root, const char* pszFileName)
{
	if(!root || !pszFileName)
		return;

	std::ifstream is(pszFileName);
	char buffer[256] = {0};
	::ZeroMemory(buffer, 256);
	while(is.getline(buffer, 256))
	{
		root->AddFilterWord(root, buffer);
		::ZeroMemory(buffer, sizeof(buffer));
	}
}

//���һ��������������(���������ֵ��� *��ʾ����������*)
void G_Filter_String(CTrie* root, std::string strIn, std::string& strOut)
{
	strOut = strIn;

	for(unsigned int i = 0; i < strIn.length(); )
	{
		unsigned int interval = 1;
		//GB3212�����ַ���ռ�����ֽ�
		if((unsigned char)strIn[i] >= 176 && (unsigned char)strIn[i] <= 247)
			interval = 2;
	
		//���ж��������ַ��Ƿ�Ϊ������
		std::string strSelf = strIn.substr(i, interval);
		if(root->Search(root, strSelf.c_str()))
		{
			std::cout<<"�����֣�	"<<strSelf<<std::endl;
			strOut[i] = '*';
			if(interval == 2)	strOut[i+1] = '*';
		}
		
		for( int j = i + interval; j < strIn.length(); )
		{
			int inInterval = 1;

			if((unsigned char)strIn[i] >= 176 && (unsigned char)strIn[i] <= 247)
			{
				inInterval = 2;
			}

			std::string strAll = strIn.substr(i, j-(i+interval));
			
			if(root->Search(root, strAll.c_str()))
			{
				std::cout<<"�����֣�	"<<strAll<<std::endl;
				const char* p = strAll.c_str();
				int begin = i;
				while(*p++)
				{
					strOut[begin++] = '*';
				}
			}

			j += inInterval;
		}
		
		i += interval;
	}
}