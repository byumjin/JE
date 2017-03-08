////////////////////////////////////////////////////////////////////////////////
// Filename: ObjectRender.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ObjectsRender.h"

ObjectsRender::ObjectsRender()
{
	
	
}

ObjectsRender::ObjectsRender(const ObjectsRender& other)
{

}

ObjectsRender::~ObjectsRender()
{
	Shutdown();
}

bool ObjectsRender::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
		
	return true;
}

void ObjectsRender::Shutdown()
{
	return;
}
void ObjectsRender::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth / m_TextureDivisonX;
	m_bitmapHeight = MapHeight / m_TextureDivisonY;

	//UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
}



bool ObjectsRender::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{
	D3DX11_TECHNIQUE_DESC techDesc;

	for (std::list<Object*>::iterator li = pObjectManager->mObj_List.begin(); li != pObjectManager->mObj_List.end(); li++)
	{
		//(*li)->Update(mTimer.DeltaTime()*3.0f);

		// Incomplete
		for (UINT j = 0; j < (*li)->GetLayerCount(); j++)
		{
			UpdateBasicMatrix((*li)->GetWorldMatrix(), pCamera);

			// 0: not intersect 1: intersect 2: include
			if (Check_IntersectAxisAlignedBoxFrustum((*li)) != 0)
			{
				BasicMaterialEffect* pEffect = pEffects->BasicFX;//  (BasicMaterialEffect*)((*li)->GetMaterials(j));

				if (pEffect)
				{
					// Set per frame constants.		
					pEffect->SetAllVariables
					(XMLoadFloat4x4(&(*li)->GetWorldMatrix()), XMLoadFloat4x4(&mViewProj), XMLoadFloat4x4(&mWVP), XMLoadFloat4x4(&mInvWorldTranspo), XMLoadFloat4x4(&mInvViewProj), XMLoadFloat4x4(&mInvProj),
						*pEyePosW, pTimer->TotalTime(), (*li)->bSelected, (*li)->pMI->m_DiffuseMap->GetShaderResourceView(), (*li)->pMI->m_SpecularMap->GetShaderResourceView(),
						(*li)->pMI->m_NormalMap->GetShaderResourceView());


					pEffect->mTech->GetDesc(&techDesc);
				}

				(*li)->Draw(deviceContext, pEffect->mTech, techDesc);

				////////// NULL Shader Texture
				for (UINT p = 0; p < techDesc.Passes; ++p)
				{
					pEffect->SetNulltoTextures();
					pEffect->mTech->GetPassByIndex(p)->Apply(0, deviceContext);
				}
			}
		}
	}

	//Release MRT
	pMRTforDeferredR->ReleaseRenderTarget(deviceContext);

	return true;
}
