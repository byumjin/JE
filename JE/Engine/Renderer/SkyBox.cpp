////////////////////////////////////////////////////////////////////////////////
// Filename: SkyBox.cpp
////////////////////////////////////////////////////////////////////////////////
#include "SkyBox.h"

SkyBox::SkyBox()
{
	
	ReleaseCOM(m_VBforSkyBox);
	ReleaseCOM(m_IBforSkyBox);

	ReleaseCOM(m_SkyCubeMap_Texture);
	ReleaseCOM(m_SkyCubeMap_SRV);

}

SkyBox::SkyBox(const SkyBox& other)
{

}

SkyBox::~SkyBox()
{
	Shutdown();
}

void SkyBox::BuildSkyBox(ID3D11Device* md3dDevice)
{
	DWORD Len = MAX_PATH;
	WCHAR DirPath[MAX_PATH];

	GetCurrentDirectory(Len, DirPath);
	wcscat_s(DirPath, L"\\Assets\\Textures\\skybox.dds");
	HR(d3dHelper::CreateShaderResourceViewFromFile(md3dDevice, DirPath, &m_SkyCubeMap_SRV));



	Vertex VertexSykBox[24];

	VertexSykBox[0].Position = XMFLOAT3(0.5f, 0.5f, -0.5f);
	VertexSykBox[1].Position = XMFLOAT3(0.5f, -0.5f, -0.5f);
	VertexSykBox[2].Position = XMFLOAT3(0.5f, -0.5f, 0.5f);
	VertexSykBox[3].Position = XMFLOAT3(0.5f, 0.5f, 0.5f);

	VertexSykBox[4].Position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	VertexSykBox[5].Position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	VertexSykBox[6].Position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	VertexSykBox[7].Position = XMFLOAT3(-0.5f, 0.5f, 0.5f);

	VertexSykBox[8].Position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	VertexSykBox[9].Position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	VertexSykBox[10].Position = XMFLOAT3(0.5f, -0.5f, -0.5f);
	VertexSykBox[11].Position = XMFLOAT3(0.5f, 0.5f, -0.5f);

	VertexSykBox[12].Position = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	VertexSykBox[13].Position = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	VertexSykBox[14].Position = XMFLOAT3(0.5f, -0.5f, 0.5f);
	VertexSykBox[15].Position = XMFLOAT3(0.5f, 0.5f, 0.5f);

	VertexSykBox[16].Position = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	VertexSykBox[17].Position = XMFLOAT3(0.5f, 0.5f, -0.5f);
	VertexSykBox[18].Position = XMFLOAT3(0.5f, 0.5f, 0.5f);
	VertexSykBox[19].Position = XMFLOAT3(-0.5f, 0.5f, 0.5f);

	VertexSykBox[20].Position = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	VertexSykBox[21].Position = XMFLOAT3(0.5f, -0.5f, -0.5f);
	VertexSykBox[22].Position = XMFLOAT3(0.5f, -0.5f, 0.5f);
	VertexSykBox[23].Position = XMFLOAT3(-0.5f, -0.5f, 0.5f);

	VertexSykBox[0].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	VertexSykBox[1].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	VertexSykBox[2].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	VertexSykBox[3].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	VertexSykBox[4].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	VertexSykBox[5].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	VertexSykBox[6].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	VertexSykBox[7].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	VertexSykBox[8].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	VertexSykBox[9].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	VertexSykBox[10].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	VertexSykBox[11].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	VertexSykBox[12].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	VertexSykBox[13].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	VertexSykBox[14].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);;
	VertexSykBox[15].Color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);

	VertexSykBox[16].Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	VertexSykBox[17].Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	VertexSykBox[18].Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	VertexSykBox[19].Color = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);

	VertexSykBox[20].Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	VertexSykBox[21].Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	VertexSykBox[22].Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	VertexSykBox[23].Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);


	VertexSykBox[0].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	VertexSykBox[1].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	VertexSykBox[2].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	VertexSykBox[3].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	VertexSykBox[4].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	VertexSykBox[5].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	VertexSykBox[6].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
	VertexSykBox[7].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

	VertexSykBox[8].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	VertexSykBox[9].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	VertexSykBox[10].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	VertexSykBox[11].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	VertexSykBox[12].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	VertexSykBox[13].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	VertexSykBox[14].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
	VertexSykBox[15].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	VertexSykBox[16].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	VertexSykBox[17].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	VertexSykBox[18].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
	VertexSykBox[19].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	VertexSykBox[20].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	VertexSykBox[21].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	VertexSykBox[22].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	VertexSykBox[23].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);




	UINT IndiceSykBox[36];

	IndiceSykBox[0] = 0; IndiceSykBox[1] = 1; IndiceSykBox[2] = 2;
	IndiceSykBox[3] = 0; IndiceSykBox[4] = 2; IndiceSykBox[5] = 3;

	IndiceSykBox[6] = 4; IndiceSykBox[7] = 6; IndiceSykBox[8] = 5;
	IndiceSykBox[9] = 4; IndiceSykBox[10] = 7; IndiceSykBox[11] = 6;

	IndiceSykBox[12] = 8; IndiceSykBox[13] = 9; IndiceSykBox[14] = 10;
	IndiceSykBox[15] = 8; IndiceSykBox[16] = 10; IndiceSykBox[17] = 11;

	IndiceSykBox[18] = 12; IndiceSykBox[19] = 14; IndiceSykBox[20] = 13;
	IndiceSykBox[21] = 12; IndiceSykBox[22] = 15; IndiceSykBox[23] = 14;

	IndiceSykBox[24] = 16; IndiceSykBox[25] = 17; IndiceSykBox[26] = 18;
	IndiceSykBox[27] = 16; IndiceSykBox[28] = 18; IndiceSykBox[29] = 19;

	IndiceSykBox[30] = 20; IndiceSykBox[31] = 22; IndiceSykBox[32] = 21;
	IndiceSykBox[33] = 20; IndiceSykBox[34] = 23; IndiceSykBox[35] = 22;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * 24;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &VertexSykBox[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &m_VBforSkyBox));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * 36;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &IndiceSykBox[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &m_IBforSkyBox));


}

