//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : pakheader.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月18日
// 说    明 : pak头文件
//*******************************************************************************


//pak文件格式

//pak文件头
struct stPackHeader
{
	DWORD magic;	//XPAK
	DWORD version;
	int filenum;
	int headersize;
	DWORD flag;
};

/////中间 GetFileSize() - headersize 的内容




//最后 headersize加了密的头
//N个stFileInfo
struct stFileInfo1
{
	DWORD dwFileID;
	DWORD fileoffset;   //grp,ghd文件在.pak文件中的饿数据段起始位置【即pak除去 stPackHeader后 偏移值 0开始】
	DWORD size;			//压缩加密后的大小
	DWORD interspacesize;	//所有空间大小，包括未用的空间
	DWORD dwCRC;			//压缩加密后的CRC
};

struct stFileInfo
{
	stFileInfo1 st1;
	char szName[MAX_PATH];
};


//////////////////////////////////////////////////////////////////////////
//加密标识
enum PakEnc
{
	flagCompress = 0x1,
	flagEncrypt = 0x2,
	flagEncryptHeaderOnly = 0x4,
};


struct stFileSizeInfo
{
	DWORD version;
	DWORD flags;
	DWORD uncompresssize;
	DWORD dwCRC;
};