//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : TextureCache.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月12日
// 说    明 : 是IDirect3DTexture9的封装，代表一整块纹理【256*256】
//*******************************************************************************
#pragma once

#include "TexturePrecompile.h"


class CTextureCache
{
public:
	CTextureCache(void);
	~CTextureCache(void);

protected:
	//texture
	IDirect3DTexture9*	m_pTexture;
};
