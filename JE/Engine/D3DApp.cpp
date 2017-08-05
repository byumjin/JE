#include "D3DApp.h"
#include <sstream>

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	CD3DApp* gd3dApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}


struct MODELINFO
{
	WCHAR lpstrFile[MAX_PATH];
	WCHAR lpstrMaterialTitle[MAX_PATH];
	UINT MaterialNum;
	WCHAR lpstrTexture[MAX_TEXTURE][MAX_PATH];
};

CD3DApp::CD3DApp()
{
	md3dDevice = NULL;
	md3dImmediateContext = NULL;
	mSwapChain = NULL;
	mRenderTargetView = NULL;

	mDepthStencilView = NULL;
	mDepthStencilBuffer = NULL;
	mDepthStencilState = NULL;
	mBackBuffer = NULL;

	m_d3dDebug = NULL;
	pInfoQueue = NULL;

	//mDepthResourceView = NULL;
	//mSceneResourseView = NULL;

	mEnable4xMsaa = TRUE;

	mClientWidth = 600;
	mClientHeight = 480;

	m_bIsFullScreen = FALSE;
	m_bIsGointoFullScreen = FALSE;
}

CD3DApp::CD3DApp(HINSTANCE hInstance)
:mhAppInst(hInstance),
mMainWndCaption(L"D3D11 Application"),
md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
mClientWidth(800),
mClientHeight(600),
mEnable4xMsaa(false),
mhMainWnd(0),
mAppPaused(false),
mMinimized(false),
mMaximized(false),
mResizing(false),
m4xMsaaQuality(0),

md3dDevice(0),
md3dImmediateContext(0),
mSwapChain(0),
mDepthStencilBuffer(0),
mBackBuffer(0),
mRenderTargetView(0),
mDepthStencilView(0),
mDepthStencilState(0)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	// Get a pointer to the application object so we can forward 
	// Windows messages to the object's window procedure through
	// the global window procedure.
	gd3dApp = this;

}

CD3DApp::~CD3DApp()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(mDepthStencilState);
	ReleaseCOM(mBackBuffer);
	ReleaseCOM(mSwapChain);
	
	// Restore all default settings.
	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);

	//m_d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	// Get the size of the message
	ReleaseCOM(m_d3dDebug);
	ReleaseCOM(pInfoQueue);

}

HINSTANCE CD3DApp::AppInst()const
{
	return mhAppInst;
}

HWND CD3DApp::MainWnd()const
{
	return mhMainWnd;
}

float CD3DApp::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

int CD3DApp::Run()
{
	MSG msg = { 0 };

	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				DrawScene();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool CD3DApp::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	return true;
}

