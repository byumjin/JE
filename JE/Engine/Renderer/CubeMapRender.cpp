////////////////////////////////////////////////////////////////////////////////
// Filename: CubeMapRender.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CubeMapRender.h"

CubeMapRender::CubeMapRender()
{
	
	
}

CubeMapRender::CubeMapRender(const CubeMapRender& other)
{

}

CubeMapRender::~CubeMapRender()
{
	Shutdown();
}

bool CubeMapRender::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
	//BuildObjectRender(device);

	
	return true;
}

void CubeMapRender::Shutdown()
{
	return;
}
void CubeMapRender::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth / m_TextureDivisonX;
	m_bitmapHeight = MapHeight / m_TextureDivisonY;

	//UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
}

bool CubeMapRender::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{
	D3DX11_TECHNIQUE_DESC techDesc;

	deviceContext->RSSetState(RenderStates::NormalRS);
	deviceContext->IASetInputLayout(InputLayouts::VertexInputDesc);
	
	if (pObjectManager->mObj_List.size() < 1)
	{

	}
	else
	{
		SetBackBufferRenderTarget(pRenderTextureManger->mRT_List.at(REFLECTION)->GetRenderTargetView(), *ppDepthStencilView, NULL, NULL, FALSE, TRUE, TRUE, FALSE);


		for (std::list<Object*>::iterator li = pObjectManager->mObj_List.begin(); li != pObjectManager->mObj_List.end(); li++)
		{
			// Incomplete
			for (UINT j = 0; j < (*li)->GetLayerCount(); j++)
			{
				UpdateBasicMatrix((*li)->GetWorldMatrix(), pCamera);

				// 0: not intersect 1: intersect 2: include
				if (Check_IntersectAxisAlignedBoxFrustum((*li)) != 0)
				{
					ReflectionActorEffect* pEffect = pEffects->ReflectionActorFx;

					if (pEffect)
					{
						ReflectionActor* RAarray[6];

						std::list<ReflectionActor*>::iterator Rai = pReflectionActorManager->m_RA_List.begin();

						UINT EnvCubeCount = 0;

						for (UINT g = 0; g < pReflectionActorManager->m_RA_List.size(); g++, Rai++)
						{
							if (MathHelper::Distance((*Rai)->GetWorldPosition(), (*li)->GetOriginPointPos()) <= (*Rai)->GetRadius())
							{
								RAarray[EnvCubeCount] = *Rai;
								EnvCubeCount++;
							}
						}

						if (EnvCubeCount >= 1)
						{
							for (UINT g = 0; g < EnvCubeCount; g++)
								pEffect->SetAllEnvTextures(RAarray[g]->GetCubeMapSRV(), g);
						}

						// Set per frame constants.		
						pEffect->SetCubeVariables(XMLoadFloat4x4(&(*li)->GetWorldMatrix()), XMLoadFloat4x4(&mWVP), pCamera->InvViewProj(),
							(*li)->pMI->m_NormalMap->GetShaderResourceView(),
							(*li)->pMI->m_SpecularMap->GetShaderResourceView(),
							*pEyePosW,
							(float)EnvCubeCount,
							(*li)->GetOriginPointPos());

						pEffect->CompositionCubeMapTech->GetDesc(&techDesc);
					}

					(*li)->Draw(deviceContext, pEffect->CompositionCubeMapTech, techDesc);

					////////// NULL Shader Texture
					for (UINT p = 0; p < techDesc.Passes; ++p)
					{
						pEffects->ReflectionActorFx->SetNulltoTextures();
						pEffect->CompositionCubeMapTech->GetPassByIndex(p)->Apply(0, deviceContext);
					}
				}
			}
		}

		deviceContext->GenerateMips(pRenderTextureManger->mRT_List.at(REFLECTION)->GetShaderResourceView());
	}


	

	//ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	//deviceContext->PSSetShaderResources(0, 1, pSRV);

	return true;
}
