#include "RenderCapsule.h"

void RenderCapsule::UpdateBasicMatrix(XMFLOAT4X4 ObjWorldMat, Camera* pCamera)
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

	if(mWorld != NULL)
		XMStoreFloat4x4(mWorld, world);

	XMStoreFloat4x4(&mWorldView, world*view);
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

void RenderCapsule::UpdateBasicMatrixforDL(XMFLOAT4X4 ObjWorldMat, DirectionalLightActor* pDL)
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

void RenderCapsule::SetBackBufferRenderTarget(ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, ID3D11DepthStencilState* pDSS, UINT StencilRef,
	bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil)
{
	ID3D11RenderTargetView* pNullRTV[] = { NULL };
	pdeviceContext->OMSetRenderTargets(1, pNullRTV, NULL);

	ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	pdeviceContext->PSSetShaderResources(0, 1, pSRV);

	pdeviceContext->OMSetRenderTargets(1, &pRTV, pDSV);

	if (bClearRenderTargetView)
		pdeviceContext->ClearRenderTargetView(pRTV, reinterpret_cast<const float*>(&Colors::Black));

	if (pDSS)
		pdeviceContext->OMSetDepthStencilState(pDSS, StencilRef);

	if (pDSV)
		pdeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

void RenderCapsule::SetBackBufferRenderTarget(bool bDepthStencil, bool bClearRenderTargetView, bool bClearDepth, bool bClearStencil)
{
	ID3D11RenderTargetView* pNullRTV[] = { NULL };
	pdeviceContext->OMSetRenderTargets(1, pNullRTV, NULL);

	ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	pdeviceContext->PSSetShaderResources(0, 1, pSRV);

	if (bDepthStencil)
		pdeviceContext->OMSetRenderTargets(1, ppRenderTargetView, *ppDepthStencilView);
	else
		pdeviceContext->OMSetRenderTargets(1, ppRenderTargetView, NULL);

	if (bClearRenderTargetView)
		pdeviceContext->ClearRenderTargetView(*ppRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	if (bClearDepth && bClearStencil)
		pdeviceContext->ClearDepthStencilView(*ppDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	else if (bClearDepth)
		pdeviceContext->ClearDepthStencilView(*ppDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	else if (bClearStencil)
		pdeviceContext->ClearDepthStencilView(*ppDepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);

	return;
}

void RenderCapsule::InitExternValue(ID3D11DeviceContext* deviceContext, MultiRenderTexture* MRTforDeferredR, Camera* MainCamera, Camera* CameraForRenderWindow,
	XMFLOAT3* EyePosW, CGameTimer* Timer,
	ID3D11RenderTargetView** RenderTargetViewParam, ID3D11DepthStencilView** DepthStencilViewParam,
	ObjectManager *ObjectManagerParam,
	ReflectionActorManager* ReflectionActorManagerParam,
	DirectionalLightActorManager* DirectionalLightActorManagerParam,
	RenderTextureManger* RenderTextureMangerParam,
	XMFLOAT4X4* World,
	RenderCapsule* SceneParam,
	ID3D11ShaderResourceView* LUT_SRV)
{

	pdeviceContext = deviceContext;

	pMRTforDeferredR = MRTforDeferredR;

	pCamera = MainCamera;
	pCameraForRenderWindow = CameraForRenderWindow;

	pEyePosW = EyePosW;
	pTimer = Timer;

	ppRenderTargetView = RenderTargetViewParam;
	ppDepthStencilView = DepthStencilViewParam;

	pObjectManager = ObjectManagerParam;

	pReflectionActorManager = ReflectionActorManagerParam;
	pDirectionalLightActorManager = DirectionalLightActorManagerParam;

	pRenderTextureManger = RenderTextureMangerParam;

	mWorld = World;

	pScene = SceneParam;

	pLUT_SRV = LUT_SRV;
}

int RenderCapsule::Check_IntersectAxisAlignedBoxFrustum(Object* pObject)
{
	BoundingFrustum localspaceFrustum;

	BoundingBox XX;

	pObject->GetAABB().Transform(XX, DirectX::XMMatrixMultiply(XMLoadFloat4x4(&pObject->GetWorldMatrix()), XMLoadFloat4x4(&mView)));

	return pCamera->GetFrustum().Intersects(XX);
}