void CD3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mBackBuffer);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);
	//ReleaseCOM(mDepthStencilState);

	md3dImmediateContext->ClearState();
	
	if (m_bIsGointoFullScreen != m_bIsFullScreen)
	{
		if (m_bIsGointoFullScreen == TRUE)
		{
			HR(mSwapChain->SetFullscreenState(TRUE, nullptr));

			DXGI_SWAP_CHAIN_DESC oldChainDesc;
			mSwapChain->GetDesc(&oldChainDesc);
			m_OldDesc = oldChainDesc.BufferDesc;
			
			UINT currentMode;
			DXGI_MODE_DESC currentModeDesc;

			// Resize the swap chain and recreate the render target view.
			
			IDXGIOutput *pOutput = NULL;
			mSwapChain->GetContainingOutput(&pOutput);
			UINT numModes = 1024;
			DXGI_MODE_DESC modes[1024];
			pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM /*DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/, 0, &numModes, modes);
			pOutput->Release();

			currentMode = numModes - 1;
			currentModeDesc = modes[currentMode];

			HR(mSwapChain->ResizeTarget(&currentModeDesc));

			mClientWidth = currentModeDesc.Width;
			mClientHeight = currentModeDesc.Height;			

			m_bIsFullScreen = TRUE;
		}
		else
		{
			HR(mSwapChain->SetFullscreenState(FALSE, nullptr));

			HR(mSwapChain->ResizeTarget(&m_OldDesc));			

			mClientWidth = m_OldDesc.Width;
			mClientHeight = m_OldDesc.Height;
			
			m_bIsFullScreen = FALSE;
		}
	}
	


	
	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM /*DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&mBackBuffer)));
	HR(md3dDevice->CreateRenderTargetView(mBackBuffer, 0, &mRenderTargetView));
		
	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DXGI_FORMAT_R32G8X24_TYPELESS;// DXGI_FORMAT_R32_TYPELESS; // Use a typeless type here so that it can be both depth-stencil and shader resource.
														// This will fail if we try a format like D32_FLOAT

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;// | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	if (1 == depthStencilDesc.SampleDesc.Count) {
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	else {
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}
	
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DXGI_FORMAT_D32_FLOAT_S8X24_UINT;// DXGI_FORMAT_D32_FLOAT;	// Make the view see this as D32_FLOAT instead of typeless
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, &descDSV, &mDepthStencilView));
	
	// Bind the render target view and depth/stencil view to the pipeline.
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	//CreateDepthStencilState();
}

void CD3DApp::SetViewport(UINT Width, UINT Height)
{
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(Width);
	mScreenViewport.Height = static_cast<float>(Height);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

LRESULT CD3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	//TGA_HEADER TGA_Header;
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_COMMAND:

		if (LOWORD(wParam) == ID_FILE_OPEN)
		{
			OnLoadMap(hwnd);
		}

		if (LOWORD(wParam) == ID_FILE_SAVEAS)
		{
			OnSaveMap(hwnd);
		}

		if (LOWORD(wParam) == ID_IMPORT_GEOMETRY)
		{
			m_hDialogModelImport = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_IMPORT_GEOMETRY), hwnd, gd3dApp->ImportGeometryDlgProc);

			if (m_hDialogModelImport != NULL)
			{
				ShowWindow(m_hDialogModelImport, SW_SHOW);
			}
			else
			{
				MessageBox(hwnd, L"CreateDialog returned NULL", L"Warning!",
					MB_OK | MB_ICONINFORMATION);
			}
		}

		if (LOWORD(wParam) == ID_IMPORT_MODEL)
		{
			m_hDialogModelImport = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_IMPORT_MODEL), hwnd, gd3dApp->ImportModelDlgProc);

			if (m_hDialogModelImport != NULL)
			{
				ShowWindow(m_hDialogModelImport, SW_SHOW);
			}
			else
			{
				MessageBox(hwnd, L"CreateDialog returned NULL", L"Warning!",
					MB_OK | MB_ICONINFORMATION);
			}
		}


		if (LOWORD(wParam) == ID_IMPORT_TEXTURE)
		{
			m_hDialogModelImport = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_IMPORT_TEXTURE), hwnd, gd3dApp->ImportTextureDlgProc);

			if (m_hDialogModelImport != NULL)
			{
				ShowWindow(m_hDialogModelImport, SW_SHOW);
			}
			else
			{
				MessageBox(hwnd, L"CreateDialog returned NULL", L"Warning!",
					MB_OK | MB_ICONINFORMATION);
			}
			
		}

		if (LOWORD(wParam) == ID_CREATE_MATERIAL)
		{
			m_hDialogModelImport = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_CREATE_MATERIAL), hwnd, gd3dApp->CreateMaterialDlgProc);

			if (m_hDialogModelImport != NULL)
			{
				ShowWindow(m_hDialogModelImport, SW_SHOW);
			}
			else
			{
				MessageBox(hwnd, L"CreateDialog returned NULL", L"Warning!",
					MB_OK | MB_ICONINFORMATION);
			}

		}

		if (LOWORD(wParam) == ID_ACTOR_REFLECTION)
		{
			OnAddReflectionActor();
		}

		return 0;

	case WM_IMPORT_MODEL:
	{
		MODELINFO* modelInfo = (MODELINFO*)(lParam);
		OnImportModel(modelInfo->lpstrFile, modelInfo->MaterialNum, modelInfo->lpstrMaterialTitle);
	}

	return 0;

	case WM_IMPORT_GEOMETRY:
	{
		MODELINFO* modelInfo = (MODELINFO*)(lParam);
		OnImportGeometry(modelInfo->lpstrFile, modelInfo->MaterialNum, modelInfo->lpstrTexture);
	}

	return 0;

	case WM_IMPORT_TEXTURE:
	{
		MODELINFO* modelInfo = (MODELINFO*)(lParam);
		OnImportTexture(modelInfo->lpstrFile, modelInfo->MaterialNum, modelInfo->lpstrTexture);
	}

	return 0;

	case WM_CREATE_MATERIAL:
	{
		MODELINFO* modelInfo = (MODELINFO*)(lParam);
		OnCreateMaterial(modelInfo->lpstrMaterialTitle, modelInfo->MaterialNum, modelInfo->lpstrTexture);
	}

	case WM_SAVETEXTURE:
	{

	}


	return 0;

	case WM_CREATE:



		return 0;

	case WM_ACTIVATE:
		
		/*
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_bFocus = false;
			ShowWindow(mhMainWnd, SW_SHOWNOACTIVATE);
		}
		else if (LOWORD(wParam) == WA_ACTIVE)
		{
			m_bFocus = true;
			ShowWindow(mhMainWnd, SW_SHOWNORMAL);
		}
		*/

		m_bFocus = true;
		ShowWindow(mhMainWnd, SW_SHOWNORMAL);

		return 0;

		/*else

		{
			mAppPaused = false;
			mTimer.Start();
		}*/



	return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);

		if (md3dDevice)
		{	
			OnWMSize(wParam);
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{					
					//OnResize();
				}
			}
			

			
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		/*
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		*/
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if(m_bFocus)
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		if (m_bFocus)
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		if (m_bFocus)
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEHOVER:
		if (m_bFocus)
		OnMouseHover(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		if (m_bFocus)
		OnMouseWheel(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYDOWN:
		if (m_bFocus)
		OnKeyDown(wParam);
		return 0;
	case WM_SYSKEYDOWN:
		if (m_bFocus)
		OnSystemKeyDown(wParam);
	
		return 0;
	}
	
	

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK CD3DApp::ImportTextureDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HWND DlgHandle = NULL;

	switch (Message)
	{
	case WM_INITDIALOG:
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDC_IM_BUTTON14:
		{
			OPENFILENAME OFN;
			WCHAR lpstrFile[MAX_PATH] = L"";

			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hwnd;
			OFN.lpstrFilter = L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = L"c:\\";
			if (GetOpenFileName(&OFN) != 0)
			{
				DlgHandle = GetDlgItem(hwnd, IDC_TI_EDIT4);
				Edit_SetText(DlgHandle, OFN.lpstrFile);
			}
		}
		break;

		case ID_TI_OK:
		{
			MODELINFO paraModelInfo;

			DlgHandle = GetDlgItem(hwnd, IDC_TI_EDIT4);
			Edit_GetText(DlgHandle, paraModelInfo.lpstrFile, MAX_PATH);

			//DlgHandle = GetDlgItem(hwnd, IDC_COMBO1);
			//paraModelInfo.MaterialNum = SendMessage(DlgHandle, CB_GETCURSEL, 0, 0);
			

			SendMessage(GetParent(hwnd), WM_IMPORT_TEXTURE, 0, (LPARAM)&paraModelInfo);
		}
		SendMessage(hwnd, WM_DESTROY, 0, 0);
		break;

		case ID_TI_CANCEL:
			SendMessage(hwnd, WM_DESTROY, 0, 0);


			break;
		}
		break;
	case WM_DESTROY:
		//delete pTGA_Header;
		DestroyWindow(hwnd);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK CD3DApp::ImportGeometryDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HWND DlgHandle = NULL;
	/*LPCTSTR strMaterial[] = { L"BASICMATERIAL" };
	UINT countMaterial[] = { 3 };
	UINT iSelectedMaterial;*/

	switch (Message)
	{
	case WM_INITDIALOG:

		//DlgHandle = GetDlgItem(hwnd, IDC_COMBO1);
		//for (int Count = 0; Count < 1; Count++)
		//	SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(strMaterial[Count]));

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDC_IM_BUTTON12:
		{
			OPENFILENAME OFN;
			WCHAR lpstrFile[MAX_PATH] = L"";

			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hwnd;
			OFN.lpstrFilter = L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = L"c:\\";
			if (GetOpenFileName(&OFN) != 0)
			{
				DlgHandle = GetDlgItem(hwnd, IDC_EDIT12);
				Edit_SetText(DlgHandle, OFN.lpstrFile);
			}
		}
		break;

		case IDOK:
		{
			MODELINFO paraModelInfo;
			

			DlgHandle = GetDlgItem(hwnd, IDC_EDIT12);
			Edit_GetText(DlgHandle, paraModelInfo.lpstrFile, MAX_PATH);

			//DlgHandle = GetDlgItem(hwnd, IDC_COMBO1);
			//paraModelInfo.MaterialNum = SendMessage(DlgHandle, CB_GETCURSEL, 0, 0);

			/*
			for (UINT i = 0; i < MAX_TEXTURE; i++)
			{
				DlgHandle = GetDlgItem(hwnd, IDC_IM_EDIT1 + i);
				Edit_GetText(DlgHandle, paraModelInfo.lpstrTexture[i], MAX_PATH);
			}
			*/

			SendMessage(GetParent(hwnd), WM_IMPORT_GEOMETRY, 0, (LPARAM)&paraModelInfo);
		}
		SendMessage(hwnd, WM_DESTROY, 0, 0);
		break;

		case IDCANCEL:
			SendMessage(hwnd, WM_DESTROY, 0, 0);


			break;
		}
		break;
	case WM_DESTROY:
		//delete pTGA_Header;
		DestroyWindow(hwnd);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK CD3DApp::CreateMaterialDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HWND DlgHandle = NULL;
	LPCTSTR strMaterial[] = { L"BASICMATERIAL" };

	UINT countMaterial[] = { 3 };
//	UINT iSelectedMaterial;

	switch (Message)
	{
	case WM_INITDIALOG:

		/*
		DlgHandle = GetDlgItem(hwnd, IDC_COMBO5);
		for (UINT Count = 0; Count < AssetDatabase::GetInstance()->m_ListofGeometry.size(); Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(AssetDatabase::GetInstance()->m_ListofGeometry.at(Count)));
		*/

		DlgHandle = GetDlgItem(hwnd, IDC_COMBO1);
		for (UINT Count = 0; Count < 1; Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(strMaterial[Count]));

		DlgHandle = GetDlgItem(hwnd, IDC_COMBO2);
		for (UINT Count = 0; Count < AssetDatabase::GetInstance()->m_ListofTexture.size(); Count++)
		{
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(AssetDatabase::GetInstance()->m_ListofTexture.at(Count)));
		}

		DlgHandle = GetDlgItem(hwnd, IDC_COMBO3);
		for (UINT Count = 0; Count < AssetDatabase::GetInstance()->m_ListofTexture.size(); Count++)
		{
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(AssetDatabase::GetInstance()->m_ListofTexture.at(Count)));
		}

		DlgHandle = GetDlgItem(hwnd, IDC_COMBO4);
		for (UINT Count = 0; Count < AssetDatabase::GetInstance()->m_ListofTexture.size(); Count++)
		{
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(AssetDatabase::GetInstance()->m_ListofTexture.at(Count)));
		}


		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			WCHAR MatTitle[MAX_PATH];

			MODELINFO paraModelInfo;

			//DlgHandle = GetDlgItem(hwnd, IDC_COMBO5);
			//Edit_GetText(DlgHandle, paraModelInfo.lpstrFile, MAX_PATH);

			DlgHandle = GetDlgItem(hwnd, IDC_EDIT12);
			Edit_GetText(DlgHandle, MatTitle, MAX_PATH);
			

			wcscpy_s(paraModelInfo.lpstrMaterialTitle, MAX_PATH, MatTitle);

			
			paraModelInfo.MaterialNum = SendMessage(DlgHandle, CB_GETCURSEL, 0, 0);

			for (UINT i = 0; i < MAX_TEXTURE; i++)
			{
				DlgHandle = GetDlgItem(hwnd, IDC_COMBO2 + i);
				Edit_GetText(DlgHandle, paraModelInfo.lpstrTexture[i], MAX_PATH);
			}


			SendMessage(GetParent(hwnd), WM_CREATE_MATERIAL, 0, (LPARAM)&paraModelInfo);
		}
		SendMessage(hwnd, WM_DESTROY, 0, 0);
		break;

		case IDCANCEL:
			SendMessage(hwnd, WM_DESTROY, 0, 0);


			break;
		}
		break;
	case WM_DESTROY:
		//delete pTGA_Header;
		DestroyWindow(hwnd);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK CD3DApp::ImportModelDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HWND DlgHandle = NULL;
	LPCTSTR strMaterial[] = { L"BASICMATERIAL" };

	UINT countMaterial[] = { 3 };
