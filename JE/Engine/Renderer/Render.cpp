#include "Render.h"

struct DDS_PIXELFORMAT {
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwFourCC;
	DWORD dwRGBBitCount;
	DWORD dwRBitMask;
	DWORD dwGBitMask;
	DWORD dwBBitMask;
	DWORD dwABitMask;
};

typedef struct {
	DWORD           dwSize;
	DWORD           dwFlags;
	DWORD           dwHeight;
	DWORD           dwWidth;
	DWORD           dwPitchOrLinearSize;
	DWORD           dwDepth;
	DWORD           dwMipMapCount;
	DWORD           dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	DWORD           dwCaps;
	DWORD           dwCaps2;
	DWORD           dwCaps3;
	DWORD           dwCaps4;
	DWORD           dwReserved2;
} DDS_HEADER;

Renderer::Renderer(HINSTANCE hInstance)
: CD3DApp(hInstance), mVB(0), mIB(0), mFX(0), mTech(0),
mfxWorldViewProj(0), mfxWorld(0), mfxWorldInvTranspose(0), mInputLayout(0), mGeoLayout(0),
mEyePosW(0.0f, 0.0f, 0.0f), 
mTheta(1.5f*MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f), bshowBoundingBox(false), bshowDebugWindows(false)
{
	bSelectedObjExist = false;
	bShowWireframe = false;

	mMainWndCaption = L"Jin Engine";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = DirectX::XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);	
	
	////////////CreateDirectionalLight/////////////	
	XMMATRIX boxScale = DirectX::XMMatrixScaling(100000.0f, 100000.0f, 100000.0f);
	XMMATRIX boxOffset = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, DirectX::XMMatrixMultiply(boxScale, boxOffset));	
}

Renderer::~Renderer()
{
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
	mEffects.DestroyAll();

	for (UINT i = 0; i < 12; i++)
		m_UITextureList[i].Shutdown();

	ClearResources();

	delete AssetDatabase::GetInstance();

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);

	ReleaseCOM(mFX);
	ReleaseCOM(mInputLayout);
	ReleaseCOM(mGeoLayout);;

	ReleaseCOM(m_LUT_Texture);
	ReleaseCOM(m_LUT_SRV);
}

void Renderer::Shutdown()
{
	mRenderTextureManger.mRT_List.clear();
	mMRTforDeferredR.Shutdown();

	m_Scene.Shutdown();	
	m_Bloom.Shutdown();
	mDebugWindows.Shutdown();
	
	m_SkyBox.Shutdown();	
	m_ShadowRender.Shutdown();
	m_ObjectsRender.Shutdown();	
	mMRTforDeferredR.Shutdown();
	m_CubeMapRender.Shutdown();	
	m_LightingRender.Shutdown();


	ClearResources();

	mGizmo.Shutdown();	

	Destroy();

}

void Renderer::ClearResources()
{
	for (std::list<DirectionalLightActor*>::iterator DLi = mDirectionalLightActorManager.mDLA_List.begin(); DLi != mDirectionalLightActorManager.mDLA_List.end(); DLi++)
	{
		DirectionalLightActor* pDL = (*DLi);
		delete pDL;
	}

	mDirectionalLightActorManager.mDLA_List.clear();


	for (std::list<ReflectionActor*>::iterator RAi = m_ReflectionActorManager.m_RA_List.begin(); RAi != m_ReflectionActorManager.m_RA_List.end(); RAi++)
	{
		ReflectionActor* pRA = (*RAi);
		delete pRA;
	}

	m_ReflectionActorManager.m_RA_List.clear();

	for (std::list<Object*>::iterator obj = mObjectManager.mObj_List.begin(); obj != mObjectManager.mObj_List.end(); obj++)
	{
		Object* pObj = (*obj);
		delete pObj;
	}

	mObjectManager.mObj_List.clear();
	
	AssetDatabase::GetInstance()->Shutdown();
}

void Renderer::Destroy()
{
	
}

void Renderer::CreateUISprite()
{
	DWORD Len = MAX_PATH;
	WCHAR DirPath[MAX_PATH];
	
	GetCurrentDirectory(Len, DirPath);
	wcscat_s(DirPath, L"\\Engine\\UI\\UIsprite\\256\\idea-256.png");
	m_UITextureList[0].Initialize(md3dDevice, DirPath); //DLA

	GetCurrentDirectory(Len, DirPath);
	wcscat_s(DirPath, L"\\Engine\\UI\\UIsprite\\256\\landscape-256.png");
	m_UITextureList[1].Initialize(md3dDevice, DirPath); //RA
}

void Renderer::InsertRenderer(RenderCapsule* pRC)
{
	std::vector<RenderCapsule*>::iterator it = m_RenderCapsules.begin();
	for (UINT i = 0; i < m_RenderCapsules.size(); i++, it++)
	{
		if (m_RenderCapsules.at(i)->RenderPriority > pRC->RenderPriority)
		{
			m_RenderCapsules.insert(it, pRC);
			break;
		}
	}

	m_RenderCapsules.push_back(pRC);
}


void Renderer::Create()
{
	AssetDatabase::GetInstance();
	AssetDatabase::SetDevice(md3dDevice);

	CreateUISprite();
	BuildGeometryBuffers();
	
	//BuildSkyBox();

	// Must init Effects first since InputLayouts depend on shader signatures.
	//Effects::InitAll(md3dDevice);
	mEffects.InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice, &mEffects);
	RenderStates::InitAll(md3dDevice);

	mGizmo.Initialize(md3dDevice, md3dImmediateContext, mClientWidth, mClientHeight);
	

	//SceneTexture
	mRenderTextureManger.LoadRT(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0);

	//silhouette 						
	mRenderTextureManger.LoadRT(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R8_UNORM,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0);

	//reflectionTexture 		
	mRenderTextureManger.LoadRT(md3dDevice, mClientWidth, mClientHeight, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		FALSE, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS);


	mMRTforDeferredR.Initialize(md3dDevice, 5);
	////BasicColor
	mMRTforDeferredR.CreateRenderTexture(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0, 0);
	////Specular
	mMRTforDeferredR.CreateRenderTexture(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0, 1);
	////Normal
	mMRTforDeferredR.CreateRenderTexture(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0, 2);
	////Depth
	mMRTforDeferredR.CreateRenderTexture(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0, 3);
	////UV
	mMRTforDeferredR.CreateRenderTexture(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0, 4);
	////Reflection
	//mMRTforDeferredR.CreateRenderTexture(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
	//	mEnable4xMsaa, m4xMsaaQuality, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0, 5);

	BuildBRDFLUT();

	m_Scene.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);


	m_SkyBox.RenderPriority = BACKGROUND + 1;
	m_SkyBox.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	m_SkyBox.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
							&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, &mBoxWorld, &m_Scene, m_LUT_SRV);
	InsertRenderer(&m_SkyBox);

	m_ObjectsRender.RenderPriority = RENDERING + 10;
	m_ObjectsRender.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	m_ObjectsRender.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
		&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, NULL, &m_Scene, m_LUT_SRV);
	InsertRenderer(&m_ObjectsRender);

	m_CubeMapRender.RenderPriority = RENDERING + 20;
	m_CubeMapRender.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	m_CubeMapRender.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
		&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, NULL, &m_Scene, m_LUT_SRV);
	InsertRenderer(&m_CubeMapRender);


	m_ShadowRender.RenderPriority = RENDERING + 30;
	m_ShadowRender.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	m_ShadowRender.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
		&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, NULL, &m_Scene, m_LUT_SRV);
	InsertRenderer(&m_ShadowRender);	
	
	m_LightingRender.RenderPriority = RENDERING + 40;
	m_LightingRender.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	m_LightingRender.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
		&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, &mWorld, &m_Scene, m_LUT_SRV);
	InsertRenderer(&m_LightingRender);

	m_Bloom.RenderPriority = POSTPROCESS + 1;
	m_Bloom.SetTextureDivision(2, 2);
	
	m_Bloom.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	m_Bloom.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
		&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, &mWorld, &m_Scene, m_LUT_SRV);

	InsertRenderer(&m_Bloom);	


	std::list<DirectionalLightActor*>::iterator ppDLi = mDirectionalLightActorManager.mDLA_List.begin();
	mDebugWindows.RenderPriority = DEBUG_RENDER + 1;
	
	mDebugWindows.Initialize(md3dDevice, mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	mDebugWindows.m_BloomTexture = m_Bloom.m_GaussianHTexture;
	mDebugWindows.InitExternValue(md3dImmediateContext, &mMRTforDeferredR, &mCamera, &mCameraForRenderWindow, &mEyePosW, &mTimer, &mRenderTargetView, &mDepthStencilView,
		&mObjectManager, &m_ReflectionActorManager, &mDirectionalLightActorManager, &mRenderTextureManger, &mWorld, &m_Scene, m_LUT_SRV);
	
	InsertRenderer(&mDebugWindows);
		
}

void Renderer::BuildBRDFLUT()
{
	Image timage;
	timage.format = DXGI_FORMAT_R16G16B16A16_FLOAT;

	HANDLE hFile;

	DWORD  dwBytesWrite = 0;
	char   ReadBuffer[4096] = { 0 };

	DWORD Len = MAX_PATH;
	WCHAR LUTPath[MAX_PATH];

	GetCurrentDirectory(Len, LUTPath);
	wcscat_s(LUTPath, L"\\Engine\\Textures\\LUT\\LUT.ddf");

	hFile = CreateFile(LUTPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);

	ReadFile(hFile, &timage.width, sizeof(int), &dwBytesWrite, NULL);
	ReadFile(hFile, &timage.height, sizeof(int), &dwBytesWrite, NULL);

	byte dump;

	ReadFile(hFile, &dump, 1, &dwBytesWrite, NULL);
	ReadFile(hFile, &dump, 1, &dwBytesWrite, NULL);

	timage.rowPitch = sizeof(float) * timage.width;
	timage.slicePitch = sizeof(float) * timage.height * timage.width;

	float* farray01 = new float[1024 * 1024];
	float* farray02 = new float[1024 * 1024];
	float* farray03 = new float[1024 * 1024];
	float* farray04 = new float[1024 * 1024];

	float* farray = new float[1024 * 1024 * 4];

	float fz = 0.0f;
	for (UINT i = 0; i < 1024 * 1024; i++)
	{
		ReadFile(hFile, &farray01[i], sizeof(float), &dwBytesWrite, NULL);
		ReadFile(hFile, &farray02[i], sizeof(float), &dwBytesWrite, NULL);
		ReadFile(hFile, &farray03[i], sizeof(float), &dwBytesWrite, NULL);
		ReadFile(hFile, &farray04[i], sizeof(float), &dwBytesWrite, NULL);
	}

	for (UINT i = 0; i < 1024 * 1024; i++)
	{
		farray[i * 4] = farray01[i];
		farray[i * 4 + 1] = farray02[i];
		farray[i * 4 + 2] = farray03[i];
		farray[i * 4 + 3] = farray04[i];
	}


	CloseHandle(hFile);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Height = 1024;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA  subResource;
	subResource.pSysMem = farray;
	subResource.SysMemPitch = 1024 * 16;
	subResource.SysMemSlicePitch = 0;

	HR(md3dDevice->CreateTexture2D(&texDesc, &subResource, &m_LUT_Texture));


	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(md3dDevice->CreateShaderResourceView(m_LUT_Texture, &srvDesc, &m_LUT_SRV));


	delete[] farray01;
	delete[] farray02;
	delete[] farray03;
	delete[] farray04;

	delete[] farray;
}