bool SkyBox::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
	BuildSkyBox(device);

	
	
	return true;
}

void SkyBox::Shutdown()
{
	// Shutdown the vertex and index buffers.
	//ShutdownBuffers();

	ReleaseCOM(m_VBforSkyBox);
	ReleaseCOM(m_IBforSkyBox);

	ReleaseCOM(m_SkyCubeMap_Texture);
	ReleaseCOM(m_SkyCubeMap_SRV);

	return;
}
void SkyBox::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth / m_TextureDivisonX;
	m_bitmapHeight = MapHeight / m_TextureDivisonY;

	//UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
}


bool SkyBox::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{
	pMRTforDeferredR->ReleaseRenderTarget(deviceContext);

	deviceContext->IASetInputLayout(InputLayouts::VertexInputDesc);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	UpdateBasicMatrix(*mWorld, pCamera);
	
	pEffects->BasicFX->SetSkyBoxVariables(XMLoadFloat4x4(mWorld), XMLoadFloat4x4(&mViewProj), XMLoadFloat4x4(&mWVP),
		XMLoadFloat4x4(&mInvWorldTranspo), XMLoadFloat4x4(&mInvViewProj), XMLoadFloat4x4(&mInvProj),
		*pEyePosW, pTimer->TotalTime(), m_SkyCubeMap_SRV);

	UINT iBoxstride = sizeof(Vertex);
	UINT iBoxoffset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_VBforSkyBox, &iBoxstride, &iBoxoffset);
	deviceContext->IASetIndexBuffer(m_IBforSkyBox, DXGI_FORMAT_R32_UINT, 0);

	pEffects->BasicFX->SkyBoxTech->GetDesc(&techDesc);

	pMRTforDeferredR->RenderToTexture(deviceContext, *ppDepthStencilView);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pEffects->BasicFX->SkyBoxTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(36, 0, 0);
	}

	////////// NULL Shader Texture
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pEffects->BasicFX->SetNulltoTextures();
		pEffects->BasicFX->SkyBoxTech->GetPassByIndex(p)->Apply(0, deviceContext);
	}

	//pMRTforDeferredR->ReleaseRenderTarget(deviceContext);

	return true;
}