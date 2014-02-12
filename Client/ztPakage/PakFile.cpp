//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : PakFile.cpp
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��18��
// ˵    �� : 
//*******************************************************************************
#include "../Common/precompile.h"
#include "PakFile.h"
#include "FileStream.h"

#include "DesEncryption.h"

#include "WindowsHelper.h"

#include "StreamCompress.h"


CPakFile::CPakFile(void)
{
}

CPakFile::~CPakFile(void)
{
}

bool CPakFile::Open(const char* pakfile)
{
	CFileStream stream;
	if(!stream.Open(pakfile, Stream::ReadAccess))
		return false;

	Stream::Size size = stream.GetSize();
	stPackHeader hdr;
	stream.Read(&hdr, sizeof(stPackHeader));

	if(strncmp((char*)&hdr.magic, "XPAK", 4))
	{
		System_LogDebug("Open %s Pak Error!", pakfile);
		return false;
	}

	if(hdr.version != 9 && hdr.version != 8)
		return false;

	if(hdr.headersize == 0 || hdr.headersize > 1024*1024*2)
		return false;

	m_dwVersion = hdr.version;
	m_dwFlags = hdr.flag;
	m_dwFileNum = hdr.filenum;
	m_strPakName = pakfile;

	//��ȡ�����ļ�ͷ
	stream.Seek(size-hdr.headersize, Stream::SeekBegin);
	std::vector<BYTE> aBuf;
	aBuf.resize(hdr.headersize);
	stream.Read(&aBuf[0], hdr.headersize);

	//����
	if( (hdr.flag & flagEncrypt) ||(hdr.flag & flagEncryptHeaderOnly))
	{
		bin::DesDecryptHeader(&aBuf[0], hdr.headersize);
	}

	m_vecFile.resize(hdr.filenum);
	BYTE* p = &aBuf[0];
	for(int i = 0; i < hdr.filenum; ++i)
	{
		stFileInfo st;
		stFileBaseInfo& dst = m_vecFile[i];

		memcpy(&st.st1, p, sizeof(stFileInfo1));
		p += sizeof(stFileInfo1);
		if(!strncpy(st.szName, (char*)p, MAX_PATH))
			return false;

		p += strlen(st.szName)+1;

		dst.size = st.st1.size;
		dst.fileOffset = st.st1.fileoffset + sizeof(hdr);
		dst.dwCRC = st.st1.dwCRC;
		strcpy(dst.szName, st.szName);
	}

	return true;
}


void CPakFile::stFileBaseInfo::print()
{
	std::cout<< "FileName: "<<szName
		<<"  FileOffset: "<<fileOffset
		<<" Size: "<<size<<std::endl;
		//<<" CRC: "<<dwCRC

}

//��ѹ��һ���ļ�����datas.pak
void CPakFile::CompressFile()
{
	CFileStream stream;
	if(!stream.Open(m_strPakName.c_str(), Stream::ReadAccess))
		return;

	for(int i = 0; i < m_vecFile.size(); ++i)
	{
		stFileBaseInfo& info = m_vecFile[i];

		CWindowsHelper::CreateDirectory(std::string(info.szName));

		//�洢�����ص��ļ�
		CFileStream localStream;
		if(localStream.Open(info.szName, Stream::WriteAccess))
		{

			stream.Seek(info.fileOffset, Stream::SeekBegin);
			
			//��ѹ���ͽ���

			stFileSizeInfo fileSizeInfo;
			stream.Read(&fileSizeInfo, sizeof(stFileSizeInfo));

			if(m_dwFlags & flagEncrypt)
			{
				bin::DesDecryptData((BYTE*)&fileSizeInfo, sizeof(stFileSizeInfo));
			}

			bool bZip = m_dwFlags & flagCompress;
			bool bEncrypt = m_dwFlags & flagEncrypt;
			std::vector<BYTE> realData;
			DWORD fileLen;

			if(bZip || bEncrypt)
			{
				DWORD dwLen = (fileSizeInfo.uncompresssize + 7)/8 * 8;

				if(!bZip)  //ֻ����
				{
					realData.resize(dwLen);
					stream.Read(&realData[0], dwLen);
					bin::DesDecryptData((BYTE*)&realData[0], dwLen);

					fileLen = dwLen;
				}
				else	//ѹ�� + ����
				{
					DWORD outLen = dwLen*5/4;
					DWORD realLen;

					std::vector<BYTE> tempData;

					if(bEncrypt)
					{
						realLen = (outLen+7)/8*8;

						//�Ƚ���
						tempData.resize(realLen);
						stream.Read(&tempData[0], realLen);
						bin::DesDecryptData((BYTE*)&tempData[0], realLen);
					}
					else
					{
						realLen = outLen;
						tempData.resize(realLen);
						stream.Read(&tempData[0], realLen);
					}

					//�ٽ�ѹ��
					realData.resize(outLen);
					StreamCompress::uncompress_data((unsigned char*)&realData[0], (DWORD*)&outLen,
						(unsigned char*)&tempData[0], realLen);

					fileLen = realLen;

				}
			}
			else
			{
				stream.Read(&realData[0], fileSizeInfo.uncompresssize);
			}



			//д���ݵ�����
			localStream.Write(&realData[0], fileSizeInfo.uncompresssize);

			//std::cout<< info.szName <<" ����ɹ�"<<std::endl;
			info.print();
		}

		
	}
}


void CPakFile::CompressFile2()
{
	CFileStream stream;
	if(!stream.Open(m_strPakName.c_str(), Stream::ReadAccess))
		return;

	for(int i = 0; i < m_vecFile.size(); ++i)
	{
		stFileBaseInfo& info = m_vecFile[i];

		CWindowsHelper::CreateDirectory(std::string(info.szName));

		//�洢�����ص��ļ�
		CFileStream localStream;
		if(localStream.Open(info.szName, Stream::WriteAccess))
		{

			stream.Seek(info.fileOffset, Stream::SeekBegin);

			//��ѹ���ͽ���

			stFileSizeInfo fileSizeInfo;
			stream.Read(&fileSizeInfo, sizeof(stFileSizeInfo));

			std::vector<BYTE> realData;
			realData.resize(info.size);
			{
				stream.Read(&realData[0], fileSizeInfo.uncompresssize);
			}



			//д���ݵ�����
			localStream.Write(&realData[0], fileSizeInfo.uncompresssize);

			//std::cout<< info.szName <<" ����ɹ�"<<std::endl;
			info.print();
		}


	}
}