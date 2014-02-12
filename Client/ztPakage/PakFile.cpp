//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : PakFile.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月18日
// 说    明 : 
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

	//读取所有文件头
	stream.Seek(size-hdr.headersize, Stream::SeekBegin);
	std::vector<BYTE> aBuf;
	aBuf.resize(hdr.headersize);
	stream.Read(&aBuf[0], hdr.headersize);

	//解密
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

//解压缩一般文件，如datas.pak
void CPakFile::CompressFile()
{
	CFileStream stream;
	if(!stream.Open(m_strPakName.c_str(), Stream::ReadAccess))
		return;

	for(int i = 0; i < m_vecFile.size(); ++i)
	{
		stFileBaseInfo& info = m_vecFile[i];

		CWindowsHelper::CreateDirectory(std::string(info.szName));

		//存储到本地的文件
		CFileStream localStream;
		if(localStream.Open(info.szName, Stream::WriteAccess))
		{

			stream.Seek(info.fileOffset, Stream::SeekBegin);
			
			//解压缩和解密

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

				if(!bZip)  //只加密
				{
					realData.resize(dwLen);
					stream.Read(&realData[0], dwLen);
					bin::DesDecryptData((BYTE*)&realData[0], dwLen);

					fileLen = dwLen;
				}
				else	//压缩 + 加密
				{
					DWORD outLen = dwLen*5/4;
					DWORD realLen;

					std::vector<BYTE> tempData;

					if(bEncrypt)
					{
						realLen = (outLen+7)/8*8;

						//先解密
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

					//再解压缩
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



			//写数据到本地
			localStream.Write(&realData[0], fileSizeInfo.uncompresssize);

			//std::cout<< info.szName <<" 输出成功"<<std::endl;
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

		//存储到本地的文件
		CFileStream localStream;
		if(localStream.Open(info.szName, Stream::WriteAccess))
		{

			stream.Seek(info.fileOffset, Stream::SeekBegin);

			//解压缩和解密

			stFileSizeInfo fileSizeInfo;
			stream.Read(&fileSizeInfo, sizeof(stFileSizeInfo));

			std::vector<BYTE> realData;
			realData.resize(info.size);
			{
				stream.Read(&realData[0], fileSizeInfo.uncompresssize);
			}



			//写数据到本地
			localStream.Write(&realData[0], fileSizeInfo.uncompresssize);

			//std::cout<< info.szName <<" 输出成功"<<std::endl;
			info.print();
		}


	}
}