void Renderer::OnLoadMap(HWND hwnd)
{
	HANDLE hFile;

	DWORD  dwBytesWrite = 0;
	char   ReadBuffer[4096] = { 0 };


	WCHAR FilePath[MAX_PATH];
	WCHAR DirPath[MAX_PATH];

	const WCHAR* pDirPath = NULL;
	DWORD DW = MAX_PATH;

	GetModuleFileName(NULL, FilePath, DW);
	wcscpy_s(DirPath, (ExtractDirectoryWchar(FilePath)).c_str());

	OPENFILENAME OFN;
	//WCHAR str[300];
	WCHAR lpstrFile[MAX_PATH] = L"";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hwnd;
	OFN.lpstrFilter = L"Map Files (*.map)\0*.map\0";
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = DirPath;
	if (GetOpenFileName(&OFN) != 0)
	{
		hFile = CreateFile(OFN.lpstrFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
		
		SetCurrentMapTitle(OFN.lpstrFile);

		ClearResources();

		AssetDatabase::GetInstance();
		AssetDatabase::SetDevice(md3dDevice);

		bSelectedObjExist = false;

		for (std::list<DirectionalLightActor*>::iterator DLi = mDirectionalLightActorManager.mDLA_List.begin(); DLi != mDirectionalLightActorManager.mDLA_List.end(); DLi++)
		{
			DirectionalLightActor* pDL = (*DLi);
			delete pDL;
		}
		

		mDirectionalLightActorManager.mDLA_List.clear();
		std::list<DirectionalLightActor*> vDLClear;
		mDirectionalLightActorManager.mDLA_List.swap(vDLClear);




		for (std::list<ReflectionActor*>::iterator RAi = m_ReflectionActorManager.m_RA_List.begin(); RAi != m_ReflectionActorManager.m_RA_List.end(); RAi++)
		{
			ReflectionActor* pRA = (*RAi);
			delete pRA;
		}

		m_ReflectionActorManager.m_RA_List.clear();
		std::list<ReflectionActor*> vRAClear;
		m_ReflectionActorManager.m_RA_List.swap(vRAClear);

		std::list<Object*>::iterator oi;
		
		for (oi = mObjectManager.mObj_List.begin(); oi != mObjectManager.mObj_List.end(); oi++)
		{
			Object* pObj = (*oi);
			pObj->Shutdown();
			delete pObj;
		}
		
		mObjectManager.mObj_List.clear();
		std::list<Object*> vObjClear;
		mObjectManager.mObj_List.swap(vObjClear);

		//GameCamera
		CameraActorInfo CameraInfo;
		ReadFile(hFile, &CameraInfo, sizeof(CameraActorInfo), &dwBytesWrite, NULL);		
		mCamera.SetCameraActorInfo(CameraInfo);		

		//RenderCamera
		ReadFile(hFile, &CameraInfo, sizeof(CameraActorInfo), &dwBytesWrite, NULL);
		mCameraForRenderWindow.SetCameraActorInfo(CameraInfo);		

		//GizmoGamera
		ReadFile(hFile, &CameraInfo, sizeof(CameraActorInfo), &dwBytesWrite, NULL);
		mGizmoCamera.SetCameraActorInfo(CameraInfo);

		//Directional Lights
		UINT DLightCount;
		ReadFile(hFile, &DLightCount, sizeof(DLightCount), &dwBytesWrite, NULL);
		
		DirectionalLightActorInfo* DLAINFO = new DirectionalLightActorInfo[DLightCount];

		std::list<DirectionalLightActor*>::iterator li;
		UINT i;

		for (i = 0; i < DLightCount; i++)
		{
			////////////CreateDirectionalLight/////////////
			mDirectionalLightActorManager.LoadDLA(md3dDevice);
		}

		for (i = 0, li = mDirectionalLightActorManager.mDLA_List.begin(); i < DLightCount; i++, li++)
		{
			ReadFile(hFile, &DLAINFO[i], sizeof(DirectionalLightActorInfo), &dwBytesWrite, NULL);

			(*li)->SetWorldPosition(DLAINFO[i].m_ActorInfo.m_WorldPosition);
			(*li)->SetRotate(DLAINFO[i].m_ActorInfo.m_Rotate);
			(*li)->SetScale(DLAINFO[i].m_ActorInfo.m_Scale);
			(*li)->SetWorldMatrix(DLAINFO[i].m_ActorInfo.m_WorldMat);
			(*li)->SetDirLightData(DLAINFO[i].m_LightData);

			if ((*li)->m_Shadow_Texture == NULL)
			{
				(*li)->m_Shadow_Texture = new RenderTexture();
				(*li)->m_Shadow_Texture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32_FLOAT,
						FALSE, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0);

				(*li)->UpdateRLU();
				(*li)->UpdateFrustrum(mClientWidth, mClientHeight);
				(*li)->m_Shadow_Texture->OnResize(mClientWidth, mClientHeight, md3dImmediateContext, NULL);
			}

			//(*li)->UpdateFrustrum(mClientWidth, mClientHeight);
			//(*li)->m_Shadow_Texture->OnResize(mClientWidth, mClientHeight, md3dImmediateContext, NULL);
			
			
		}
		delete[] DLAINFO;
		DLAINFO = NULL;


		//Reflection Actor
		UINT RACount;
		ReadFile(hFile, &RACount, sizeof(RACount), &dwBytesWrite, NULL);

		ReflectionActorInfo* RAINFO = new ReflectionActorInfo[RACount];

		std::list<ReflectionActor*>::iterator RAi;
		//UINT i;

		for (i = 0; i < RACount; i++)
		{
			////////////CreateDirectionalLight/////////////
			m_ReflectionActorManager.LoadRA(md3dDevice);
		}

		for (i = 0, RAi = m_ReflectionActorManager.m_RA_List.begin(); i < RACount; i++, RAi++)
		{
			ReadFile(hFile, &RAINFO[i], sizeof(ReflectionActorInfo), &dwBytesWrite, NULL);

			(*RAi)->SetWorldPosition(RAINFO[i].m_ActorInfo.m_WorldPosition);
			(*RAi)->SetRotate(RAINFO[i].m_ActorInfo.m_Rotate);
			(*RAi)->SetScale(RAINFO[i].m_ActorInfo.m_Scale);
			(*RAi)->SetWorldMatrix(RAINFO[i].m_ActorInfo.m_WorldMat);

			(*RAi)->Translation(0.0f, 0.0f, 0.0f);

			(*RAi)->SetRadius(RAINFO[i].m_RAData.radius);

		}
		delete[] RAINFO;
		RAINFO = NULL;

		//Assets
		//Geo

		UINT GeoCount;
		ReadFile(hFile, &GeoCount, sizeof(GeoCount), &dwBytesWrite, NULL);

		AssetData_GeoListInfo* AD_GeoInfo = new AssetData_GeoListInfo[GeoCount];

		for (UINT i = 0; i < GeoCount; i++)
		{
			ReadFile(hFile, &AD_GeoInfo[i], sizeof(AssetData_GeoListInfo), &dwBytesWrite, NULL);

			OnImportGeometry(AD_GeoInfo[i].GeometryPath, 1, NULL);
		}

		delete[] AD_GeoInfo;
		AD_GeoInfo = NULL;


		//Texture
		UINT TextureCount;
		ReadFile(hFile, &TextureCount, sizeof(TextureCount), &dwBytesWrite, NULL);

		AssetData_TextureListInfo* AD_TexInfo = new AssetData_TextureListInfo[TextureCount];

		for (UINT i = 0; i < TextureCount; i++)
		{
			ReadFile(hFile, &AD_TexInfo[i], sizeof(AssetData_TextureListInfo), &dwBytesWrite, NULL);

			WCHAR *TexturePath = new WCHAR[MAX_PATH];
			wcscpy_s(TexturePath, MAX_PATH, AD_TexInfo[i].TexturePath);

			AssetDatabase::GetInstance()->m_ListofTexture.push_back(TexturePath);
			AssetDatabase::GetInstance()->LoadAsset<Texture>(TexturePath);
		}

		delete[] AD_TexInfo;
		AD_TexInfo = NULL;


		//Material

		UINT MaterialCount;
		ReadFile(hFile, &MaterialCount, sizeof(MaterialCount), &dwBytesWrite, NULL);

		AssetData_MaterialListInfo* AD_MatInfo = new AssetData_MaterialListInfo[MaterialCount];

		for (UINT i = 0; i < MaterialCount; i++)
		{
			ReadFile(hFile, &AD_MatInfo[i], sizeof(AssetData_MaterialListInfo), &dwBytesWrite, NULL);

			OnCreateMaterial(AD_MatInfo[i].MaterialPath, 1, AD_MatInfo[i].MaterialTexturePath);
		}

		delete[] AD_MatInfo;
		AD_MatInfo = NULL;

	
		//Objects

		UINT ObjCount;
		
		ReadFile(hFile, &ObjCount, sizeof(ObjCount), &dwBytesWrite, NULL);

		ObjectInfo* OBJINFO = new ObjectInfo[ObjCount];

		for (i = 0; i < ObjCount; i++)
		{
			ReadFile(hFile, &OBJINFO[i], sizeof(ObjectInfo), &dwBytesWrite, NULL);

			//Object* pObj = new Object;

			OnImportModel(OBJINFO[i].m_GeometryPath, 1, OBJINFO[i].m_MaterialPath[0]);

			//mObjectManager.mObj_List.push_back(pObj);

		}

		for (i = 0, oi = mObjectManager.mObj_List.begin(); oi != mObjectManager.mObj_List.end(); i++, oi++)
		{
			(*oi)->SetObjectInfo(OBJINFO[i]);
		}
		

		delete[] OBJINFO;

		CloseHandle(hFile);


		
	}

	
}

