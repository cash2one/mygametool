//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : pakheader.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��18��
// ˵    �� : pakͷ�ļ�
//*******************************************************************************


//pak�ļ���ʽ

//pak�ļ�ͷ
struct stPackHeader
{
	DWORD magic;	//XPAK
	DWORD version;
	int filenum;
	int headersize;
	DWORD flag;
};

/////�м� GetFileSize() - headersize ������




//��� headersize�����ܵ�ͷ
//N��stFileInfo
struct stFileInfo1
{
	DWORD dwFileID;
	DWORD fileoffset;   //grp,ghd�ļ���.pak�ļ��еĶ����ݶ���ʼλ�á���pak��ȥ stPackHeader�� ƫ��ֵ 0��ʼ��
	DWORD size;			//ѹ�����ܺ�Ĵ�С
	DWORD interspacesize;	//���пռ��С������δ�õĿռ�
	DWORD dwCRC;			//ѹ�����ܺ��CRC
};

struct stFileInfo
{
	stFileInfo1 st1;
	char szName[MAX_PATH];
};


//////////////////////////////////////////////////////////////////////////
//���ܱ�ʶ
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