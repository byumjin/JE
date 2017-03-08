#include "ReflectionActor.h"

ReflectionActor::ReflectionActor(ID3D11Device* pd3dDevice)
{
	m_radius = 1.0f;

	BuildBuffers(pd3dDevice);
	BuildActorBuffers(pd3dDevice);
	BuildCubeFaceCamera(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 6; ++i)
	{
		m_DynamicCubeMapRTV[i] = NULL;
	}

	BuildDynamicCubeMapViews(pd3dDevice);
}

ReflectionActor::~ReflectionActor()
{
	//ReleaseCOM(m_DynamicCubeMapDSV);
	ReleaseCOM(m_DynamicCubeMapSRV);
	for (int i = 0; i < 6; ++i)
	{
		ReleaseCOM(m_DynamicCubeMapRTV[i]);
		ReleaseCOM(m_DynamicCubeMapDSV[i]);
		//ReleaseCOM(m_DynamicCubeMapSRV[i]);
		
		ReleaseCOM(m_RenderTexture[i]);
	}		

	ReleaseCOM(m_DynamicCubeMapDSRV);
}

void ReflectionActor::Translation(float x, float y, float z)
{
	Actor::Translation(x, y, z);
	XMFLOAT3 pos = GetWorldPosition();

	BuildCubeFaceCamera(pos.x, pos.y, pos.z);
	
}

int ReflectionActor::Check_IntersectAxisAlignedBoxFrustum(Object* pObject, XMFLOAT4X4* WorldMat, XMMATRIX* ViewMat, Camera* pCamera)
{


	BoundingFrustum localspaceFrustum;

	BoundingBox XX;

	pObject->GetAABB().Transform(XX, DirectX::XMMatrixMultiply(XMLoadFloat4x4(&pObject->GetWorldMatrix()), pCamera->View()));

	return pCamera->GetFrustum().Intersects(XX);

}

