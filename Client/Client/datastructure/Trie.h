//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : Trie.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��11��28��
// ˵    �� : ����trie�������δʴ���
//*******************************************************************************
#pragma once

// CTrie�Ǵ洢�����ֿ�ģ�ÿ���ڵ㶼�洢0- 256���ַ���һ���ֽڵ���Ϣ��
// �洢�������£� �����и����δʣ�abc��

#include <string>

class CTrie
{
private:
	CTrie*	m_pSon[256];	//256���ӽڵ�
	bool	m_bEnd;			//�˽ڵ��Ƿ����ս��
public:
	
	CTrie();

	//���һ�����δ�
	void AddFilterWord(CTrie* root, const char* pszWord);

	//�ж�ĳһ���ַ����Ƿ������δ�
	bool Search(CTrie* root, const char* pszWord);

	//���trie
	void DeleteAll(CTrie* root);
};


//���������ֿ��ļ�
void G_Filter_Read_Data(CTrie* root, const char* pszFileName);

//���һ��������������(���������ֵ��� *��ʾ����������*)
void G_Filter_String(CTrie* root, std::string strIn, std::string& strOut);