void Renderer::OnSaveMap(HWND hwnd)
{
	HANDLE hFile;

	DWORD  dwBytesWrite = 0;
	char   ReadBuffer[4096] = { 0 };

	OPENFILENAME OFN;
	//WCHAR str[300];
	WCHAR lpstrFile[MAX_PATH] = L"";

	memset(&OFN, 0, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = hwnd;
	OFN.lpstrFilter = L"Map Files (*.map)\0*.map\0";
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"c:\\";
	if (GetSaveFileName(&OFN) != 0)
	{

		std::wstring FileTitle(OFN.lpstrFile);
		
		if (FileTitle.find(L".map") == std::wstring::npos)
		{
			FileTitle.append(L".map");
		}



		hFile = CreateFile(FileTitle.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
		
		//GameCamera

		CameraActorInfo CameraInfo;

		mCamera.GetCameraActorInfo(CameraInfo);

		WriteFile(hFile, &CameraInfo, sizeof(CameraActorInfo), &dwBytesWrite, NULL);

		//RenderCamera

		mCameraForRenderWindow.GetCameraActorInfo(CameraInfo);

		WriteFile(hFile, &CameraInfo, sizeof(CameraActorInfo), &dwBytesWrite, NULL);

		//GizmoGamera
		mGizmoCamera.GetCameraActorInfo(CameraInfo);

		WriteFile(hFile, &CameraInfo, sizeof(CameraActorInfo), &dwBytesWrite, NULL);

		//Directional Lights

		UINT DLightCount = mDirectionalLightActorManager.mDLA_List.size();

		WriteFile(hFile, &DLightCount, sizeof(DLightCount), &dwBytesWrite, NULL);

		DirectionalLightActorInfo* DLAINFO = new DirectionalLightActorInfo[DLightCount];

		std::list<DirectionalLightActor*>::iterator li;
		UINT i;
		for (i = 0, li = mDirectionalLightActorManager.mDLA_List.begin(); i < DLightCount; i++, li++)
		{
			DLAINFO[i].m_ActorInfo.m_WorldPosition = (*li)->GetWorldPosition();
			DLAINFO[i].m_ActorInfo.m_Rotate = (*li)->GetRotate();
			DLAINFO[i].m_ActorInfo.m_Scale = (*li)->GetScale();

			DLAINFO[i].m_ActorInfo.m_WorldMat = (*li)->GetWorldMatrix();
			DLAINFO[i].m_LightData = (*li)->GetDirLightData();

			WriteFile(hFile, &DLAINFO[i], sizeof(DirectionalLightActorInfo), &dwBytesWrite, NULL);
		}

		delete[] DLAINFO;

		//Reflection Actor

		UINT RACount = m_ReflectionActorManager.m_RA_List.size();;
		WriteFile(hFile, &RACount, sizeof(RACount), &dwBytesWrite, NULL);

		ReflectionActorInfo* RAINFO = new ReflectionActorInfo[RACount];

		std::list<ReflectionActor*>::iterator RAi;
		//UINT i;
		for (i = 0, RAi = m_ReflectionActorManager.m_RA_List.begin(); i < RACount; i++, RAi++)
		{
			RAINFO[i].m_ActorInfo.m_WorldPosition = (*RAi)->GetWorldPosition();
			RAINFO[i].m_ActorInfo.m_Rotate = (*RAi)->GetRotate();
			RAINFO[i].m_ActorInfo.m_Scale = (*RAi)->GetScale();

			RAINFO[i].m_ActorInfo.m_WorldMat = (*RAi)->GetWorldMatrix();
			RAINFO[i].m_RAData.radius = (*RAi)->GetRadius();
			//RAINFO[i].m_LightData = (*li)->GetDirLightData();

			WriteFile(hFile, &RAINFO[i], sizeof(ReflectionActorInfo), &dwBytesWrite, NULL);
		}

		delete[] RAINFO;
		RAINFO = NULL;		

		//Assets
		//Geo

		UINT GeoCount = AssetDatabase::GetInstance()->m_ListofGeometry.size();
		WriteFile(hFile, &GeoCount, sizeof(GeoCount), &dwBytesWrite, NULL);

		AssetData_GeoListInfo* AD_GeoInfo = new AssetData_GeoListInfo[GeoCount];

		for (UINT i = 0; i < GeoCount; i++)
		{
			wcscpy_s(AD_GeoInfo[i].GeometryPath, MAX_PATH, AssetDatabase::GetInstance()->m_ListofGeometry.at(i));
			WriteFile(hFile, &AD_GeoInfo[i], sizeof(AssetData_GeoListInfo), &dwBytesWrite, NULL);
		}

		delete[] AD_GeoInfo;
		AD_GeoInfo = NULL;

		//Texture

		UINT TextureCount = AssetDatabase::GetInstance()->m_ListofTexture.size();
		WriteFile(hFile, &TextureCount, sizeof(TextureCount), &dwBytesWrite, NULL);

		AssetData_TextureListInfo* AD_TexInfo = new AssetData_TextureListInfo[TextureCount];

		for (UINT i = 0; i < TextureCount; i++)
		{
			wcscpy_s(AD_TexInfo[i].TexturePath, MAX_PATH, AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			WriteFile(hFile, &AD_TexInfo[i], sizeof(AssetData_TextureListInfo), &dwBytesWrite, NULL);
		}

		delete[] AD_TexInfo;
		AD_TexInfo = NULL;

		//Material

		UINT MaterialCount = AssetDatabase::GetInstance()->m_ListofMaterial.size();
		WriteFile(hFile, &MaterialCount, sizeof(MaterialCount), &dwBytesWrite, NULL);

		AssetData_MaterialListInfo* AD_MatInfo = new AssetData_MaterialListInfo[MaterialCount];

		for (UINT i = 0; i < MaterialCount; i++)
		{
			wcscpy_s(AD_MatInfo[i].MaterialPath, MAX_PATH, AssetDatabase::GetInstance()->m_ListofMaterial.at(i));

			UINT MaterialsTextureCount = 3;
		
			BasicMaterialEffect_Instance* pBE = AssetDatabase::GetInstance()->LoadAsset<BasicMaterialEffect_Instance>(AssetDatabase::GetInstance()->m_ListofMaterial.at(i));

			wcscpy_s(AD_MatInfo[i].MaterialTexturePath[0], MAX_PATH, pBE->m_strPath_DiffuseMap);
			wcscpy_s(AD_MatInfo[i].MaterialTexturePath[1], MAX_PATH, pBE->m_strPath_SpecularMap);
			wcscpy_s(AD_MatInfo[i].MaterialTexturePath[2], MAX_PATH, pBE->m_strPath_NormalMap);			
			
			WriteFile(hFile, &AD_MatInfo[i], sizeof(AssetData_MaterialListInfo), &dwBytesWrite, NULL);
		}

		delete[] AD_MatInfo;
		AD_MatInfo = NULL;

		//Objects

		UINT ObjCount = mObjectManager.mObj_List.size();

		WriteFile(hFile, &ObjCount, sizeof(ObjCount), &dwBytesWrite, NULL);

		ObjectInfo* OBJINFO = new ObjectInfo[ObjCount];

		/*
		for (UINT i = 0; i < ObjCount; i++)
		{
			ZeroMemory(OBJINFO[i].m_strPath, MAX_PATH);

			for (int j = 0; j < MAX_MATERIAL; j++)
			{
				for (int k = 0; k < MAX_TEXTURE; k++)
				{
					ZeroMemory(OBJINFO[i].m_strTexturePath[j][k] , MAX_PATH);
				}
			}			
		}
		*/

		std::list<Object*>::iterator oi;
		
		for (i = 0, oi = mObjectManager.mObj_List.begin(); oi != mObjectManager.mObj_List.end();i++, oi++)
		{
			(*oi)->GetObjectInfo(OBJINFO[i]);

			WriteFile(hFile, &OBJINFO[i], sizeof(ObjectInfo), &dwBytesWrite, NULL);
		}

		delete[] OBJINFO;

		CloseHandle(hFile);
	}
}

bool Renderer::Init()
{
	if (!CD3DApp::Init())
		return false;
	//m_d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			
	mDirectionalLightActorManager.LoadDLA(md3dDevice);
	m_ReflectionActorManager.LoadRA(md3dDevice);

	Create();

	OnResize();
	return true;
}

void Renderer::OnResize()
{
	CD3DApp::OnResize();
	
	mRenderTextureManger.OnResize((UINT)mClientWidth, (UINT)mClientHeight, md3dImmediateContext, mDepthStencilView);
	mMRTforDeferredR.OnResize(mClientWidth, mClientHeight, md3dImmediateContext, mDepthStencilView);

	mGizmo.OnResize(mClientWidth, mClientHeight);
	m_Scene.OnResize(mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	
	//m_SceneTexture->OnResize(mClientWidth, mClientHeight, md3dImmediateContext, mDepthStencilView);

	for (UINT i = 0; i < m_RenderCapsules.size(); i++)
	{
		m_RenderCapsules.at(i)->OnResize(mClientWidth, mClientHeight, mClientWidth, mClientHeight);
	}
	

	
	
	//m_reflectionTexture->OnResize(mClientWidth, mClientHeight, md3dImmediateContext, NULL);
	
	

	if (mDirectionalLightActorManager.mDLA_List.size() > 0)
	{
		for (std::list<DirectionalLightActor*>::iterator DLi = mDirectionalLightActorManager.mDLA_List.begin(); DLi != mDirectionalLightActorManager.mDLA_List.end(); DLi++)
		{
			if ((*DLi)->m_Shadow_Texture == NULL)
			{
				(*DLi)->m_Shadow_Texture = new RenderTexture();
				(*DLi)->m_Shadow_Texture->Initialize(md3dDevice, mClientWidth, mClientHeight, 1, 1, DXGI_FORMAT_R32_FLOAT,
					FALSE, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, 0);
			}			

			(*DLi)->UpdateFrustrum(mClientWidth, mClientHeight);
			(*DLi)->m_Shadow_Texture->OnResize(mClientWidth, mClientHeight, md3dImmediateContext, NULL);
		}
	}

	
	mCamera.SetLens(0.25f*MathHelper::Pi, AspectRatio(), NEAR_DIST, FAR_DIST*FAR_DIST);
	//mCamera.SetLensOrthographic((float)mClientWidth/16.0f, (float)mClientHeight / 16.0f, NEAR_DIST, FAR_DIST*FAR_DIST);
	mCamera.SetFrustumFromProjection();
	
	mCameraForRenderWindow.SetLens(0.25f*MathHelper::Pi, AspectRatio(), NEAR_DIST, FAR_DIST);
	//mCameraForRenderWindow.SetLensOrthographic((float)mClientWidth, (float)mClientHeight, NEAR_DIST, 10000.0f);
	mCameraForRenderWindow.SetFrustumFromProjection();

	mGizmoCamera.SetLensOrthographic((float)mClientWidth, (float)mClientHeight, 0.1f, 10000.0f);
	//mGizmoCamera.SetFrustumFromProjection();
}

void Renderer::UpdateScene(float dt)
{
	mEyePosW = mCamera.GetPosition();

	if (GetAsyncKeyState(VK_UP) & 0x8000)
		mCamera.Walk(100.0f*dt);

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		mCamera.Walk(-100.0f*dt);

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		mCamera.Strafe(-100.0f*dt);

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		mCamera.Strafe(100.0f*dt);
}

void Renderer::UpdateBasicMatrix(XMFLOAT4X4 ObjWorldMat, Camera* pCamera)
{
	////////// Set Basic Matrix
	XMMATRIX world = XMLoadFloat4x4(&ObjWorldMat);
	
	XMMATRIX view = pCamera->View();
	XMMATRIX proj = pCamera->Proj();
	XMMATRIX viewProj = pCamera->ViewProj();
	
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	XMMATRIX viewInv = MathHelper::Inverse(view);
	XMMATRIX projInv = MathHelper::Inverse(proj);
	XMMATRIX worldInv = MathHelper::Inverse(world);


	XMMATRIX worldViewProj = world*viewProj;


	XMStoreFloat4x4(&mView, view);
	XMStoreFloat4x4(&mProj, proj);
	XMStoreFloat4x4(&mViewProj, viewProj);

	XMStoreFloat4x4(&mWVP, worldViewProj);
	XMStoreFloat4x4(&mInvWorld, worldInv);
	XMStoreFloat4x4(&mInvProj, projInv);
	XMStoreFloat4x4(&mInvView, viewInv);
	
	XMStoreFloat4x4(&mInvViewProj, MathHelper::Inverse(viewProj));
	XMStoreFloat4x4(&mInvWorldViewProj, MathHelper::Inverse(worldViewProj));
	XMStoreFloat4x4(&mInvWorldTranspo, worldInvTranspose);
}


void Renderer::UpdateBasicMatrixforDL(XMFLOAT4X4 ObjWorldMat, DirectionalLightActor* pDL)
{
	////////// Set Basic Matrix
	XMMATRIX world = XMLoadFloat4x4(&ObjWorldMat);

	XMMATRIX view = pDL->View();
	XMMATRIX proj = pDL->Proj();
	XMMATRIX viewProj = pDL->ViewProj();

	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

	XMMATRIX viewInv = MathHelper::Inverse(view);
	XMMATRIX projInv = MathHelper::Inverse(proj);
	XMMATRIX worldInv = MathHelper::Inverse(world);


	XMMATRIX worldViewProj = world*viewProj;


	XMStoreFloat4x4(&mView, view);
	XMStoreFloat4x4(&mProj, proj);
	XMStoreFloat4x4(&mViewProj, viewProj);

	XMStoreFloat4x4(&mWVP, worldViewProj);
	XMStoreFloat4x4(&mInvWorld, worldInv);
	XMStoreFloat4x4(&mInvProj, projInv);
	XMStoreFloat4x4(&mInvView, viewInv);

	XMStoreFloat4x4(&mInvViewProj, MathHelper::Inverse(viewProj));
	XMStoreFloat4x4(&mInvWorldViewProj, MathHelper::Inverse(worldViewProj));
	XMStoreFloat4x4(&mInvWorldTranspo, worldInvTranspose);
}


int Renderer::Check_IntersectAxisAlignedBoxFrustum(Object* pObject)
{
	BoundingFrustum localspaceFrustum;
	
	BoundingBox XX;

	pObject->GetAABB().Transform(XX, DirectX::XMMatrixMultiply(XMLoadFloat4x4(&pObject->GetWorldMatrix()), XMLoadFloat4x4(&mView)));

	return mCamera.GetFrustum().Intersects(XX);		
}

void Renderer::ResortRa()
{
	if (m_ReflectionActorManager.m_RA_List.size() < 2)
		return;

	std::list<ReflectionActor*>::iterator Rai = m_ReflectionActorManager.m_RA_List.end();
	Rai--;
	std::list<ReflectionActor*>::iterator fRai = m_ReflectionActorManager.m_RA_List.end();
	fRai--;
	fRai--;
	std::list<ReflectionActor*>::iterator rRai = m_ReflectionActorManager.m_RA_List.begin();
	//rRai++;


	for (; ; Rai--, fRai--)
	{
		if ((*Rai)->GetRadius() < (*fRai)->GetRadius())
		{
			ReflectionActor* temp = (*Rai);
			(*Rai) = (*fRai);
			(*fRai) = temp;

			//Rai = fRai;
			//fRai--;
		}

		if (fRai == rRai)
			break;

	}
}


void Renderer::OnAddReflectionActor()
{
	m_ReflectionActorManager.LoadRA(md3dDevice);

	ResortRa();
}

void Renderer::DrawScene()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	mCamera.UpdateViewMatrix();

	if (bShowWireframe)
	{
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
	}

	DrawRender();	

	DrawUI();

	DrawDebug();	
	
	
	ReleaseBackBufferRenderTarget();
	///////// FINISH BackBuffer Texture

	HR(mSwapChain->Present(0, 0));
}

void Renderer::DrawRender()
{
	for(UINT i = 0; i < m_RenderCapsules.size(); i++)
	{
		if(m_RenderCapsules.at(i)->RenderPriority < UI)
			m_RenderCapsules.at(i)->Render(md3dImmediateContext, 0, 0, mInvViewProj, NULL, &mEffects);
	}
}

void Renderer::DrawDebug()
{
	for (UINT i = 0; i < m_RenderCapsules.size(); i++)
	{
		if (bshowDebugWindows && m_RenderCapsules.at(i)->RenderPriority >= DEBUG_RENDER)
			m_RenderCapsules.at(i)->Render(md3dImmediateContext, 0, 0, mInvViewProj, NULL, &mEffects);
	}
	
}

void Renderer::DrawUI()
{
	ID3DX11EffectTechnique* activeTech = NULL;

	//////////<---------Draw OUTLINE
	if (bSelectedObjExist)
	{
		SetBackBufferRenderTarget(mRenderTextureManger.mRT_List.at(SILHOUETTE)->GetRenderTargetView(), NULL, NULL, NULL, FALSE, TRUE, FALSE, FALSE);

		//for(UINT i = 0; i < mFbxManager.mFbx_List.size(); i++)

		for (std::list<Object*>::iterator li = mObjectManager.mObj_List.begin(); li != mObjectManager.mObj_List.end(); li++)
		{
			// Incomplete
			for (UINT j = 0; j < (*li)->GetLayerCount(); j++)
			{
				if ((*li)->bSelected)
				{
					UpdateBasicMatrix((*li)->GetWorldMatrix(), &mCamera);

					// 0: not intersect 1: intersect 2: include
					if (Check_IntersectAxisAlignedBoxFrustum((*li)) != 0)
					{
						mEffects.OutLineDrawFx->SetAllVariables(XMLoadFloat4x4(&mViewProj), XMLoadFloat4x4(&mWVP), NULL, XMFLOAT2((float)mClientWidth, (float)mClientHeight));

						activeTech = mEffects.OutLineDrawFx->FillSilhouetteTech;
						D3DX11_TECHNIQUE_DESC techDesc;
						activeTech->GetDesc(&techDesc);

						(*li)->Draw(md3dImmediateContext, activeTech, techDesc);
					}

					break;
				}

			}
		}

		SetBackBufferRenderTarget(FALSE, FALSE, FALSE, FALSE);
		//UpdateBasicMatrix(mWorld, &mCamera);
		UpdateBasicMatrix(mWorld, &mCameraForRenderWindow);

		SetBlendState(FALSE, FALSE, TRUE, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_ONE, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);
		mEffects.OutLineDrawFx->SetAllVariables(XMLoadFloat4x4(&mViewProj), XMLoadFloat4x4(&mWVP), mRenderTextureManger.mRT_List.at(SILHOUETTE)->GetShaderResourceView(), XMFLOAT2((float)mClientWidth, (float)mClientHeight));
		activeTech = mEffects.OutLineDrawFx->OutLineTech;

		md3dImmediateContext->IASetInputLayout(InputLayouts::BRDFL_VID);
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//m_Scene.Render(md3dImmediateContext, 0, 0, mInvViewProj, activeTech, mEffects.OutLineDrawFx);

		m_Scene.Render(md3dImmediateContext, 0, 0, mInvViewProj, activeTech, &mEffects);

		ReleaseBlendState();
		UpdateBasicMatrix(mWorld, &mCamera);
	}
	//////////<---------Draw OUTLINE



	///////// Set BackBuffer Texture	
	//SetBackBufferRenderTarget(mRenderTargetView, mDepthStencilView, NULL, 0, TRUE, FALSE, TRUE, FALSE);
	

	//////////<--------- Draw BoundingBox
	if (bshowBoundingBox)
	{
		SetBackBufferRenderTarget(TRUE, FALSE, FALSE, FALSE);

		activeTech = mEffects.LineDrawFx->mTech;

		D3DX11_TECHNIQUE_DESC techDesc3;
		activeTech->GetDesc(&techDesc3);

		md3dImmediateContext->IASetInputLayout(InputLayouts::Line_VID);
		md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		//for (UINT i = 0; i < mFbxManager.mFbx_List.size(); i++)

		for (std::list<Object*>::iterator li = mObjectManager.mObj_List.begin(); li != mObjectManager.mObj_List.end(); li++)
		{
			// Incomplete
			for (UINT j = 0; j < (*li)->GetLayerCount(); j++)
			{
				//if (mFbxManager.mFbx_List.at(i).bSelected)
				//{
				UpdateBasicMatrix((*li)->GetWorldMatrix(), &mCamera);

				// 0: not intersect 1: intersect 2: include
				if (Check_IntersectAxisAlignedBoxFrustum((*li)) != 0)
				{
					mEffects.LineDrawFx->SetAllVariables(XMLoadFloat4x4(&mWVP), (*li)->bSelected);					

					(*li)->DrawBoundingBox(md3dImmediateContext, activeTech, techDesc3);
				}
				//}

			}
		}

	}
	///////// ---------> Draw BoundingBox


	///////// <--------- Draw UISprite
	

	if (bshowUISprite)
	{
		SetBackBufferRenderTarget(TRUE, FALSE, TRUE, FALSE);

		activeTech = mEffects.UISpriteFx->UISpriteTech;
		D3DX11_TECHNIQUE_DESC UISpritetechDesc;
		activeTech->GetDesc(&UISpritetechDesc);

		for (std::list<DirectionalLightActor*>::iterator DLi = mDirectionalLightActorManager.mDLA_List.begin(); DLi != mDirectionalLightActorManager.mDLA_List.end(); DLi++)
		{
			UpdateBasicMatrix((*DLi)->GetWorldMatrix(), &mCamera);
			(*DLi)->Render(md3dImmediateContext, mCamera.GetPosition(), &mEffects, mView, mProj, activeTech, UISpritetechDesc, m_UITextureList[0].GetShaderResourceView());
		}

		for (std::list<ReflectionActor*>::iterator RAi = m_ReflectionActorManager.m_RA_List.begin(); RAi != m_ReflectionActorManager.m_RA_List.end(); RAi++)
		{
			UpdateBasicMatrix((*RAi)->GetWorldMatrix(), &mCamera);
			(*RAi)->Render(md3dImmediateContext, mCamera.GetPosition(), &mEffects, mView, mProj, activeTech, UISpritetechDesc, m_UITextureList[1].GetShaderResourceView());
		}
	}



	///////// ---------> Draw UISprite


	///////// <--------- Draw Gizmo
	SetBackBufferRenderTarget(TRUE, FALSE, TRUE, FALSE);

	//WorldGizmo

	md3dImmediateContext->IASetInputLayout(InputLayouts::BRDFL_VID);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	mGizmoCamera.SetPosition(XMFLOAT3(-mCamera.GetLook().x*100.0f, -mCamera.GetLook().y*100.0f, -mCamera.GetLook().z*100.0f));
	mGizmoCamera.SetLook(mCamera.GetLook());
	mGizmoCamera.SetRight(mCamera.GetRight());
	mGizmoCamera.SetUp(mCamera.GetUp());
	mGizmoCamera.UpdateViewMatrix();

	UpdateBasicMatrix(mWorld, &mGizmoCamera);

	activeTech = mEffects.GizmoDrawFx->mTech;

	D3DX11_TECHNIQUE_DESC techDesc4;
	activeTech->GetDesc(&techDesc4);
	mGizmo.Render(md3dImmediateContext, activeTech, &mEffects, mWorld, mWVP, -0.75f, -0.75f, 6.0f, 0);


	//SelectedGizmo
	if (bSelectedObjExist && mGizmo.GetSelectedObj() != NULL)
	{
		UpdateBasicMatrix(mWorld, &mCamera);

		XMFLOAT4X4 objWorldMat = mGizmo.GetSelectedObj()->GetWorldMatrix();

		XMVECTOR scale;
		XMVECTOR rotQuat;
		XMVECTOR translation;

		DirectX::XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&objWorldMat));

		XMFLOAT3 T = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);

		XMStoreFloat4x4(&objWorldMat, DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixRotationQuaternion(rotQuat)), DirectX::XMMatrixTranslation(T.x, T.y, T.z)));


		XMFLOAT4 WorldPosition = MathHelper::XMFLOAT4_DIV_W(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(mGizmo.GetSelectedObj()->GetOriginPointPos(), mViewProj));

		activeTech = mEffects.GizmoDrawFx->TransTech;
		D3DX11_TECHNIQUE_DESC techDesc4;
		activeTech->GetDesc(&techDesc4);
		XMFLOAT3 OPP = mGizmo.GetSelectedObj()->GetOriginPointPos();

		mGizmoCamera.SetPosition(XMFLOAT3(OPP.x - mCamera.GetLook().x*310.0f, OPP.y - mCamera.GetLook().y*310.0f, OPP.z - mCamera.GetLook().z*310.0f));
		//mGizmoCamera.SetPosition(mCamera.GetPosition());
		mGizmoCamera.SetLook(mCamera.GetLook());
		mGizmoCamera.SetRight(mCamera.GetRight());
		mGizmoCamera.SetUp(mCamera.GetUp());
		mGizmoCamera.UpdateViewMatrix();

		mGizmo.m_SelectedObjScreenPos = XMFLOAT2(WorldPosition.x, WorldPosition.y);

		UpdateBasicMatrix(objWorldMat, &mGizmoCamera);

		if (mGizmo.GetPushedAxis() == 0)
			mGizmo.HoverT(mLastMousePos.x, mLastMousePos.y, WorldPosition.x, WorldPosition.y, mProj, mView, objWorldMat);

		md3dImmediateContext->IASetInputLayout(InputLayouts::GIZMO_VID);


		mGizmo.Render(md3dImmediateContext, activeTech, &mEffects, objWorldMat, mWVP, WorldPosition.x, WorldPosition.y, 1.0f, mGizmo.mGizmoStatus + 1);

	}
	else if (bSelectedActorExist && mGizmo.GetSelectedActor() != NULL)
	{
		UpdateBasicMatrix(mWorld, &mCamera);

		XMFLOAT4X4 objWorldMat = mGizmo.GetSelectedActor()->GetWorldMatrix();

		XMVECTOR scale;
		XMVECTOR rotQuat;
		XMVECTOR translation;

		DirectX::XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&objWorldMat));

		XMFLOAT3 T = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);

		XMStoreFloat4x4(&objWorldMat, DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixRotationQuaternion(rotQuat)), DirectX::XMMatrixTranslation(T.x, T.y, T.z)));


		XMFLOAT4 WorldPosition = MathHelper::XMFLOAT4_DIV_W(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(mGizmo.GetSelectedActor()->GetOriginPointPos(), mViewProj));

		activeTech = mEffects.GizmoDrawFx->TransTech;
		D3DX11_TECHNIQUE_DESC techDesc4;
		activeTech->GetDesc(&techDesc4);
		XMFLOAT3 OPP = mGizmo.GetSelectedActor()->GetOriginPointPos();

		mGizmoCamera.SetPosition(XMFLOAT3(OPP.x - mCamera.GetLook().x*310.0f, OPP.y - mCamera.GetLook().y*310.0f, OPP.z - mCamera.GetLook().z*310.0f));
		mGizmoCamera.SetLook(mCamera.GetLook());
		mGizmoCamera.SetRight(mCamera.GetRight());
		mGizmoCamera.SetUp(mCamera.GetUp());
		mGizmoCamera.UpdateViewMatrix();

		mGizmo.m_SelectedObjScreenPos = XMFLOAT2(WorldPosition.x, WorldPosition.y);

		UpdateBasicMatrix(objWorldMat, &mGizmoCamera);

		if (mGizmo.GetPushedAxis() == 0)
			mGizmo.HoverT(mLastMousePos.x, mLastMousePos.y, WorldPosition.x, WorldPosition.y, mProj, mView, objWorldMat);

		md3dImmediateContext->IASetInputLayout(InputLayouts::GIZMO_VID);


		mGizmo.Render(md3dImmediateContext, activeTech, &mEffects, objWorldMat, mWVP, WorldPosition.x, WorldPosition.y, 1.0f, mGizmo.mGizmoStatus + 1);

	}
	///////// ---------> Draw Gizmo

	///////// <--------- Draw ActorGizmo

	if (bSelectedActorExist && mGizmo.GetSelectedActor() != NULL)
	{
		UpdateBasicMatrix(mWorld, &mCamera);

		XMFLOAT4 WorldPosition = MathHelper::XMFLOAT4_DIV_W(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(mGizmo.GetSelectedActor()->GetOriginPointPos(), mViewProj));

		Actor* pActor = mGizmo.GetSelectedActor();

		/*
		DirectionalLightActor* pDirectionalLightActor = dynamic_cast<DirectionalLightActor*>(pActor);
		if (pDirectionalLightActor)
		{

		continue;
		}
		*/



		ReflectionActor* pReflectionActor = dynamic_cast<ReflectionActor*>(pActor);

		if (pReflectionActor)
		{
			//pReflectionActor->DrawActorGizmo(md3dImmediateContext,  &mEffects, &XMLoadFloat4x4(&objWorldMat), &mWVP, WorldPosition.x, WorldPosition.y, &(pl));
			UpdateBasicMatrix(pReflectionActor->GetWorldMatrix(), &mCamera);
			pReflectionActor->DrawActorGizmo(md3dImmediateContext, &mEffects, &XMLoadFloat4x4(&mWorld), &mWVP, WorldPosition.x, WorldPosition.y, pReflectionActor->GetRadius());
		}
	}
}

