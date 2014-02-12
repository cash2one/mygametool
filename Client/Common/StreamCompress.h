#pragma once

#include "Stream.h"

class StreamCompress
{
public:
	//Ω‚—πÀıstream
	static int uncompressStream(Stream& i_stream, Stream& o_stream, size_t size = -1);

	//—πÀı
	static int compress_data(unsigned char* pOut, unsigned long* pOutlen, unsigned char* pIn, unsigned long nInLen, int level);

	//Ω‚—πÀı
	static int uncompress_data(unsigned char* pOut, unsigned long* pOutLen, unsigned char* pIn, unsigned long nInLen);


};
