//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : TextureCache.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��12��
// ˵    �� : ��IDirect3DTexture9�ķ�װ������һ��������256*256��
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