//	UINT iSelectedMaterial;

	switch (Message)
	{
	case WM_INITDIALOG:

		DlgHandle = GetDlgItem(hwnd, IDC_COMBO5);
		for (UINT Count = 0; Count < AssetDatabase::GetInstance()->m_ListofGeometry.size(); Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(AssetDatabase::GetInstance()->m_ListofGeometry.at(Count)));


		DlgHandle = GetDlgItem(hwnd, IDC_COMBO1);
		for (UINT Count = 0; Count < AssetDatabase::GetInstance()->m_ListofMaterial.size(); Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(AssetDatabase::GetInstance()->m_ListofMaterial.at(Count)));
			

		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
	
		case IDOK:
		{
			MODELINFO paraModelInfo;

			DlgHandle = GetDlgItem(hwnd, IDC_COMBO5);
			Edit_GetText(DlgHandle, paraModelInfo.lpstrFile, MAX_PATH);

			WCHAR matTile[MAX_PATH];
			DlgHandle = GetDlgItem(hwnd, IDC_COMBO1);
			Edit_GetText(DlgHandle, matTile, MAX_PATH);

			wcscpy_s(paraModelInfo.lpstrMaterialTitle, MAX_PATH, matTile);

			SendMessage(GetParent(hwnd), WM_IMPORT_MODEL, 0, (LPARAM)&paraModelInfo);			
		}
		SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;

		case IDCANCEL:
			SendMessage(hwnd, WM_DESTROY, 0, 0);


			break;
		}
		break;
	case WM_DESTROY:
		//delete pTGA_Header;
		DestroyWindow(hwnd);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

