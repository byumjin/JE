// include the basic windows header files and the Direct3D header files
#include "stdafx.h"
#include "Engine/D3DApp.h"
#include "Engine/Renderer/Render.h"

class InitDirect3DApp : public CD3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
};

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	//_CrtSetBreakAlloc(961);
#endif

	//InitDirect3DApp theApp(hInstance);
	Renderer renderer(hInstance);
	if (!renderer.Init())
		return 0;

	return renderer.Run();

	_CrtDumpMemoryLeaks();
}


InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance)
: CD3DApp(hInstance)
{
}

InitDirect3DApp::~InitDirect3DApp()
{
}

bool InitDirect3DApp::Init()
{
	if (!CD3DApp::Init())
		return false;

	return true;
}

void InitDirect3DApp::OnResize()
{
	CD3DApp::OnResize();
}

void InitDirect3DApp::UpdateScene(float dt)
{

}

void InitDirect3DApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Blue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	HR(mSwapChain->Present(0, 0));
}





