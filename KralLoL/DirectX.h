#pragma once

#include <string>

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <tchar.h>
#include <dinput.h>
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <dcomp.h>
#pragma comment(lib, "dcomp.lib")
#pragma comment(lib, "d3d11.lib")

// Veri
inline ID3D11Device* g_pd3dDevice = NULL;
inline ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
inline ID3D11RenderTargetView* g_pd3dRenderTargetView = NULL;
inline IDXGISwapChain* g_pSwapChain = NULL;

class Direct3D9Render
{
private:

	std::string gamePatch;
	void AutoAccept();
public:
	bool closedClient = false;

	Direct3D9Render()
	{
	}

	~Direct3D9Render() = default;

	void StartFrame();

	void EndFrame();

	// directx'i, yazý tiplerini, imgui'yi ve nesneleri baþlatýr
	bool DirectXInit(HWND hWnd);

	bool CreateRenderTarget();

	void CleanupRenderTarget();

	// ana iþleme döngüsü
	int Render();

	// directx'i serbest býrakýr ve imgui'yi temizler
	void Shutdown();

	// imgui'yi baþlatýr
	void Renderimgui(HWND hWnd);

	// imgui stillerini baþlatýr
	void MenuInit();
};
extern Direct3D9Render Direct3D9;