/*
BOOL CALLBACK CD3DApp::ImportTextureDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	HWND DlgHandle = NULL;
	TGA_HEADER* pTGA_Header = NULL;
	
	//Three-channel color with alpha channel	Three color channels (5 bits:6 bits:5 bits), with 0 or 1 bit(s) of alpha			BC1		Direct3D 9.1
	//Three-channel color with alpha channel	Three color channels (5 bits:6 bits:5 bits), with 4 bits of alpha					BC2		Direct3D 9.1
	//Three-channel color with alpha channel	Three color channels (5 bits:6 bits:5 bits) with 8 bits of alpha					BC3		Direct3D 9.1
	//One-channel color	One color channel (8 bits)																				BC4		Direct3D 10
	//Two-channel color	Two color channels (8 bits:8 bits)																		BC5		Direct3D 10
	//Three-channel high dynamic range (HDR) color	Three color channels (16 bits:16 bits:16 bits) in "half" floating point*	BC6H	Direct3D 11
	//Three-channel color, alpha channel optional																					BC7		Direct3D 11
	

	LPCTSTR strFormat[] = { L"BC1", L"BC2", L"BC3", L"BC4", L"BC5", L"BC6H", L"BC7"};
	LPCTSTR strMode[] = { L"Wrap", L"Mirror", L"Clamp", L"Border"};
	LPCTSTR strFilter[] = { L"Nearest-Point", L"Linear", L"Bilinear", L"Anisotropic" };
	wchar_t strTextureTitle[30];

	wchar_t strTextureWidth[6];
	wchar_t strTextureHeight[6];
	
	int iFilter;
	int iFormat;
	UINT bsRGB;
	UINT bNormal;
	UINT bTypeless;

	SAVING_TEXTURE_INFO STINFO;
	//D3DX11_IMAGE_LOAD_INFO loadInfo;
	UINT FilterFlag = 0;
	DXGI_FORMAT FormatFlag;

	switch (Message)
	{
	case WM_INITDIALOG:
		DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO1);
		for (int Count = 0; Count < 7; Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(strFormat[Count]));
		
		DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO2);		
		for (int Count = 0; Count < 4; Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(strMode[Count]));

		SendMessage(DlgHandle, CB_SETCURSEL, 0, reinterpret_cast<LPARAM>(strMode[0]));

		DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO3);
		for (int Count = 0; Count < 4; Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(strMode[Count]));

		SendMessage(DlgHandle, CB_SETCURSEL, 0, reinterpret_cast<LPARAM>(strMode[0]));
		
		DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO4);
		for (int Count = 0; Count < 4; Count++)
			SendMessage(DlgHandle, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(strFilter[Count]));

		SendMessage(DlgHandle, CB_SETCURSEL, 0, reinterpret_cast<LPARAM>(strFilter[2]));

		//DlgHandle = GetDlgItem(hwnd, IDC_RADIO1);
		CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_TI_OK:

			Edit_GetText(GetDlgItem(hwnd, IDC_TI_EDIT4), STINFO.lpstrFile, MAX_PATH);

			Edit_GetText(GetDlgItem(hwnd, IDC_TI_EDIT1), strTextureTitle, 30);

			Edit_GetText(GetDlgItem(hwnd, IDC_TI_EDIT2), strTextureWidth, 6);
			Edit_GetText(GetDlgItem(hwnd, IDC_TI_EDIT3), strTextureHeight, 6);
						
			

			

			DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO4);			
			iFilter = SendMessage(DlgHandle, CB_GETCURSEL, 0, 0);

			if (iFilter == 0)
				FilterFlag = D3DX11_FILTER_POINT;
			else 
				FilterFlag = D3DX11_FILTER_LINEAR;
			
			//STINFO.loadInfo.Filter = FilterFlag;
			//STINFO.loadInfo.FirstMipLevel = 0;

			DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO1);
			iFormat = SendMessage(DlgHandle, CB_GETCURSEL, 0, 0);
			bsRGB = IsDlgButtonChecked(hwnd, IDC_RADIO1);
			bNormal = IsDlgButtonChecked(hwnd, IDC_RADIO2);
			bTypeless = IsDlgButtonChecked(hwnd, IDC_RADIO3);


			if (bsRGB)
			{
				if (iFormat == 0)
					FormatFlag = DXGI_FORMAT_BC1_UNORM_SRGB;
				else if (iFormat == 1)
					FormatFlag = DXGI_FORMAT_BC2_UNORM_SRGB;
				else if (iFormat == 2)
					FormatFlag = DXGI_FORMAT_BC3_UNORM_SRGB;
				else if (iFormat == 3)
					FormatFlag = DXGI_FORMAT_BC4_UNORM;
				else if (iFormat == 4)
					FormatFlag = DXGI_FORMAT_BC5_UNORM;
				else if (iFormat == 5)
					FormatFlag = DXGI_FORMAT_BC6H_UF16;
				else if (iFormat == 6)
					FormatFlag = DXGI_FORMAT_BC7_UNORM_SRGB;
			}
			else if (bNormal)
			{
				if (iFormat == 0)
					FormatFlag = DXGI_FORMAT_BC1_UNORM;
				else if (iFormat == 1)
					FormatFlag = DXGI_FORMAT_BC2_UNORM;
				else if (iFormat == 2)
					FormatFlag = DXGI_FORMAT_BC3_UNORM;
				else if (iFormat == 3)
					FormatFlag = DXGI_FORMAT_BC4_UNORM;
				else if (iFormat == 4)
					FormatFlag = DXGI_FORMAT_BC5_UNORM;
				else if (iFormat == 5)
					FormatFlag = DXGI_FORMAT_BC6H_UF16;
				else if (iFormat == 6)
					FormatFlag = DXGI_FORMAT_BC7_UNORM;
			}
			else
			{
				if (iFormat == 0)
					FormatFlag = DXGI_FORMAT_BC1_TYPELESS;
				else if (iFormat == 1)
					FormatFlag = DXGI_FORMAT_BC2_TYPELESS;
				else if (iFormat == 2)
					FormatFlag = DXGI_FORMAT_BC3_TYPELESS;
				else if (iFormat == 3)
					FormatFlag = DXGI_FORMAT_BC4_TYPELESS;
				else if (iFormat == 4)
					FormatFlag = DXGI_FORMAT_BC5_TYPELESS;
				else if (iFormat == 5)
					FormatFlag = DXGI_FORMAT_BC6H_TYPELESS;
				else if (iFormat == 6)
					FormatFlag = DXGI_FORMAT_BC7_TYPELESS;
			}

			STINFO.loadInfo.Format = FormatFlag;
			STINFO.loadInfo.Height = D3DX11_FROM_FILE;//(UINT)_wtoi(strTextureWidth);
			STINFO.loadInfo.Width = D3DX11_FROM_FILE;// (UINT)_wtoi(strTextureHeight);
			STINFO.loadInfo.MipLevels = D3DX11_FROM_FILE;
			STINFO.loadInfo.MipFilter = STINFO.loadInfo.Filter;
			STINFO.loadInfo.Usage = D3D11_USAGE_STAGING;
			STINFO.loadInfo.MiscFlags = 0;
			STINFO.loadInfo.pSrcInfo = 0;
			STINFO.loadInfo.BindFlags = 0;
			STINFO.loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			STINFO.loadInfo.Depth = D3DX11_FROM_FILE;		

						

			//DlgHandle = GetDlgItem(hwnd, IDD_DIALOG_IMPORT_TEXTURE);
			SendMessage(GetParent(hwnd), WM_SAVETEXTURE, 0, (LPARAM)&STINFO);

		

			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		case ID_TI_CANCEL:

			//DlgHandle = GetDlgItem(hwnd, IDD_DIALOG_IMPORT_TEXTURE);

			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		}
		break;
	case WM_DESTROY:
		//delete pTGA_Header;
		DestroyWindow(hwnd);
		
		//PostQuitMessage(0);
		break;
	case WM_IMPORT_TEXTURE_TGA:

		pTGA_Header = (TGA_HEADER*)lParam;

		if (pTGA_Header->bitsperpixel == 24)
		{
			DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO1);
			SendMessage(DlgHandle, CB_SELECTSTRING, 0, reinterpret_cast<LPARAM>(strFormat[0]));
		}
		else if (pTGA_Header->bitsperpixel == 32)
		{
			DlgHandle = GetDlgItem(hwnd, IDC_TI_COMBO1);
			SendMessage(DlgHandle, CB_SETCURSEL, 0, reinterpret_cast<LPARAM>(strFormat[2]));
			//SendMessage(DlgHandle, CB_GETCURSEL, 0, 0);
		}

		{
#ifdef _UNICODE
			std::wostringstream osW;
			std::wostringstream osH;
#else
			std::ostringstream osW;
			std::ostringstream osH;
#endif

			osW << pTGA_Header->width;
			osH << pTGA_Header->height;

			Edit_SetText(GetDlgItem(hwnd, IDC_TI_EDIT2), osW.str().c_str());
			Edit_SetText(GetDlgItem(hwnd, IDC_TI_EDIT3), osH.str().c_str());


			Edit_SetText(GetDlgItem(hwnd, IDC_TI_EDIT4), pTGA_Header->path);
		}


		break;
	default:
		return FALSE;
	}
	return TRUE;
}
*/

