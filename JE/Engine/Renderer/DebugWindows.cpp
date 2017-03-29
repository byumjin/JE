////////////////////////////////////////////////////////////////////////////////
// Filename: DebugWindows.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DebugWindows.h"


DebugWindows::DebugWindows()
{
	
}

DebugWindows::DebugWindows(const DebugWindows& other)
{

}

DebugWindows::~DebugWindows()
{
	Shutdown();	
}

bool DebugWindows::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
	

	for(UINT i = 0; i < 12; i++)
	{		
			m_DebugWindow[i].SetTextureDivision(4, 4);
			m_DebugWindow[i].Initialize(device, screenWidth, screenHeight, resolutionWidth, resolutionHeight);		
	}

	return true;
}


void DebugWindows::Shutdown()
{
	// Shutdown the vertex and index buffers.
	for (UINT i = 0; i < 12; i++)
	{
		m_DebugWindow[i].Shutdown();
	}	

	return;
}
void DebugWindows::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	for (UINT i = 0; i < 12; i++)
	{
		m_DebugWindow[i].OnResize(ScreenWidth, ScreenHeight, MapWidth, MapHeight);
	}	
}

bool DebugWindows::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{
	
	pdeviceContext = deviceContext;

	UpdateBasicMatrix(*mWorld, pCameraForRenderWindow);

	///////// <--------- Draw DebugWindows
	SetBackBufferRenderTarget(TRUE, FALSE, TRUE, FALSE);

	//if (*pBshowDebugWindows)
	//{
	

		deviceContext->IASetInputLayout(InputLayouts::BRDFL_VID);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pEffects->DebugWindowFx->SetAllVariables(XMLoadFloat4x4(mWorld), XMLoadFloat4x4(&mViewProj), XMLoadFloat4x4(&mWVP), XMLoadFloat4x4(&mInvWorldTranspo),
			XMLoadFloat4x4(&mInvViewProj), XMLoadFloat4x4(&mInvProj), *pEyePosW, pTimer->TotalTime(), pMRTforDeferredR->GetShaderResourceView(0),
			pMRTforDeferredR->GetShaderResourceView(1),
			pMRTforDeferredR->GetShaderResourceView(2), pMRTforDeferredR->GetShaderResourceView(3), pRenderTextureManger->mRT_List.at(SILHOUETTE)->GetShaderResourceView(),
			pMRTforDeferredR->GetShaderResourceView(4), pRenderTextureManger->mRT_List.at(REFLECTION)->GetShaderResourceView(), m_BloomTexture->GetShaderResourceView(),
			pRenderTextureManger->mRT_List.at(SCENETEXTURE)->GetShaderResourceView() /*pShadowTexture->GetShaderResourceView()*/, pCamera->InvViewProj());

		// Figure out which technique to use.
		
		m_DebugWindow[0].Render(pdeviceContext, 0, 0, mInvViewProj, pEffects->DebugWindowFx->BasicTech, NULL);		
		m_DebugWindow[1].Render(pdeviceContext, m_DebugWindow[1].m_bitmapWidth * 1, 0, mInvViewProj, pEffects->DebugWindowFx->SpecTech, NULL);		
		m_DebugWindow[2].Render(pdeviceContext, m_DebugWindow[2].m_bitmapWidth * 2, 0, mInvViewProj, pEffects->DebugWindowFx->RoughnessTech, NULL);		
		m_DebugWindow[3].Render(pdeviceContext, m_DebugWindow[3].m_bitmapWidth * 3, 0, mInvViewProj, pEffects->DebugWindowFx->NormalTech, NULL);		
		m_DebugWindow[4].Render(pdeviceContext, 0, m_DebugWindow[4].m_bitmapHeight, mInvViewProj, pEffects->DebugWindowFx->MetallicTech, NULL);		
		m_DebugWindow[5].Render(pdeviceContext, 0, m_DebugWindow[5].m_bitmapHeight * 2, mInvViewProj, pEffects->DebugWindowFx->WorldPositionTech, NULL);		
		m_DebugWindow[6].Render(pdeviceContext, 0, m_DebugWindow[6].m_bitmapHeight * 3, mInvViewProj, pEffects->DebugWindowFx->DepthTech, NULL);		
		m_DebugWindow[7].Render(pdeviceContext, m_DebugWindow[7].m_bitmapWidth * 3, m_DebugWindow[7].m_bitmapHeight, mInvViewProj, pEffects->DebugWindowFx->SelectedTech, NULL);		
		m_DebugWindow[8].Render(pdeviceContext, m_DebugWindow[8].m_bitmapWidth * 3, m_DebugWindow[8].m_bitmapHeight * 2, mInvViewProj, pEffects->DebugWindowFx->OpacityTech, NULL);		
		m_DebugWindow[9].Render(pdeviceContext, m_DebugWindow[9].m_bitmapWidth * 3, m_DebugWindow[9].m_bitmapHeight * 3, mInvViewProj, pEffects->DebugWindowFx->AOTech, NULL);		
		m_DebugWindow[10].Render(pdeviceContext, m_DebugWindow[10].m_bitmapWidth * 1, m_DebugWindow[10].m_bitmapHeight * 3, mInvViewProj, pEffects->DebugWindowFx->EnvTech, NULL);
		
		
		m_DebugWindow[11].Render(pdeviceContext, m_DebugWindow[11].m_bitmapWidth * 2, m_DebugWindow[11].m_bitmapHeight * 3, mInvViewProj, pEffects->DebugWindowFx->BloomTech, pEffects/*mEffects.DebugWindowFx*/);
		//m_DebugWindow[11].Render(pdeviceContext, 0, 0, mInvViewProj, pEffects->DebugWindowFx->BloomTech, pEffects/*mEffects.DebugWindowFx*/);

	//}
	///////// ---------> Draw DebugWindows
	UpdateBasicMatrix(*mWorld, pCamera);

	
	return true;
}
