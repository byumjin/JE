//***************************************************************************************
// //
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//***************************************************************************************
#ifndef D3DAPP_H
#define D3DAPP_H

#include "Common/D3DUtil.h"

#include "Common/GameTimer.h"
#include <string>
#include "D3D11SDKLayers.h"
#include "../resource.h"
#include "../Assets/AssetDatabase.h"

//#include "ImageLoader.h"

class CD3DApp
{
public:
	CD3DApp();
	CD3DApp(HINSTANCE hInstance);
	virtual ~CD3DApp();

	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	int Run();

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hWnd,		UINT msg,		WPARAM wParam,		LPARAM lParam);
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
	virtual void OnMouseWheel(WPARAM btnState, int x, int y) {}
	virtual void OnMouseHover(WPARAM btnState, int x, int y) {}
	virtual void OnKeyDown(WPARAM btnState) {}

	virtual void OnSystemKeyDown(WPARAM btnState) {}

	virtual void OnWMSize(WPARAM winState) {}
	

	virtual void OnSaveMap(HWND hwnd) {}
	virtual void OnLoadMap(HWND hwnd) {}

	virtual void OnImportModel(WCHAR* pModelPath, UINT MaterialNum, WCHAR* Material) {}
	virtual void OnImportGeometry(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH]) {}
	virtual void OnImportTexture(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH]) {}
	virtual void OnCreateMaterial(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH]) {}

	virtual void OnAddReflectionActor() {}
	//static for class member
	
	//static BOOL CALLBACK ImportTextureDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ImportModelDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ImportGeometryDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ImportTextureDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK CreateMaterialDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

protected:
	
	void SetCurrentMapTitle(std::wstring maptitle);
	
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

	void SetViewport(UINT Width, UINT Height);

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;

	HWND	  m_hDialogTextureImport;
	HWND	  m_hDialogModelImport;

	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	UINT      m4xMsaaQuality;

	bool	  m_bFocus;

	CGameTimer mTimer;
	

	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11Texture2D* mBackBuffer;

	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	
	ID3D11DepthStencilState* mDepthStencilState;

	D3D11_VIEWPORT mScreenViewport;

	ID3D11Debug* m_d3dDebug;
	ID3D11InfoQueue* pInfoQueue;

	DXGI_SWAP_CHAIN_DESC DeviceSwapChainDesc;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;

	std::wstring m_strMap;

	D3D_DRIVER_TYPE md3dDriverType;
	int mClientWidth;
	int mClientHeight;

	DXGI_MODE_DESC m_OldDesc;

	int mResolutionWidth;
	int mResolutionHeight;

	bool mEnable4xMsaa;

	//ImageLoader m_ImageLoader;

	BOOL m_bIsFullScreen;
	BOOL m_bIsGointoFullScreen;

	void MSAA4X(BOOL bEnable4xMsaa, UINT u4xMsaaQuality, BOOL bSwapChain, DXGI_SWAP_CHAIN_DESC* psd, D3D11_TEXTURE2D_DESC* pds);	
	ID3D11DepthStencilView* GetDepthStencilView();
	
	void SetBackBufferRenderTarget(bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil);
	void SetBackBufferRenderTarget(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, ID3D11DepthStencilState* pDSS, UINT StencilRef,
		bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil);
	void ReleaseBackBufferRenderTarget();
	void CreateDepthStencilState();

};

#endif // CD3DAPP_H