bool CD3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(wc.lpszClassName, mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	RECT desktop;
	GetWindowRect(mhMainWnd, &desktop);

	mResolutionWidth = desktop.right;
	mResolutionHeight = desktop.bottom;

	ShowWindow(mhMainWnd, SW_SHOWNORMAL);
	UpdateWindow(mhMainWnd);

	return true;
}

bool CD3DApp::InitDirect3D()
{
	// Create the device and device context.
	
	UINT createDeviceFlags = 0;
	
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	//_CrtSetBreakAlloc(2684040);
	
#endif	

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		md3dDriverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}
	
#if _DEBUG
	HR(md3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&m_d3dDebug)));
	HR(md3dDevice->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void**>(&pInfoQueue)));
	//pInfoQueue->SetBreakOnID(D3D11_MESSAGE_ID_LIVE_PIXELSHADER, true);
#endif
	
	//m_d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.
	
	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM /*DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.s

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = /*DXGI_FORMAT_R8G8B8A8_UNORM*/DXGI_FORMAT_R8G8B8A8_UNORM /*DXGI_FORMAT_R8G8B8A8_UNORM_SRGB*/;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	DeviceSwapChainDesc = sd;

	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.  If we tried to use a different IDXGIFactory instance
	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
	// This function is being called with a device from a different IDXGIFactory."

	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));	

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));	

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));	

	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
	
	//No Alt-Enter
	HR(dxgiFactory->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_WINDOW_CHANGES));

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	//this->OnResize();
	CD3DApp::OnResize();


	//Default 
	CreateDepthStencilState();
	return true;
}