void Renderer::SetBlendState(bool bAlphaToCoverageEnable, bool bIndependentBlendEnable, bool bBlendEnable,
	D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask)
{
	D3D11_BLEND_DESC BD;
	BD.AlphaToCoverageEnable = bAlphaToCoverageEnable;
	BD.IndependentBlendEnable = bIndependentBlendEnable;
	BD.RenderTarget[0].BlendEnable = bBlendEnable;
	BD.RenderTarget[0].SrcBlend = SrcBlend;
	BD.RenderTarget[0].DestBlend = DestBlend;
	BD.RenderTarget[0].BlendOp = BlendOp;
	BD.RenderTarget[0].SrcBlendAlpha = SrcBlendAlpha;
	BD.RenderTarget[0].DestBlendAlpha = DestBlendAlpha;
	BD.RenderTarget[0].BlendOpAlpha = BlendOpAlpha;
	BD.RenderTarget[0].RenderTargetWriteMask = RenderTargetWriteMask;
	
	HR(md3dDevice->CreateBlendState(&BD, &mBlendState));
	md3dImmediateContext->OMSetBlendState(mBlendState, NULL, 0xffffffff);
}

void Renderer::ReleaseBlendState()
{
	md3dImmediateContext->OMSetBlendState(NULL, NULL, 0xffffffff);

	ReleaseCOM(mBlendState);
}