void ReflectionActor::DrawScene(ID3D11DeviceContext* pd3dImmediateContext, ObjectManager* pObjectManager)
{
	ID3D11RenderTargetView* renderTargets[1];

	// Generate the cube map.
	pd3dImmediateContext->RSSetViewports(1, &m_CubeMapViewport);
	for (int i = 0; i < 6; ++i)
	{
		// Clear cube map face and depth buffer.
		pd3dImmediateContext->ClearRenderTargetView(m_DynamicCubeMapRTV[i], reinterpret_cast<const float*>(&Colors::Silver));
		pd3dImmediateContext->ClearDepthStencilView(m_DynamicCubeMapDSV[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// Bind cube map face as render target.
		renderTargets[0] = m_DynamicCubeMapRTV[i];
		pd3dImmediateContext->OMSetRenderTargets(1, renderTargets, m_DynamicCubeMapDSV[i]);

		// Draw the scene with the exception of the center sphere to this cube map face.
		//DrawScene(m_CubeMapCamera[i], pd3dImmediateContext, pObjectManager);
	}
	
}

bool ReflectionActor::CreateCubeMap(ID3D11Device *pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ObjectManager* pObjectManager,
	Effects* pEffects, ID3D11Buffer* m_VBforSkyBox, ID3D11Buffer* m_IBforSkyBox, ID3D11ShaderResourceView* SkyBoxSRV, XMMATRIX* pworldforSky,
	XMFLOAT3 gEyePos,
	ID3D11ShaderResourceView* WPSRV, ID3D11ShaderResourceView* WNSRV)
{
	ID3D11RenderTargetView* renderTargets[1];

	// Draw the scene with the exception of the center sphere to this cube map face.
	// Generate the cube map.
	pd3dImmediateContext->RSSetViewports(1, &m_CubeMapViewport);
	
	

	for (int i = 0; i < 6; ++i)
	{
		// Clear cube map face and depth buffer.
		pd3dImmediateContext->ClearRenderTargetView(m_DynamicCubeMapRTV[i], reinterpret_cast<const float*>(&Colors::Minus));
		pd3dImmediateContext->ClearDepthStencilView(m_DynamicCubeMapDSV[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		// Bind cube map face as render target.
		renderTargets[0] = m_DynamicCubeMapRTV[i];
		pd3dImmediateContext->OMSetRenderTargets(1, renderTargets, NULL);
		

		ID3DX11EffectTechnique* activeTech = NULL;
		D3DX11_TECHNIQUE_DESC techDesc;
				
		// Draw the scene with the exception of the center sphere to this cube map face.
		for (std::list<Object*>::iterator li = pObjectManager->mObj_List.begin(); li != pObjectManager->mObj_List.end(); li++)
		{
			// Incomplete
			for (UINT j = 0; j < (*li)->GetLayerCount(); j++)
			{

				UINT iBoxstride = sizeof(Vertex);
				UINT iBoxoffset = 0;

				pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_VBforSkyBox, &iBoxstride, &iBoxoffset);
				pd3dImmediateContext->IASetIndexBuffer(m_IBforSkyBox, DXGI_FORMAT_R32_UINT, 0);

				ReflectionActorEffect* pEffect = pEffects->ReflectionActorFx;

				Camera* pCamera = &m_CubeMapCamera[i];

				////////// Set Basic Matrix
				XMMATRIX world = XMLoadFloat4x4(&(*li)->GetWorldMatrix());

				XMMATRIX view = pCamera->View();
				XMMATRIX proj = pCamera->Proj();
				XMMATRIX viewProj = pCamera->ViewProj();

				XMMATRIX worldViewProj = world*viewProj;
							
				XMFLOAT4X4 WorldMat;
				XMStoreFloat4x4(&WorldMat, world);

				XMMATRIX worldViewProjforSkyBox = (*pworldforSky)*viewProj;

				pEffect->SetSkyBoxVariables(worldViewProjforSkyBox, (*pworldforSky), SkyBoxSRV);
				//pEffect->SetSkyBoxVariables(worldViewProjforSkyBox, (*pworldforSky), SkyBoxSRV, (*li)->GetOriginPointPos(), gEyePos, WPSRV, WNSRV, MathHelper::Inverse(viewProj));

				activeTech = pEffect->SkyBoxTech;
				activeTech->GetDesc(&techDesc);

				for (UINT p = 0; p < techDesc.Passes; ++p)
				{
					activeTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
					pd3dImmediateContext->DrawIndexed(36, 0, 0);
				}

				////////// NULL Shader Texture
				for (UINT p = 0; p < techDesc.Passes; ++p)
				{
					pEffects->ReflectionActorFx->SetNulltoTextures();
					activeTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
				}

				//pd3dImmediateContext->ClearDepthStencilView(m_DynamicCubeMapDSV[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
				pd3dImmediateContext->OMSetRenderTargets(1, renderTargets, m_DynamicCubeMapDSV[i]);

				// 0: not intersect 1: intersect 2: include
				if (Check_IntersectAxisAlignedBoxFrustum((*li), &WorldMat, &view, pCamera) != 0)
				{					
					//First : draw with color
					ReflectionActorEffect* pEffect = pEffects->ReflectionActorFx;
					
					
					if (pEffect)
					{
						// Set per frame constants.		
						pEffect->SetColorVariables(worldViewProj, (*li)->pMI->m_DiffuseMap->GetShaderResourceView());
						activeTech = pEffect->ReflectionActorTech;
						activeTech->GetDesc(&techDesc);
					}

					(*li)->Draw(pd3dImmediateContext, activeTech, techDesc);
										
					
					////////// NULL Shader Texture
					for (UINT p = 0; p < techDesc.Passes; ++p)
					{
						pEffects->ReflectionActorFx->SetNulltoTextures();						
						activeTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
						
					}
					
					
					
				}
				
			}


		}	
		

		pd3dImmediateContext->GenerateMips(m_DynamicCubeMapSRV);		
		
	}

	

	return true;
}

ID3D11ShaderResourceView* ReflectionActor::GetCubeMapSRV()
{
	return m_DynamicCubeMapSRV;
}

void ReflectionActor::BuildActorBuffers(ID3D11Device *pd3dDevice)
{
	float SinVal;
	float CosVal;

	m_VertexforActorGizmo = new Vertex[33 * 3];

	for (UINT i = 0; i < 33; i++)
	{
		SinVal = sin(MathHelper::ConvertToRadians(11.25f * (float)i));
		CosVal = cos(MathHelper::ConvertToRadians(11.25f * (float)i));

		m_VertexforActorGizmo[i].Position = XMFLOAT3(CosVal, 0.0f, SinVal);
		m_VertexforActorGizmo[i + 33].Position = XMFLOAT3(0.0f, CosVal, SinVal);
		m_VertexforActorGizmo[i + 33 * 2].Position = XMFLOAT3(CosVal, SinVal, 0.0f);

		m_VertexforActorGizmo[i].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		m_VertexforActorGizmo[i + 33].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		m_VertexforActorGizmo[i + 33 * 2].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	m_IndicesforActorGizmo = new UINT[33 * 6];

	for (UINT i = 0; i < 32; i++)
	{
		m_IndicesforActorGizmo[2 * i] = i;
		m_IndicesforActorGizmo[2 * i + 1] = i + 1;
	}

	for (UINT i = 33; i < 33 + 32; i++)
	{
		m_IndicesforActorGizmo[2 * i - 2] = i;
		m_IndicesforActorGizmo[2 * i - 2 + 1] = i + 1;
	}

	for (UINT i = 33 * 2; i < 33 * 2 + 32; i++)
	{
		m_IndicesforActorGizmo[2 * i - 4] = i;
		m_IndicesforActorGizmo[2 * i - 4 + 1] = i + 1;
	}

	m_VertexCountforActorGizmo = 33 * 3;
	m_IndexCountforActorGizmo = 33 * 6;

	BuildVertexBuffers(pd3dDevice, &m_VertexforActorGizmo[0], D3D11_USAGE_IMMUTABLE, m_VertexCountforActorGizmo, D3D11_BIND_VERTEX_BUFFER, 0, 0, &m_VertexBufferforActorGizmo);
	BuildIndexBuffers(pd3dDevice, &m_IndicesforActorGizmo[0], D3D11_USAGE_IMMUTABLE, m_IndexCountforActorGizmo, D3D11_BIND_INDEX_BUFFER, 0, 0, &m_IndexBufferforActorGizmo);


	delete[] m_VertexforActorGizmo;
	delete[] m_IndicesforActorGizmo;
}

void ReflectionActor::DrawActorGizmo(ID3D11DeviceContext* pd3dImmediateContext, Effects* pEffects, XMMATRIX* paraWorld, XMFLOAT4X4* paraWVP, FLOAT X, FLOAT Y, float paraLength)
{
	XMFLOAT4 originPOSITION;

	originPOSITION = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	originPOSITION = MathHelper::XMFLOAT4_DIV_W(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(originPOSITION, *paraWVP));
	XMFLOAT4X4 mTransform = CalculateTransformMat(X, Y, originPOSITION);

	ID3DX11EffectTechnique* activeTech = NULL;
	D3DX11_TECHNIQUE_DESC techDesc;

	pEffects->GizmoDrawFx->SetAxisTechVariables(*paraWorld, XMLoadFloat4x4(paraWVP), XMLoadFloat4x4(&mTransform), paraLength);

	activeTech = pEffects->GizmoDrawFx->mTech;

	pd3dImmediateContext->IASetInputLayout(InputLayouts::BRDFL_VID);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);



	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	pd3dImmediateContext->IASetVertexBuffers(0, 1, &m_VertexBufferforActorGizmo, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(m_IndexBufferforActorGizmo, DXGI_FORMAT_R32_UINT, 0);

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		activeTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(m_IndexCountforActorGizmo, 0, 0);
	}

}

void ReflectionActor::BuildCubeFaceCamera(float x, float y, float z)
{
	// Generate the cube map about the given position.
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);

	// Look along each coordinate axis.
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};

	for (int i = 0; i < 6; ++i)
	{
		m_CubeMapCamera[i].LookAt(center, targets[i], ups[i]);
		m_CubeMapCamera[i].SetLens(0.5f*XM_PI, 1.0f, NEAR_DIST, FAR_DIST*FAR_DIST);
		m_CubeMapCamera[i].UpdateViewMatrix();
		m_CubeMapCamera[i].SetFrustumFromProjection();
	}
}

void ReflectionActor::BuildDynamicCubeMapViews(ID3D11Device* pd3dDevice)
{
	//
	// Cubemap is a special texture array with 6 elements.
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* cubeTex = 0;
	HR(pd3dDevice->CreateTexture2D(&texDesc, 0, &cubeTex));
	
	/*
	for (int i = 0; i < 6; ++i)
	{
		HR(pd3dDevice->CreateTexture2D(&texDesc, 0, &m_RenderTexture[i]));
	}
	*/

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		//HR(pd3dDevice->CreateRenderTargetView(m_RenderTexture[i], &rtvDesc, &m_DynamicCubeMapRTV[i]));
		HR(pd3dDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &m_DynamicCubeMapRTV[i]));
	}

	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(pd3dDevice->CreateShaderResourceView(cubeTex, &srvDesc, &m_DynamicCubeMapSRV));
	ReleaseCOM(cubeTex);


	//
	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	//

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = CubeMapSize;
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 0;
	depthTexDesc.ArraySize = 6;
	//depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;// DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	//depthTexDesc.MiscFlags = 0;
	depthTexDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	ID3D11Texture2D* depthTex = 0;
	HR(pd3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	//dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY; // D3D11_DSV_DIMENSION_TEXTURE2D;
	//dsvDesc.Texture2D.MipSlice = 0;
	//dsvDesc.Texture2DArray.MipSlice = 1;
	for (int i = 0; i < 6; ++i)
	{
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		HR(pd3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &m_DynamicCubeMapDSV[i]));
	}
	//
	// Create a shader resource view to the cubedepth map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC DsrvDesc;
	DsrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	DsrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;// D3D11_SRV_DIMENSION_TEXTURE2D;
	DsrvDesc.Texture2D.MostDetailedMip = 0;
	DsrvDesc.Texture2D.MipLevels = -1;

	HR(pd3dDevice->CreateShaderResourceView(depthTex, &DsrvDesc, &m_DynamicCubeMapDSRV));
	ReleaseCOM(cubeTex);



	ReleaseCOM(depthTex);

	//
	// Viewport for drawing into cubemap.
	// 

	m_CubeMapViewport.TopLeftX = 0.0f;
	m_CubeMapViewport.TopLeftY = 0.0f;
	m_CubeMapViewport.Width = (float)CubeMapSize;
	m_CubeMapViewport.Height = (float)CubeMapSize;
	m_CubeMapViewport.MinDepth = 0.0f;
	m_CubeMapViewport.MaxDepth = 1.0f;
}