void CD3DApp::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC DSD;
	DSD.DepthEnable = TRUE;
	DSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;// D3D11_DEPTH_WRITE_MASK_ZERO;
	DSD.DepthFunc = D3D11_COMPARISON_LESS;

	DSD.StencilEnable = TRUE;
	DSD.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	DSD.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	D3D11_DEPTH_STENCILOP_DESC FF;
	FF.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	FF.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	FF.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	FF.StencilFunc = D3D11_COMPARISON_ALWAYS;

	DSD.FrontFace = FF;

	D3D11_DEPTH_STENCILOP_DESC BF;
	BF.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	BF.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	BF.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	BF.StencilFunc = D3D11_COMPARISON_NEVER;

	DSD.BackFace = BF;

	HR(md3dDevice->CreateDepthStencilState(&DSD, &mDepthStencilState));
}

void CD3DApp::SetCurrentMapTitle(std::wstring maptitle)
{
	size_t lastposition;
	UINT i = 0;
	
	lastposition = maptitle.rfind(L"\\", maptitle.length());
	maptitle = maptitle.substr(lastposition + 1, maptitle.length());
	m_strMap = maptitle;
}

void CD3DApp::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << m_strMap << L"    " /*mMainWndCaption << L"    "*/
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void CD3DApp::MSAA4X(BOOL bEnable4xMsaa, UINT u4xMsaaQuality, BOOL bSwapChain, DXGI_SWAP_CHAIN_DESC* psd, D3D11_TEXTURE2D_DESC* pds)
{
	if (bSwapChain)
	{
		//4x MSAAA?
		if (bEnable4xMsaa)
		{
			psd->SampleDesc.Count = 4;// how many multisamples				
			psd->SampleDesc.Quality = u4xMsaaQuality - 1;
		}
		else
		{
			psd->SampleDesc.Count = 1;
			psd->SampleDesc.Quality = 0;
		}
	}
	else
	{
		//4x MSAAA?
		if (bEnable4xMsaa)
		{
			pds->SampleDesc.Count = 4;// how many multisamples				
			pds->SampleDesc.Quality = u4xMsaaQuality - 1;
		}
		else
		{
			pds->SampleDesc.Count = 1;
			pds->SampleDesc.Quality = 0;
		}
	}
	
}