void Renderer::OnImportTexture(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH])
{
	WCHAR* CopiedPath = new WCHAR[MAX_PATH];
	wcscpy_s(CopiedPath, MAX_PATH, pModelPath);

	AssetDatabase::GetInstance()->LoadAsset<Texture>(CopiedPath);
	AssetDatabase::GetInstance()->m_ListofTexture.push_back(CopiedPath);
}

void Renderer::OnImportGeometry(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH])
{
	WCHAR* CopiedPath = new WCHAR[MAX_PATH];
	wcscpy_s(CopiedPath, MAX_PATH, pModelPath);

	AssetDatabase::GetInstance()->LoadAsset<Geometry>(CopiedPath);
	AssetDatabase::GetInstance()->m_ListofGeometry.push_back(CopiedPath);

	Geometry* pGeo = AssetDatabase::GetInstance()->LoadAsset<Geometry>(CopiedPath);
}

void Renderer::OnCreateMaterial(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH])
{
	WCHAR* CopiedPath = new WCHAR[MAX_PATH];
	wcscpy_s(CopiedPath, MAX_PATH, pModelPath);
	
	AssetDatabase::GetInstance()->LoadAsset<BasicMaterialEffect_Instance>(CopiedPath);
	AssetDatabase::GetInstance()->m_ListofMaterial.push_back(CopiedPath);

	BasicMaterialEffect_Instance * pNewMI = AssetDatabase::GetInstance()->LoadAsset<BasicMaterialEffect_Instance>(CopiedPath);

	for (UINT i = 0; i < AssetDatabase::GetInstance()->m_ListofMaterial.size(); i++)
	{
		if (wcscmp(pModelPath, AssetDatabase::GetInstance()->m_ListofMaterial.at(i)) == 0)
		{
			pNewMI = AssetDatabase::GetInstance()->LoadAsset<BasicMaterialEffect_Instance>(AssetDatabase::GetInstance()->m_ListofMaterial.at(i));  // new BasicMaterialEffect_Instance;
			wcscpy_s(pNewMI->m_strTitle, MAX_PATH, AssetDatabase::GetInstance()->m_ListofMaterial.at(i));
			break;
		}
	}
	pNewMI->pParentMaterial = mEffects.BasicFX;

	//DiffuseMap
	for (UINT i = 0; i < AssetDatabase::GetInstance()->m_ListofTexture.size(); i++)
	{
		if (wcscmp(pTexturePathList[0], AssetDatabase::GetInstance()->m_ListofTexture.at(i)) == 0)
		{
			pNewMI->m_DiffuseMap = AssetDatabase::GetInstance()->LoadAsset<Texture>(AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			wcscpy_s(pNewMI->m_strPath_DiffuseMap, MAX_PATH, AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			break;
		}
	}

	//SpecularMap
	for (UINT i = 0; i < AssetDatabase::GetInstance()->m_ListofTexture.size(); i++)
	{
		if (wcscmp(pTexturePathList[1], AssetDatabase::GetInstance()->m_ListofTexture.at(i)) == 0)
		{
			pNewMI->m_SpecularMap = AssetDatabase::GetInstance()->LoadAsset<Texture>(AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			wcscpy_s(pNewMI->m_strPath_SpecularMap, MAX_PATH, AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			break;
		}
	}

	//NormalMap
	for (UINT i = 0; i < AssetDatabase::GetInstance()->m_ListofTexture.size(); i++)
	{
		if (wcscmp(pTexturePathList[2], AssetDatabase::GetInstance()->m_ListofTexture.at(i)) == 0)
		{
			pNewMI->m_NormalMap = AssetDatabase::GetInstance()->LoadAsset<Texture>(AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			wcscpy_s(pNewMI->m_strPath_NormalMap, MAX_PATH, AssetDatabase::GetInstance()->m_ListofTexture.at(i));
			break;
		}
	}	
}

void Renderer::OnImportModel(WCHAR* pModelPath, UINT MaterialNum, WCHAR* Material)
{
	Object* pObj = new Object;

	for (UINT i = 0; i < AssetDatabase::GetInstance()->m_ListofGeometry.size(); i++)
	{
		if (wcscmp(pModelPath, AssetDatabase::GetInstance()->m_ListofGeometry.at(i)) == 0)
		{
			pObj->pGeo = AssetDatabase::GetInstance()->FindAsset<Geometry>(AssetDatabase::GetInstance()->m_ListofGeometry.at(i));
			
			break;
		}
	}
	
	for (UINT i = 0; i < AssetDatabase::GetInstance()->m_ListofMaterial.size(); i++)
	{
		if (wcscmp(Material, AssetDatabase::GetInstance()->m_ListofMaterial.at(i)) == 0)
		{
			pObj->pMI = AssetDatabase::GetInstance()->FindAsset<BasicMaterialEffect_Instance>(AssetDatabase::GetInstance()->m_ListofMaterial.at(i));

			break;
		}
	}

	mObjectManager.mObj_List.push_back(pObj);
	
}


void Renderer::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		mLastMousePos.x = x;
		mLastMousePos.y = y;

		SetCapture(mhMainWnd);			

		if (mGizmo.GetselectedTAxis() > 0)
		{
			mGizmo.SetPushedAxis(mGizmo.GetselectedTAxis());
		}
		else
		{
			UpdateBasicMatrix(mWorld, &mCamera);

			if (bshowUISprite)
			{
				if (PickUISprite(x, y))
				{
					std::list<Object*>::iterator li2 = mObjectManager.mObj_List.begin();
					for (UINT i = 0; i < mObjectManager.mObj_List.size(); i++, li2++)
					{
						(*li2)->bSelected = false;
					}
				}
				
				bSelectedObjExist = false;
			}
			
			if (!bSelectedActorExist)
			{				
				Pick(x, y);
				bSelectedActorExist = false;
			}
			

			
		}
	}	
}

void Renderer::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();

	if (mGizmo.GetPushedAxis() != 0)
	{
		mGizmo.SetPushedAxis(0);
	}

	if (mGizmo.GetselectedTAxis() > 0)
	{
		mGizmo.SetselectedTAxis(0);
	}
}

void Renderer::OnMouseMove(WPARAM btnState, int x, int y)
{

	if ((btnState & MK_LBUTTON) != 0)
	{
		
		if (mGizmo.GetPushedAxis() != 0)
		{
			if (bSelectedObjExist)
			{
				UpdateBasicMatrix(mGizmo.GetSelectedObj()->GetWorldMatrix(), &mGizmoCamera);
				mGizmo.OnMouseMove(mTimer.DeltaTime(), x, y, mLastMousePos.x, mLastMousePos.y, mClientWidth, mClientHeight, &mWorld, &mWVP, mGizmoCamera.GetLook(), 0);
				UpdateBasicMatrix(mWorld, &mCamera);
			}
			else if (bSelectedActorExist)
			{
				UpdateBasicMatrix(mGizmo.GetSelectedActor()->GetWorldMatrix(), &mGizmoCamera);
				mGizmo.OnMouseMove(mTimer.DeltaTime(), x, y, mLastMousePos.x, mLastMousePos.y, mClientWidth, mClientHeight, &mWorld, &mWVP, mGizmoCamera.GetLook(), 1);
				UpdateBasicMatrix(mWorld, &mCamera);
			}
			
		}		
	}	

	if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(50.0f*mTimer.DeltaTime()*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(50.0f*mTimer.DeltaTime()*static_cast<float>(y - mLastMousePos.y));

		mCamera.Pitch(dy/*0.2f*/);
		mCamera.RotateY(dx/*0.2f*/);
	}
	
	mLastMousePos.x = x;
	mLastMousePos.y = y;


}

void Renderer::OnMouseHover(WPARAM btnState, int x, int y)
{
	
}

void Renderer::OnMouseWheel(WPARAM btnState, int x, int y)
{	
	// Make each pixel correspond to 0.005 unit in the scene.
	float dx = 25.0f*static_cast<float>(x - mLastMousePos.x)/mClientWidth;
	float dy = 25.0f*static_cast<float>(y - mLastMousePos.y)/mClientHeight;

	if (bSelectedActorExist)
	{
		std::list<ReflectionActor*>::iterator RAi2 = m_ReflectionActorManager.m_RA_List.begin();
		for (UINT i = 0; i < m_ReflectionActorManager.m_RA_List.size(); i++, RAi2++)
		{
			if ((*RAi2)->bSelected == true)
			{
				if ((GET_WHEEL_DELTA_WPARAM(btnState)) < 0)
				{
					(*RAi2)->MnRadius(mTimer.DeltaTime()*100.0f*(dx - dy));
				}
				else if ((GET_WHEEL_DELTA_WPARAM(btnState)) > 0)
				{
					(*RAi2)->MnRadius(mTimer.DeltaTime()*-100.0f*(dx - dy));
				}	

				ResortRa();
			}
		}
	}
	else
	{
		//fwKeys = GET_KEYSTATE_WPARAM(wParam);
		if ((GET_WHEEL_DELTA_WPARAM(btnState)) < 0)
		{

			mCamera.Walk(mTimer.DeltaTime()*100.0f*(dx - dy));
		}
		else if ((GET_WHEEL_DELTA_WPARAM(btnState)) > 0)
		{

			mCamera.Walk(mTimer.DeltaTime()*-100.0f*(dx - dy));
		}

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	
}

void Renderer::OnSystemKeyDown(WPARAM btnState)
{
	//Alt + Enter
	if (btnState == VK_RETURN)
	{
		//m_OldClientWidth = mClientWidth;
		//m_OldClientHeight = mClientHeight;

		m_bIsGointoFullScreen = !m_bIsGointoFullScreen;
		OnResize();
	}
}

void Renderer::OnKeyDown(WPARAM btnState)
{

	if (btnState == VK_HOME)
	{
		mCamera.RotateY(mTimer.DeltaTime()*20.0f);
	}

	if (btnState == VK_END)
	{
		mCamera.RotateY(mTimer.DeltaTime()*-20.0f);
	}

	if (btnState == VK_F11)
	{
		mCamera.Pitch(mTimer.DeltaTime()*20.0f);
	}

	if (btnState == VK_F12)
	{
		mCamera.Pitch(mTimer.DeltaTime()*-20.0f);
	}

	if (btnState == (65 + 'x' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->bRotate = !(pSelectedObj->bRotate);
		}
	}

	// 'B'
	if (btnState == 66)
	{
		bshowBoundingBox = !bshowBoundingBox;
	}

	// 'D'
	if (btnState == 68)
	{
		bshowDebugWindows = !bshowDebugWindows;
		OnResize();
	}

	// 'W'
	if (btnState == 87)
	{
		bShowWireframe = !bShowWireframe;		
	}

	if (btnState == (65 + 's' - 'a'))
	{
		bshowUISprite = !bshowUISprite;

		if(!bshowUISprite)
		{
			std::list<DirectionalLightActor*>::iterator li2 = mDirectionalLightActorManager.mDLA_List.begin();
			for (UINT i = 0; i < mDirectionalLightActorManager.mDLA_List.size(); i++, li2++)
			{
				(*li2)->bSelected = false;
			}

			std::list<ReflectionActor*>::iterator RAi2 = m_ReflectionActorManager.m_RA_List.begin();
			for (UINT i = 0; i < m_ReflectionActorManager.m_RA_List.size(); i++, RAi2++)
			{
				(*RAi2)->bSelected = false;
			}

			bSelectedActorExist = false;
			mGizmo.SetSelectedActor(NULL);
			
		}
	}

	// 'Y' //X 90
	if (btnState == (65 + 'y' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(DirectX::XMFLOAT3(1, 0, 0), 1.0f), 15.0f);			
		}
	}

	// 'H' //X -90
	if (btnState == (65 + 'h' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(DirectX::XMFLOAT3(1, 0, 0), 1.0f), -15.0f);
		}
	}

	// 'U' //Y 90
	if (btnState == (65 + 'u' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(DirectX::XMFLOAT3(0, 1, 0), 1.0f), 15.0f);
		}
	}

	// 'J' //Y -90
	if (btnState == (65 + 'j' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(DirectX::XMFLOAT3(0, 1, 0), 1.0f), -15.0f);
		}
	}

	// 'I' //Z 90
	if (btnState == (65 + 'i' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(DirectX::XMFLOAT3(0, 0, 1), 1.0f), 15.0f);
		}
	}

	// 'K' //Z -90
	if (btnState == (65 + 'k' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(DirectX::XMFLOAT3(0, 0, 1), 1.0f), -15.0f);
		}
	}


	// 'O' //X + 1
	if (btnState == (65 + 'o' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Translation(1.0f, 0.0f, 0.0f);
		}
	}

	// 'L' //X - 1
	if (btnState == (65 + 'l' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Translation(-1.0f, 0.0f, 0.0f);
		}
	}

	// 'P' //X + 1
	if (btnState == (65 + 'p' - 'a'))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Translation(0.0f, 1.0f, 0.0f);
		}
	}

	// ';' //X - 1
	if (btnState == (186))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Translation(0.0f, -1.0f, 0.0f);
		}
	}

	// '[' //X + 1
	if (btnState == (219))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Translation(0.0f, 0.0f, 1.0f);
		}
	}

	// ''' //X - 1
	if (btnState == (222))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Translation(0.0f, 0.0f, -1.0f);
		}
	}

	// ',' //S + 1
	if (btnState == (188))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Scale(0.1f, 0.1f, 0.1f);
		}
	}

	// '.' //S - 1
	if (btnState == (190))
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();
			pSelectedObj->Scale(-0.1f, -0.1f, -0.1f);
		}
	}


	// 'A'
	if (btnState == (65))
	{
		for (std::list<ReflectionActor*>::iterator RAi = m_ReflectionActorManager.m_RA_List.begin(); RAi != m_ReflectionActorManager.m_RA_List.end(); RAi++)
		{			
				md3dImmediateContext->RSSetState(RenderStates::NormalRS);
				ID3D11InputLayout* temp = GetIASetInputLayout();
				SetIASetInputLayout(InputLayouts::GIZMO_VID);

				md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				(*RAi)->CreateCubeMap(md3dDevice, md3dImmediateContext, &mObjectManager, &mEffects, m_SkyBox.m_VBforSkyBox, m_SkyBox.m_IBforSkyBox, m_SkyBox.m_SkyCubeMap_SRV, &XMLoadFloat4x4(&mBoxWorld),
					 mCamera.GetPosition() ,mMRTforDeferredR.GetShaderResourceView(3), mMRTforDeferredR.GetShaderResourceView(2));

				SetIASetInputLayout(temp);			
		}

		// Bind the render target view and depth/stencil view to the pipeline.
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
		md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	}


	// 'R'
	if (btnState == (65 + 'r' - 'a'))
	{
		for (std::list<ReflectionActor*>::iterator RAi = m_ReflectionActorManager.m_RA_List.begin(); RAi != m_ReflectionActorManager.m_RA_List.end(); RAi++)
		{
			if ((*RAi)->bSelected)
			{
				md3dImmediateContext->RSSetState(RenderStates::NormalRS);

				ID3D11InputLayout* temp = GetIASetInputLayout();

				SetIASetInputLayout(InputLayouts::GIZMO_VID);
									
				md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				(*RAi)->CreateCubeMap(md3dDevice, md3dImmediateContext, &mObjectManager ,&mEffects, m_SkyBox.m_VBforSkyBox, m_SkyBox.m_IBforSkyBox, m_SkyBox.m_SkyCubeMap_SRV, &XMLoadFloat4x4(&mBoxWorld),
					mCamera.GetPosition(), mMRTforDeferredR.GetShaderResourceView(3), mMRTforDeferredR.GetShaderResourceView(2));

				SetIASetInputLayout(temp);
				
				break;
			}
		}

		// Bind the render target view and depth/stencil view to the pipeline.
		md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
		md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	}

	if (btnState == VK_DELETE )
	{
		if (bSelectedObjExist)
		{
			Object* pSelectedObj = mGizmo.GetSelectedObj();

			std::list<Object*>::iterator oi;

			for (oi = mObjectManager.mObj_List.begin(); oi != mObjectManager.mObj_List.end(); oi++)
			{
				Object* pObj = (*oi);

				if (pSelectedObj == pObj)
				{
					pObj->Shutdown();
					delete pObj;
					mObjectManager.mObj_List.erase(oi);

					bSelectedObjExist = false;

					break;
				}
			}
		}
	}

	if (btnState == VK_SPACE)
	{
		mGizmo.mGizmoStatus++;
		if (mGizmo.mGizmoStatus >= 3)
			mGizmo.mGizmoStatus -= 3;		
	}
}

