//*******************************************************************************
// 版权所有(C) 2012 All right reserved
// 文件名称 : D3D9Device.cpp
// 当前版本 : 1.0.0.1
// 作    者 : fuzhun (339448261@qq.com)
// 设计日期 : 2012年12月12日
// 说    明 : 
//*******************************************************************************
#include "D3D9Device.h"
#include<sstream>
#include <fstream>


static D3DFORMAT s_format[] = 
{
	D3DFMT_A8R8G8B8,
	D3DFMT_X8R8G8B8,
	D3DFMT_X1R5G5B5,
	D3DFMT_R5G6B5,
};

const char* GetFormatText(D3DFORMAT format)
{
	switch(format)
	{
	case D3DFMT_A8R8G8B8:	return "D3DFMT_A8R8G8B8";
	case D3DFMT_X8R8G8B8:	return "D3DFMT_X8R8G8B8";
	case D3DFMT_X1R5G5B5:	return "D3DFMT_X1R5G5B5";
	case D3DFMT_R5G6B5:		return "D3DFMT_R5G6B5";
	default:				return "UNKNOW_FORMAT";
	}
}

void CD3D9Device::__EnumAdapterModes()
{
	UINT fmtNum = ARRAY_SIZE(s_format);
	for(UINT i = 0; i < fmtNum; ++i)
	{
		D3DFORMAT allowedAdapterFormat = s_format[i];
		UINT numAdapterModes = m_pD3D9->GetAdapterModeCount( D3DADAPTER_DEFAULT, allowedAdapterFormat );
		for( UINT j = 0; j < numAdapterModes; ++j)
		{
			D3DDISPLAYMODE mode;
			if(SUCCEEDED(m_pD3D9->EnumAdapterModes(D3DADAPTER_DEFAULT, s_format[i], j, &mode)))
			{
				m_mapFmtMod[s_format[i]].push_back(mode);
			}
		}
	}

// 	std::ofstream ofstr("mode.txt");
// 
// 	t_MapFmtMod::iterator it = m_mapFmtMod.begin();
// 	for( ; it != m_mapFmtMod.end(); ++it)
// 	{
// 		for(size_t index = 0; index < it->second.size(); ++index )
// 		{
// 			D3DDISPLAYMODE& mode = it->second[index];
// 			std::ostringstream os;
// 			os << GetFormatText(it->first)
// 				<<"  width "<<mode.Width <<"  height " <<mode.Height
// 				<<"  refreshRate: "<<mode.RefreshRate <<"  Format " <<GetFormatText( mode.Format);
// 
// 			ofstr<<os.str() <<std::endl;
// 		}
// 	}
// 
// 	ofstr.close();
};

D3DDISPLAYMODE CD3D9Device::__GetCompatibleDisplayMode(D3DFORMAT format, int width, int height)
{
	int temp = 9999;
	D3DDISPLAYMODE retMode;
	ZeroMemory(&retMode, sizeof(D3DDISPLAYMODE));

	t_MapFmtMod::iterator it = m_mapFmtMod.find(format);
	if( it != m_mapFmtMod.end())
	{
		for(size_t index = 0; index < it->second.size(); ++index )
		{
			D3DDISPLAYMODE& mode = it->second[index];
			int abst = abs(int(width-mode.Width)) + abs(int(height-mode.Height)); 
			if( abst <= temp)
			{
				temp = abst;
				retMode = mode;
			}
		}
	}
	return retMode;		
}

CD3D9Device::CD3D9Device(void)
: m_pD3D9(NULL)
,m_pDevice9(NULL)
{
}

CD3D9Device::~CD3D9Device(void)
{
}


bool CD3D9Device::OpenD3D9()
{
	m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION);
	if(!m_pD3D9)
	{
		System_LogDebug("Direct3DCreate9 Failed!");
		return false;
	}
	return true;
}

//初始化D3D9设备
bool CD3D9Device::InitDevice(HWND hwnd, int iWidth, int iHeight, bool bWindowed)
{
	if(!OpenD3D9())
		return false;

	//打印显卡信息
	D3DADAPTER_IDENTIFIER9 adapterInfo;
	m_pD3D9->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_WHQL_LEVEL, &adapterInfo);
	System_LogDebug("D3D Driver: %s", adapterInfo.Driver);
	System_LogDebug("Description: %s", adapterInfo.Description);
	System_LogDebug("Version: %d.%d.%d.%d",
		HIWORD(adapterInfo.DriverVersion.HighPart),
		LOWORD(adapterInfo.DriverVersion.HighPart),
		HIWORD(adapterInfo.DriverVersion.LowPart),
		LOWORD(adapterInfo.DriverVersion.LowPart));

	__EnumAdapterModes();
	D3DDISPLAYMODE displaymode = __GetCompatibleDisplayMode(D3DFMT_X8R8G8B8, iWidth, iHeight);
	m_nWidth = displaymode.Width;
	m_nHeight = displaymode.Height;
	m_hMainWnd = hwnd;

	m_d3dpp.BackBufferWidth = m_nWidth;
	m_d3dpp.BackBufferHeight = m_nHeight;
	m_d3dpp.BackBufferFormat = displaymode.Format;
	m_d3dpp.hDeviceWindow = hwnd;
	m_d3dpp.Windowed = bWindowed;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;

	if(FAILED(m_pD3D9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hMainWnd, 
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pDevice9)))
	{
		System_LogDebug("Create D3D Device Error");
		return false;
	}
	
	return true;
}


bool CD3D9Device::IsDeviceLost()
{
	HRESULT hr = m_pDevice9->TestCooperativeLevel();

	if( hr == D3DERR_DEVICELOST)
	{
		// If the device is lost and cannot be reset yet then
		// sleep for a bit and we'll try again on the next 
		// message loop cycle.

		//设备刚丢失时
		::Sleep(20);
		return true;
	}
	// Driver error, exit.
	else if( hr == D3DERR_DRIVERINTERNALERROR)
	{
		MessageBox(0, L"internal Driver Error...Exiting", 0, 0);
		PostQuitMessage(0);
		return true;
	}
	// The device is lost but we can reset and restore it.
	else if( hr == D3DERR_DEVICENOTRESET)
	{
		OnLostDevice();
		m_pDevice9->Reset(&m_d3dpp);
		OnResetDevice();
		return false;
	}

	return false;
}

HRESULT CD3D9Device::OnLostDevice()
{
	//OnLostDevice要将D3DPOOL_DEFAULT创建的资源Release掉
	return S_OK;
}

HRESULT CD3D9Device::OnResetDevice()
{
	//恢复D3DPOOL_DEFAULT的资源

	return S_OK;
}