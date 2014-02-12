//*******************************************************************************
// ��Ȩ����(C) 2012 All right reserved
// �ļ����� : D3D9Device.h
// ��ǰ�汾 : 1.0.0.1
// ��    �� : fuzhun (339448261@qq.com)
// ������� : 2012��12��12��
// ˵    �� : 
//*******************************************************************************
#pragma once

#include "dxPrecompile.h"

class CD3D9Device
{
public:
	CD3D9Device(void);
	~CD3D9Device(void);

	//��ʼ��D3D9�豸
	bool InitDevice(HWND hwnd, int iWidth, int iHeight);

	//����豸��ʧ
	bool IsDeviceLost();
	HRESULT	OnLostDevice();
	HRESULT OnResetDevice();

private:

	bool OpenD3D9();

	void __EnumAdapterModes();
	D3DDISPLAYMODE __GetCompatibleDisplayMode(D3DFORMAT format, int width, int height ,bool bWindowed);
private:

	HWND						m_hMainWnd;
	IDirect3D9*					m_pD3D9;
	IDirect3DDevice9*			m_pDevice9;
	D3DPRESENT_PARAMETERS		m_d3dpp;
	int							m_nWidth, m_nHeight;



	//��ʾģʽ
	typedef std::map<D3DFORMAT, std::vector<D3DDISPLAYMODE> > t_MapFmtMod;
	t_MapFmtMod	m_mapFmtMod;
};
