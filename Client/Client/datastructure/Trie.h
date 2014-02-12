//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : Trie.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年11月28日
// 说    明 : 基于trie树的屏蔽词处理
//*******************************************************************************
#pragma once

// CTrie是存储屏蔽字库的，每个节点都存储0- 256个字符【一个字节的信息】
// 存储方法如下： 假设有个屏蔽词：abc，

#include <string>

class CTrie
{
private:
	CTrie*	m_pSon[256];	//256个子节点
	bool	m_bEnd;			//此节点是否是终结点
public:
	
	CTrie();

	//添加一个屏蔽词
	void AddFilterWord(CTrie* root, const char* pszWord);

	//判断某一个字符串是否是屏蔽词
	bool Search(CTrie* root, const char* pszWord);

	//清空trie
	void DeleteAll(CTrie* root);
};


//读入屏蔽字库文件
void G_Filter_Read_Data(CTrie* root, const char* pszFileName);

//检测一段文字中屏蔽字(将是屏蔽字的用 *表示，中文两个*)
void G_Filter_String(CTrie* root, std::string strIn, std::string& strOut);