ID3D11DepthStencilView* CD3DApp::GetDepthStencilView()
{
	return mDepthStencilView;
}

void CD3DApp::SetBackBufferRenderTarget(bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil)
{
	if (bDepthStencil)
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	else
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);
	//md3dImmediateContext->OMSetDepthStencilState(mDepthStencilState, 0);

	if (bClearRenderTargetView)
		md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	
	if (bClearDepth && bClearStencil)
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	else if (bClearDepth)
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	else if (bClearStencil)
		md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

void CD3DApp::SetBackBufferRenderTarget(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, ID3D11DepthStencilState* pDSS, UINT StencilRef,
										bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil)
{
	md3dImmediateContext->OMSetRenderTargets(1, &pRTV, pDSV);
	
	if (bClearRenderTargetView)
		md3dImmediateContext->ClearRenderTargetView(pRTV, reinterpret_cast<const float*>(&Colors::Black));

	if (pDSS)
	md3dImmediateContext->OMSetDepthStencilState(pDSS, StencilRef);

	//md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	if (pDSV)
		md3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

void CD3DApp::ReleaseBackBufferRenderTarget()
{	

	//ID3D11RenderTargetView* pNullRTV[] = { NULL };

	//md3dImmediateContext->OMSetRenderTargets(1, pNullRTV, NULL);
	md3dImmediateContext->OMSetRenderTargets(0, NULL, NULL);
	md3dImmediateContext->OMSetDepthStencilState(NULL, NULL);
}
