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
	pDevice = device;
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

	//SetBlendState(FALSE, FALSE, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_ADD,
	//	          D3D11_BLEND_DEST_ALPHA, D3D11_BLEND_INV_DEST_ALPHA, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);

	pScene->Render(deviceContext, 0, 0, mInvViewProj, pEffects->BRDFLightingFX->mTech, pEffects);

	//SetBlendState(FALSE, FALSE, TRUE, D3D11_BLEND_SRC_COLOR, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_OP_ADD,
	//	D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);

	ReleaseCOM(pBlendState);

	return true;
}

void LightingRender::SetBlendState(bool bAlphaToCoverageEnable, bool bIndependentBlendEnable, bool bBlendEnable,
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

	HR(pDevice->CreateBlendState(&BD, &pBlendState));
	pdeviceContext->OMSetBlendState(pBlendState, NULL, 0xffffffff);
}
