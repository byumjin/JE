////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowRender.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ShadowRender.h"

ShadowRender::ShadowRender()
{
	
	
}

ShadowRender::ShadowRender(const ShadowRender& other)
{

}

ShadowRender::~ShadowRender()
{
	Shutdown();
}

bool ShadowRender::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
	//BuildObjectRender(device);
	
	return true;
}

void ShadowRender::Shutdown()
{
	return;
}
void ShadowRender::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth / m_TextureDivisonX;
	m_bitmapHeight = MapHeight / m_TextureDivisonY;

	//UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
}


bool ShadowRender::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{
	D3DX11_TECHNIQUE_DESC techDesc;

	deviceContext->RSSetState(RenderStates::ShadowRS);
	deviceContext->IASetInputLayout(InputLayouts::BRDFL_VID);

	if (pDirectionalLightActorManager->mDLA_List.size() > 0)
	{
		for (std::list<DirectionalLightActor*>::iterator DLi = pDirectionalLightActorManager->mDLA_List.begin(); DLi != pDirectionalLightActorManager->mDLA_List.end(); DLi++)
		{
			DirectionalLightActor* pDL = (*DLi);

			SetBackBufferRenderTarget(pDL->m_Shadow_Texture->GetRenderTargetView(), NULL, NULL, NULL, TRUE, TRUE, TRUE, TRUE);

			for (std::list<Object*>::iterator li = pObjectManager->mObj_List.begin(); li != pObjectManager->mObj_List.end(); li++)
			{
				// Incomplete
				for (UINT j = 0; j < (*li)->GetLayerCount(); j++)
				{
					UpdateBasicMatrixforDL((*li)->GetWorldMatrix(), pDL);

					pEffects->ShadowMapFx->mTech->GetDesc(&techDesc);
					pEffects->ShadowMapFx->SetAllVariables(XMLoadFloat4x4(&mWVP), pDL->SunViewProj(), pDL->m_Shadow_Texture->GetShaderResourceView());

					(*li)->Draw(deviceContext, pEffects->ShadowMapFx->mTech, techDesc);

					////////// NULL Shader Texture
					for (UINT p = 0; p < techDesc.Passes; ++p)
					{
						pEffects->ShadowMapFx->SetNulltoTextures();
						pEffects->ShadowMapFx->mTech->GetPassByIndex(p)->Apply(0, deviceContext);
					}

				}
			}
		}
	}

	SetBackBufferRenderTarget(FALSE, FALSE, TRUE, TRUE);

	return true;
}