void Renderer::OnWMSize(WPARAM wParam)
{
	if (wParam == SIZE_MINIMIZED)
	{
		
	}
	else if (wParam == SIZE_MAXIMIZED)
	{		
		OnResize();
	}
	else if (wParam == SIZE_RESTORED)
	{
		// Restoring from minimized state?
		if (mMinimized)
		{			
			OnResize();
		}

		// Restoring from maximized state?
		else if (mMaximized)
		{			
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

BOOL Renderer::Pick(int sx, int sy)
{
	XMMATRIX P = XMLoadFloat4x4(&mProj);// mCamera.Proj();

	// Compute picking ray in view space.
	float vx = (+2.0f*sx / mClientWidth - 1.0f) / mProj._11;// P(0, 0);
	float vy = (-2.0f*sy / mClientHeight + 1.0f) / mProj._22;//P(1, 1);

	//float vx = (+2.0f*( (float)sx / (float)mClientWidth - 0.5f));// / mProj._11;// P(0, 0);
	//float vy = (2.0f*(0.5f - (float)sy / (float)mClientHeight));// / mProj._22;//P(1, 1);

	

	// Tranform ray to local space of Mesh.
	XMMATRIX View = XMLoadFloat4x4(&mView);

	mPickedMesh = -1;
	float mindist = MathHelper::Infinity;
	//for (UINT i = 0; i < mFbxManager.mFbx_List.size(); i++)
	std::list<Object*>::iterator li; UINT c = 0;
	for (li = mObjectManager.mObj_List.begin(); li != mObjectManager.mObj_List.end(); li++, c++)
	{		
		// Ray definition in view space.
		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX W = XMLoadFloat4x4(&(*li)->GetWorldMatrix());
		//XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);
		XMMATRIX toLocal = MathHelper::Inverse(DirectX::XMMatrixMultiply(W, View));

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);

		// Make the ray direction unit length for the intersection tests.
		rayDir = XMVector3Normalize(rayDir);

		// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		// so do the ray/triangle tests.
		//
		// If we did not hit the bounding box, then it is impossible that we hit 
		// the Mesh, so do not waste effort doing ray/triangle tests.

		// Assume we have not picked anything yet, so init to -1.
		//mPickedTriangle = -1;
		
		float tmin = 0.0f;
		
		//XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &(*li)->GetAABB(), &tmin)
		if ((*li)->GetAABB().Intersects(rayOrigin, rayDir, tmin))
		{
			// Find the nearest ray/triangle intersection.
			//tmin = MathHelper::Infinity;
			for (UINT j = 0; j < (*li)->pGeo->GetMeshData()->Indices.size() / 3; ++j)
			{
				// Indices for this triangle.
				UINT i0 = (*li)->pGeo->GetMeshData()->Indices[j * 3 + 0];
				UINT i1 = (*li)->pGeo->GetMeshData()->Indices[j * 3 + 1];
				UINT i2 = (*li)->pGeo->GetMeshData()->Indices[j * 3 + 2];

				// Vertices for this triangle.
				XMVECTOR v0 = XMLoadFloat3(&(*li)->pGeo->GetMeshData()->Vertices[i0].Position);
				XMVECTOR v1 = XMLoadFloat3(&(*li)->pGeo->GetMeshData()->Vertices[i1].Position);
				XMVECTOR v2 = XMLoadFloat3(&(*li)->pGeo->GetMeshData()->Vertices[i2].Position);

				// We have to iterate over all the triangles in order to find the nearest intersection.
				float t = 0.0f;
				if (MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
				{
					if (t < mindist)
					{
						// This is the new nearest picked triangle.
						mindist = t;
						mPickedMesh = c;
					}
				}
			}
		}		
	}	

	if (mPickedMesh != -1)
	{
		std::list<Object*>::iterator li2 = mObjectManager.mObj_List.begin();
		for (UINT i = 0; i < mPickedMesh; i++, li2++)
		{

		}
		(*li2)->bSelected = true;
		bSelectedObjExist = true;
		mGizmo.SetSelectedObj((*li2)->GetThisObject());
		
		li2 = mObjectManager.mObj_List.begin();
		for (UINT i = 0; i < mObjectManager.mObj_List.size(); i++, li2++)
		{
			if (i != mPickedMesh)
				(*li2)->bSelected = false;
		}
		
		

	}		
	else if (mPickedMesh == -1)
	{
		std::list<Object*>::iterator li2 = mObjectManager.mObj_List.begin();
		for (UINT i = 0; i < mObjectManager.mObj_List.size(); i++, li2++)
		{
			(*li2)->bSelected = false;
		}

		bSelectedObjExist = false;
		mGizmo.SetSelectedObj(NULL);
	}

	return bSelectedObjExist;

}

BOOL Renderer::PickUISprite(int sx, int sy)
{
	XMMATRIX P = XMLoadFloat4x4(&mProj);

	// Compute picking ray in view space.
	float vx = (+2.0f*sx / mClientWidth - 1.0f) / mProj._11;// P(0, 0);
	float vy = (-2.0f*sy / mClientHeight + 1.0f) / mProj._22;//P(1, 1);



	 // Tranform ray to local space of Mesh.
	XMMATRIX View = XMLoadFloat4x4(&mView);

	mPickedMesh = -1;
	float mindist = MathHelper::Infinity;
	//for (UINT i = 0; i < mFbxManager.mFbx_List.size(); i++)
	std::list<DirectionalLightActor*>::iterator li; UINT c = 0;
	for (li = mDirectionalLightActorManager.mDLA_List.begin(); li != mDirectionalLightActorManager.mDLA_List.end(); li++, c++)
	{
		// Ray definition in view space.
		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX W = XMLoadFloat4x4(&(*li)->GetWorldMatrix());
		XMMATRIX toWorld = MathHelper::Inverse(View);
		rayOrigin = XMVector3TransformCoord(rayOrigin, toWorld);
		rayDir = XMVector3TransformNormal(rayDir, toWorld);

		// Make the ray direction unit length for the intersection tests.
		rayDir = XMVector3Normalize(rayDir);

		// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		// so do the ray/triangle tests.
		//
		// If we did not hit the bounding box, then it is impossible that we hit 
		// the Mesh, so do not waste effort doing ray/triangle tests.

		// Assume we have not picked anything yet, so init to -1.
		//mPickedTriangle = -1;

		float tmin = 0.0f;

		//XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &(*li)->GetAABB(), &tmin)
		if ((*li)->GetAABB().Intersects(rayOrigin, rayDir, tmin))
		{
			// Find the nearest ray/triangle intersection.
			//tmin = MathHelper::Infinity;
			for (UINT j = 0; j < (*li)->GetIndexCount() / 3; ++j)
			{
				// Indices for this triangle.
				UINT i0 = (*li)->GetIndex()[j * 3 + 0];
				UINT i1 = (*li)->GetIndex()[j * 3 + 1];
				UINT i2 = (*li)->GetIndex()[j * 3 + 2];

				// Vertices for this triangle.
				XMVECTOR v0 = XMLoadFloat3(&(*li)->GetVertex()[i0].Position);
				XMVECTOR v1 = XMLoadFloat3(&(*li)->GetVertex()[i1].Position);
				XMVECTOR v2 = XMLoadFloat3(&(*li)->GetVertex()[i2].Position);

				// We have to iterate over all the triangles in order to find the nearest intersection.
				float t = 0.0f;
				if (MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
				{
					if (t < mindist)
					{
						// This is the new nearest picked triangle.
						mindist = t;
						mPickedMesh = c;
					}
				}
			}
		}
	}

	if (mPickedMesh != -1)
	{
		std::list<DirectionalLightActor*>::iterator li2 = mDirectionalLightActorManager.mDLA_List.begin();
		for (UINT i = 0; i < mPickedMesh; i++, li2++)
		{

		}
		(*li2)->bSelected = true;
		bSelectedActorExist = true;
		mGizmo.SetSelectedActor((*li2)->GetThisActor());

		li2 = mDirectionalLightActorManager.mDLA_List.begin();
		for (UINT i = 0; i < mDirectionalLightActorManager.mDLA_List.size(); i++, li2++)
		{
			if (i != mPickedMesh)
				(*li2)->bSelected = false;
		}


		std::list<ReflectionActor*>::iterator RAi2 = m_ReflectionActorManager.m_RA_List.begin();
		for (UINT i = 0; i < m_ReflectionActorManager.m_RA_List.size(); i++, RAi2++)
		{
			(*RAi2)->bSelected = false;
		}


		return bSelectedActorExist;
	}


	std::list<ReflectionActor*>::iterator RAi; c = 0;
	for (RAi = m_ReflectionActorManager.m_RA_List.begin(); RAi != m_ReflectionActorManager.m_RA_List.end(); RAi++, c++)
	{
		// Ray definition in view space.
		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX W = XMLoadFloat4x4(&(*RAi)->GetWorldMatrix());
		XMMATRIX toWorld = MathHelper::Inverse(View);
		rayOrigin = XMVector3TransformCoord(rayOrigin, toWorld);
		rayDir = XMVector3TransformNormal(rayDir, toWorld);

		// Make the ray direction unit length for the intersection tests.
		rayDir = XMVector3Normalize(rayDir);

		// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
		// so do the ray/triangle tests.
		//
		// If we did not hit the bounding box, then it is impossible that we hit 
		// the Mesh, so do not waste effort doing ray/triangle tests.

		// Assume we have not picked anything yet, so init to -1.
		//mPickedTriangle = -1;

		float tmin = 0.0f;

		//XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &(*li)->GetAABB(), &tmin)
		if ((*RAi)->GetAABB().Intersects(rayOrigin, rayDir, tmin))
		{
			// Find the nearest ray/triangle intersection.
			//tmin = MathHelper::Infinity;
			for (UINT j = 0; j < (*RAi)->GetIndexCount() / 3; ++j)
			{
				// Indices for this triangle.
				UINT i0 = (*RAi)->GetIndex()[j * 3 + 0];
				UINT i1 = (*RAi)->GetIndex()[j * 3 + 1];
				UINT i2 = (*RAi)->GetIndex()[j * 3 + 2];

				// Vertices for this triangle.
				XMVECTOR v0 = XMLoadFloat3(&(*RAi)->GetVertex()[i0].Position);
				XMVECTOR v1 = XMLoadFloat3(&(*RAi)->GetVertex()[i1].Position);
				XMVECTOR v2 = XMLoadFloat3(&(*RAi)->GetVertex()[i2].Position);

				// We have to iterate over all the triangles in order to find the nearest intersection.
				float t = 0.0f;
				if (MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
				{
					if (t < mindist)
					{
						// This is the new nearest picked triangle.
						mindist = t;
						mPickedMesh = c;
					}
				}
			}
		}
	}


	if (mPickedMesh != -1)
	{
		std::list<ReflectionActor*>::iterator RAi2 = m_ReflectionActorManager.m_RA_List.begin();
		for (UINT i = 0; i < mPickedMesh; i++, RAi2++)
		{

		}
		(*RAi2)->bSelected = true;
		bSelectedActorExist = true;
		mGizmo.SetSelectedActor((*RAi2)->GetThisActor());

		RAi2 = m_ReflectionActorManager.m_RA_List.begin();
		for (UINT i = 0; i < m_ReflectionActorManager.m_RA_List.size(); i++, RAi2++)
		{
			if (i != mPickedMesh)
				(*RAi2)->bSelected = false;
		}

		std::list<DirectionalLightActor*>::iterator li2 = mDirectionalLightActorManager.mDLA_List.begin();
		for (UINT i = 0; i < mDirectionalLightActorManager.mDLA_List.size(); i++, li2++)
		{
			(*li2)->bSelected = false;
		}

		return bSelectedActorExist;
	}
	
	
	if (mPickedMesh == -1) //Last
	{
		std::list<DirectionalLightActor*>::iterator li2 = mDirectionalLightActorManager.mDLA_List.begin();
		for (UINT i = 0; i < mDirectionalLightActorManager.mDLA_List.size(); i++, li2++)
		{
			(*li2)->bSelected = false;
		}

		std::list<ReflectionActor*>::iterator RAi2 = m_ReflectionActorManager.m_RA_List.begin();
		for (UINT i = 0; i < m_ReflectionActorManager.m_RA_List.size(); i++, RAi2++)
		{
			(*RAi2)->bSelected = false;
		}

		bSelectedActorExist = false;
		mGizmo.SetSelectedActor(NULL);
	}

	return bSelectedActorExist;
}

void Renderer::BuildGeometryBuffers()
{
	
	MeshData box;
	MeshData grid;
	MeshData sphere;
	MeshData cylinder;
	MeshData arrow;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 60, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	//geoGen.CreateGeosphere(0.5f, 2, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);
	geoGen.CreateArrow(0.5f, 0.3f, 3.0f, 20, 20, arrow);
	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();
	mArrowVertexOffset = mCylinderVertexOffset + cylinder.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();
	mArrowIndexCount = arrow.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;		
	mArrowIndexOffset = mCylinderIndexOffset + mCylinderIndexCount;

	UINT totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size() +
		arrow.Vertices.size();

	UINT totalIndexCount =
		mBoxIndexCount +
		mGridIndexCount +
		mSphereIndexCount +
		mCylinderIndexCount +
		mArrowIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex> vertices(totalVertexCount);

	XMFLOAT4 black(0.0f, 1.0f, 0.0f, 1.0f);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
		vertices[k].Color = black;
	}
	
	for (size_t i = 0; i < arrow.Vertices.size(); ++i, ++k)
	{
		vertices[k].Position = arrow.Vertices[i].Position;
		vertices[k].Normal = arrow.Vertices[i].Normal;
		vertices[k].Color = black;
	}
	
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex)* totalVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());
	indices.insert(indices.end(), arrow.Indices.begin(), arrow.Indices.end());
	
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));

	
	
	
}

