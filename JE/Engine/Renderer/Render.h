#ifndef RENDER_H
#define RENDER_H


#include "../Object/CFbx.h"

#include "../Common/GeometryGenerator.h"
#include "../Common/Effect/Effects.h"
#include "../d3dApp.h"
#include "../Common/Effect/d3dx11Effect.h"
#include "../Common/MathHelper.h"

#include "../Textures/RenderTexture.h"
#include "../Actor/Camera.h"
#include "../Actor/DirectionalLightActor.h"
#include "../Actor/ReflectionActor.h"

#include "RenderStates.h"

#include "Scene.h"
#include "SkyBox.h"
#include "Bloom.h"
#include "ObjectsRender.h"
//#include "DebugWindow.h"
#include "DebugWindows.h"
#include "Gizmo.h"
#include "CubeMapRender.h"
#include "ShadowRender.h"
#include "LightingRender.h"

using namespace DirectX;

class Renderer : public CD3DApp
{
public:
	Renderer(HINSTANCE hInstance);
	~Renderer();
	
	bool Init();

	void Destroy();
	void Create();
	void Shutdown();

	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
	
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);
	void OnMouseHover(WPARAM btnState, int x, int y);
	
	void OnMouseWheel(WPARAM btnState, int x, int y);
	void OnKeyDown(WPARAM btnState);

	void OnSystemKeyDown(WPARAM btnState);

	void OnWMSize(WPARAM wParam);
	
	void OnSaveMap(HWND hwnd);
	void OnLoadMap(HWND hwnd);

	void OnImportModel(WCHAR* pModelPath, UINT MaterialNum, WCHAR* Material);
	void OnImportGeometry(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH]);
	void OnImportTexture(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH]);
	void OnCreateMaterial(WCHAR* pModelPath, UINT MaterialNum, WCHAR(*pTexturePathList)[MAX_PATH]);

	void OnAddReflectionActor();

	void UpdateBasicMatrix(XMFLOAT4X4 ObjWorldMat, Camera* pCamera);
	void UpdateBasicMatrixforDL(XMFLOAT4X4 ObjWorldMat, DirectionalLightActor* pDL);


	int Check_IntersectAxisAlignedBoxFrustum(Object* pObject);
	BOOL Pick(int sx, int sy);
	BOOL PickUISprite(int sx, int sy);

	void SetBlendState(bool bAlphaToCoverageEnable, bool bIndependentBlendEnable, bool bBlendEnable,
		D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask);
	void ReleaseBlendState();

	//void BuildSkyBox();
	void BuildBRDFLUT();
	void CreateUISprite();

	void ResortRa();

	void ClearResources();

	void InsertRenderer(RenderCapsule* pRC);

	void SaveMap(){}

	void LoadMap(){}

	void DrawRender();
	void DrawUI();
	void DrawDebug();

	void SetIASetInputLayout(ID3D11InputLayout *pInputLayout)
	{
		if (InputLayouts::VertexInputDesc == pInputLayout)
		{
			m_uIASetInputLayout = 0;
		}
		else if (InputLayouts::BRDFL_VID == pInputLayout)
		{
			m_uIASetInputLayout = 1;
		}
		else if (InputLayouts::Line_VID == pInputLayout)
		{
			m_uIASetInputLayout = 2;
		}
		else if (InputLayouts::GIZMO_VID == pInputLayout)
		{
			m_uIASetInputLayout = 3;
		}		
		md3dImmediateContext->IASetInputLayout(pInputLayout);
	}

	ID3D11InputLayout* GetIASetInputLayout()
	{
		if (m_uIASetInputLayout == 0)
		{
			return InputLayouts::VertexInputDesc;
		}
		else if (m_uIASetInputLayout == 1)
		{
			return InputLayouts::BRDFL_VID;
		}
		else if (m_uIASetInputLayout == 2)
		{
			return InputLayouts::Line_VID;
		}
		else if (m_uIASetInputLayout == 3)
		{
			return InputLayouts::GIZMO_VID;
		}

		return NULL;
	}

private:
	void BuildGeometryBuffers();

private:
	ID3D11Buffer* mVB; //Vertex buffer
	ID3D11Buffer* mIB; //Index buffer	

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;
	ID3DX11EffectScalarVariable* mTime;

	ID3DX11EffectMatrixVariable* mfxWorld;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;

	ID3D11BlendState* mBlendState;

	Material* mMaterial;

	ID3D11InputLayout* mInputLayout;
	ID3D11InputLayout* mGeoLayout;

	ID3D11ShaderResourceView* m_LUT_SRV;
	ID3D11Texture2D* m_LUT_Texture;

	XMFLOAT3 mEyePosW;

	bool bShowWireframe;

	ObjectManager mObjectManager;
	UIObjectManager mUIObjectManager;

	DirectionalLightActorManager mDirectionalLightActorManager;
	ReflectionActorManager m_ReflectionActorManager;

	Texture m_UITextureList[12];
	RenderTextureManger mRenderTextureManger;

	Scene m_Scene;	
	Bloom m_Bloom;
	SkyBox m_SkyBox;
	Gizmo mGizmo;
	ShadowRender m_ShadowRender;
	ObjectsRender m_ObjectsRender;	
	MultiRenderTexture mMRTforDeferredR;
	CubeMapRender m_CubeMapRender;
	DebugWindows mDebugWindows;
	LightingRender m_LightingRender;

	// Define transformations from local spaces to world space.
	
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mCenterSphere;
	
	//ObjectFbx mFbxObjectManager;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	XMFLOAT4X4 mViewProj;

	XMFLOAT4X4 mWVP;
	XMFLOAT4X4 mInvWorld;
	XMFLOAT4X4 mInvWorldTranspo;
	XMFLOAT4X4 mInvView;
	XMFLOAT4X4 mInvProj;
	XMFLOAT4X4 mInvViewProj;
	XMFLOAT4X4 mInvWorldViewProj;

	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;
	int mArrowVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;
	UINT mArrowIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;
	UINT mArrowIndexCount;

	int mFbxObjVertexOffset;
	UINT mFbxObjIndexOffset;
	UINT mFbxObjIndexCount;

	float mTheta;
	float mPhi;
	float mRadius;

	Camera mCamera;
	Camera mCameraForRenderWindow;
	Camera mGizmoCamera;
	
	//UINT mPickedTriangle;
	UINT mPickedMesh;

	Material mMat;	
	Effects mEffects;

	POINT mLastMousePos;

	bool bshowBoundingBox;
	bool bshowDebugWindows;
	bool bshowUISprite;
	bool bSelectedObjExist;
	bool bSelectedActorExist;

	UINT m_uIASetInputLayout;
	
	std::vector<RenderCapsule *> m_RenderCapsules;

};



#endif