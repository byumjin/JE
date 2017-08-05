#pragma once

#include "../../stdafx.h"
#include "../Common/D3DUtil.h"
#include "../Object/Object.h"
#include "../Actor/Actor.h"
#include "../Textures/RenderTexture.h"
#include "../Actor/Camera.h"

#include "../Actor/DirectionalLightActor.h"
#include "../Actor/ReflectionActor.h"
#include "../Common/GameTimer.h"

enum RENDERORDER
{
	PRERENDER = 0, BACKGROUND = 1000, RENDERING = 2000, POSTPROCESS = 3000, UI = 4000, DEBUG_RENDER = 5000
};

class RenderCapsule
{
public:
	
	RenderCapsule()
	{
		m_ScreenDivisonX = 1;
		m_ScreenDivisonY = 1;

		m_TextureDivisonX = 1;
		m_TextureDivisonY = 1;
	}

	virtual ~RenderCapsule()
	{
	}

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight) = 0;
	virtual void Shutdown() = 0;
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight) = 0;
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects) = 0;

	void SetScreenDivision(int x, int y)
	{
		m_ScreenDivisonX = x;
		m_ScreenDivisonY = y;
	}

	void SetTextureDivision(int x, int y)
	{
		m_TextureDivisonX = x;
		m_TextureDivisonY = y;
	}

	int m_ScreenDivisonX;
	int m_ScreenDivisonY;

	int m_TextureDivisonX;
	int m_TextureDivisonY;

	UINT RenderPriority;

	void RenderCapsule::UpdateBasicMatrix(XMFLOAT4X4 ObjWorldMat, Camera* pCamera);
	void RenderCapsule::UpdateBasicMatrixforDL(XMFLOAT4X4 ObjWorldMat, DirectionalLightActor* pDL);
	void RenderCapsule::SetBackBufferRenderTarget(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, ID3D11DepthStencilState* pDSS, UINT StencilRef,
		bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil);	
	void RenderCapsule::SetBackBufferRenderTarget(bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil);
	void RenderCapsule::InitExternValue(ID3D11DeviceContext* deviceContext, MultiRenderTexture* MRTforDeferredR, Camera* MainCamera, Camera* CameraForRenderWindow,
		XMFLOAT3* EyePosW, CGameTimer* Timer,
		ID3D11RenderTargetView** RenderTargetViewParam, ID3D11DepthStencilView** DepthStencilViewParam,
		ObjectManager *ObjectManagerParam,
		ReflectionActorManager* ReflectionActorManagerParam,
		DirectionalLightActorManager* DirectionalLightActorManagerParam,
		RenderTextureManger* RenderTextureMangerParam,
		XMFLOAT4X4* mWorld,
		RenderCapsule* SceneParam,
		ID3D11ShaderResourceView* LUT_SRV);

	int RenderCapsule::Check_IntersectAxisAlignedBoxFrustum(Object* pObject);

	ID3D11DeviceContext* pdeviceContext;

	MultiRenderTexture* pMRTforDeferredR;

	Camera* pCamera;
	Camera* pCameraForRenderWindow;

	RenderCapsule* pScene;

	XMFLOAT3* pEyePosW;
	CGameTimer*pTimer;

	ID3D11RenderTargetView** ppRenderTargetView;
	ID3D11DepthStencilView** ppDepthStencilView;

	ID3D11ShaderResourceView* pLUT_SRV;

	ObjectManager *pObjectManager;

	ReflectionActorManager* pReflectionActorManager;
	RenderTextureManger* pRenderTextureManger;
	DirectionalLightActorManager* pDirectionalLightActorManager;

	XMFLOAT4X4* mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	XMFLOAT4X4 mWorldView;
	XMFLOAT4X4 mViewProj;

	XMFLOAT4X4 mWVP;
	XMFLOAT4X4 mInvWorld;
	XMFLOAT4X4 mInvWorldTranspo;
	XMFLOAT4X4 mInvView;
	XMFLOAT4X4 mInvProj;
	XMFLOAT4X4 mInvViewProj;
	XMFLOAT4X4 mInvWorldViewProj;

private:

	
	
};

