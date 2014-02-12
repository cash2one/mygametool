//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : D3D9Device.h
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月12日
// 说    明 : 
//*******************************************************************************
#pragma once

#include "dxPrecompile.h"

class CD3D9Device
{
public:
	CD3D9Device(void);
	~CD3D9Device(void);

	//初始化D3D9设备
	bool InitDevice(HWND hwnd, int iWidth, int iHeight);

	//检测设备丢失
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



	//显示模式
	typedef std::map<D3DFORMAT, std::vector<D3DDISPLAYMODE> > t_MapFmtMod;
	t_MapFmtMod	m_mapFmtMod;
};
