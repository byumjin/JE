////////////////////////////////////////////////////////////////////////////////
// Filename: LightingRender.cpp
////////////////////////////////////////////////////////////////////////////////
#include "LightingRender.h"

LightingRender::LightingRender()
{
	
	
}

LightingRender::LightingRender(const LightingRender& other)
{

}

LightingRender::~LightingRender()
{
	Shutdown();
}

bool LightingRender::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
		
	return true;
}

void LightingRender::Shutdown()
{
	return;
}
void LightingRender::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth / m_TextureDivisonX;
	m_bitmapHeight = MapHeight / m_TextureDivisonY;

	//UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
}



bool LightingRender::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{
	deviceContext->RSSetState(RenderStates::NormalRS);

	deviceContext->IASetInputLayout(InputLayouts::VertexInputDesc);
	//ID3D11RenderTargetView* pSRTV = m_SceneTexture->GetRenderTargetView();
	ID3D11RenderTargetView* pSRTV = pRenderTextureManger->mRT_List.at(SCENETEXTURE)->GetRenderTargetView();

	deviceContext->OMSetRenderTargets(1, &pSRTV, NULL);
	deviceContext->ClearRenderTargetView(pSRTV, Colors::Black);

	// Set per frame constants.	
	//UpdateBasicMatrix(mWorld, &mCamera);
	UpdateBasicMatrix(*mWorld, pCameraForRenderWindow);

	DirectionalLightActor* pDL = NULL;

	if (pDirectionalLightActorManager->mDLA_List.size() > 0)
	{
		for (std::list<DirectionalLightActor*>::iterator li = pDirectionalLightActorManager->mDLA_List.begin(); li != pDirectionalLightActorManager->mDLA_List.end(); li++)
		{
			pDL = (*li);
			pEffects->BRDFLightingFX->SetDirLights((*li)->GetDirLightData());
		}
	}


	pEffects->BRDFLightingFX->SetAllVariables(XMLoadFloat4x4(mWorld), XMLoadFloat4x4(&mViewProj), XMLoadFloat4x4(&mWVP), XMLoadFloat4x4(&mInvWorldTranspo),
		XMLoadFloat4x4(&mInvViewProj), XMLoadFloat4x4(&mInvProj),
		*pEyePosW, pTimer->TotalTime(), pDL->SunViewProj(), pCamera->InvViewProj(), pMRTforDeferredR->GetShaderResourceView(0),
		pMRTforDeferredR->GetShaderResourceView(1), pMRTforDeferredR->GetShaderResourceView(2), pMRTforDeferredR->GetShaderResourceView(3),
		pRenderTextureManger->mRT_List.at(REFLECTION)->GetShaderResourceView(), pLUT_SRV, pDL->m_Shadow_Texture->GetShaderResourceView());

	// Figure out which technique to use.
	
	deviceContext->IASetInputLayout(InputLayouts::BRDFL_VID);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pScene->Render(deviceContext, 0, 0, mInvViewProj, pEffects->BRDFLightingFX->mTech, pEffects);

	return true;
}
