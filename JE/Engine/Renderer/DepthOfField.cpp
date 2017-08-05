////////////////////////////////////////////////////////////////////////////////
// Filename: DepthOfField.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DepthOfField.h"

DepthOfField::DepthOfField()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 4;

	// Create the vertex array.
	m_Vertice = new Vertex[m_vertexCount];

	m_Vertice[0].TexCoord = (XMFLOAT2(0.0f, 0.0f));
	m_Vertice[1].TexCoord = (XMFLOAT2(1.0f, 1.0f));
	m_Vertice[2].TexCoord = (XMFLOAT2(0.0f, 1.0f));
	m_Vertice[3].TexCoord = (XMFLOAT2(1.0f, 0.0f));


	DofVal.x = NEAR_DIST;
	DofVal.y = 20.0f;
	DofVal.z = FAR_DIST*FAR_DIST;
	//DofVal.z = FAR_DIST;
	DofVal.w = FAR_DIST*10.0f;
}


DepthOfField::~DepthOfField()
{
	Shutdown();
}


bool DepthOfField::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
	bool result;
	
	pDevice = device;

	//DownSample
	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = resolutionWidth;
	m_bitmapHeight = resolutionHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;


	CreateFrameBuffers(device);

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);

	if (!result)
		return false;


	return true;
}



void DepthOfField::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();
	delete m_DOFTexture;
	delete m_GaussianVTexture;
	delete m_GaussianHTexture;
	
	return;
}
void DepthOfField::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	//DownSample
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth / m_TextureDivisonX;
	m_bitmapHeight = MapHeight / m_TextureDivisonY;

	m_DOFTexture->OnResize(m_bitmapWidth, m_bitmapHeight, pdeviceContext, NULL);
	m_GaussianVTexture->OnResize(m_bitmapWidth, m_bitmapHeight, pdeviceContext, NULL);
	m_GaussianHTexture->OnResize(m_bitmapWidth, m_bitmapHeight, pdeviceContext, NULL);
}


bool DepthOfField::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{	
	
	pdeviceContext = deviceContext;

	UpdateBasicMatrix(*mWorld, pCameraForRenderWindow);
	pdeviceContext->IASetInputLayout(InputLayouts::BRDFL_VID);

	float gap[7];

	float kernelSize = 1.0f;

	

	gap[0] = kernelSize / ((float)m_bitmapWidth);
	gap[1] = gap[0] + gap[0];
	gap[2] = gap[1] + gap[0];
	gap[3] = gap[2] + gap[0];
	gap[4] = gap[3] + gap[0];
	gap[5] = gap[4] + gap[0];
	gap[6] = gap[5] + gap[0];


	SetBackBufferRenderTarget(m_GaussianHTexture->GetRenderTargetView(), NULL, NULL, NULL, FALSE, TRUE, FALSE, FALSE);

	//XMFLOAT2 PixelSize = XMFLOAT2(1.0f / (float)m_bitmapWidth, 1.0f / (float)m_bitmapHeight);
	

	//activeTech = mEffects.HBAOFX->HBAOTech;
	pEffects->DOFFX->SetAllVariables(XMLoadFloat4x4(&mWVP), pCameraForRenderWindow->ViewProj(), pCamera->InvProj(),
		pRenderTextureManger->mRT_List.at(SCENETEXTURE)->GetShaderResourceView(), pMRTforDeferredR->GetShaderResourceView(3), m_HBAOTexture->GetShaderResourceView(), gap, DofVal);
	RenderBuffer(pdeviceContext, 0, 0, mInvViewProj, pEffects->DOFFX->HBlurTech, pEffects->DOFFX);
		
	pdeviceContext->GenerateMips(m_GaussianHTexture->GetShaderResourceView());

	gap[0] = kernelSize / ((float)m_bitmapHeight);
	gap[1] = gap[0] + gap[0];
	gap[2] = gap[1] + gap[0];
	gap[3] = gap[2] + gap[0];
	gap[4] = gap[3] + gap[0];
	gap[5] = gap[4] + gap[0];
	gap[6] = gap[5] + gap[0];

	SetBackBufferRenderTarget(m_GaussianVTexture->GetRenderTargetView(), NULL, NULL, NULL, FALSE, TRUE, FALSE, FALSE);
	
	//activeTech = mEffects.HBAOFX->GaussianVerticalTech;
	pEffects->DOFFX->SetAllVariables(XMLoadFloat4x4(&mWVP), pCameraForRenderWindow->ViewProj(), pCamera->InvProj(),
		m_GaussianHTexture->GetShaderResourceView(), pMRTforDeferredR->GetShaderResourceView(3), NULL, gap, DofVal);
	RenderBuffer(pdeviceContext, 0, 0, mInvViewProj, pEffects->DOFFX->VBlurTech, pEffects->DOFFX);

	pdeviceContext->GenerateMips(m_GaussianVTexture->GetShaderResourceView());
	
	

	SetBackBufferRenderTarget(m_DOFTexture->GetRenderTargetView(), NULL, NULL, NULL, FALSE, TRUE, FALSE, FALSE);
	
	//activeTech = mEffects.HBAOFX->GaussianHeightTech;
	pEffects->DOFFX->SetAllVariables(XMLoadFloat4x4(&mWVP), pCameraForRenderWindow->ViewProj(), pCamera->InvProj(),
		pRenderTextureManger->mRT_List.at(SCENETEXTURE)->GetShaderResourceView(), pMRTforDeferredR->GetShaderResourceView(3), m_GaussianVTexture->GetShaderResourceView(), gap, DofVal);
	RenderBuffer(pdeviceContext, 0, 0, mInvViewProj, pEffects->DOFFX->DOFTech, pEffects->DOFFX);

	

	pdeviceContext->GenerateMips(m_DOFTexture->GetShaderResourceView());

	
	//SetBackBufferRenderTarget(pRenderTextureManger->mRT_List.at(SCENETEXTURE)->GetRenderTargetView(), NULL, NULL, NULL,   FALSE, TRUE, FALSE, FALSE);	
	SetBackBufferRenderTarget(FALSE, FALSE, FALSE, FALSE);

	SetBlendState(FALSE, FALSE, TRUE, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_ZERO, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL);
	pEffects->SceneFX->SetAllVariables(XMLoadFloat4x4(&mWVP), XMLoadFloat4x4(&mViewProj), m_DOFTexture->GetShaderResourceView(), NULL);
	pScene->Render(pdeviceContext, 0, 0, mInvViewProj, pEffects->SceneFX->SceneTech, pEffects);

	pdeviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	ReleaseCOM(pBlendState);
	
	/*
	SetBackBufferRenderTarget(FALSE, TRUE, FALSE, FALSE);
	UpdateBasicMatrix(*mWorld, pCameraForRenderWindow);

	pEffects->SceneFX->SetAllVariables(XMLoadFloat4x4(&mWVP), XMLoadFloat4x4(&mViewProj), pRenderTextureManger->mRT_List.at(SCENETEXTURE)->GetShaderResourceView(), m_DOFTexture->GetShaderResourceView());
	pScene->Render(pdeviceContext, 0, 0, mInvViewProj, pEffects->SceneFX->CompTech, pEffects);
	*/

	//pdeviceContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	
	return true;
}

void DepthOfField::SetBlendState(bool bAlphaToCoverageEnable, bool bIndependentBlendEnable, bool bBlendEnable,
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

void DepthOfField::RenderBuffer(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effect* pEffect)
{
	bool result;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.

	result = UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
	if (!result)
	{
		return;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext, pTech, pEffect);
}

int DepthOfField::GetIndexCount()
{
	return m_indexCount;
}


bool DepthOfField::InitializeBuffers(ID3D11Device* device)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of indices in the index array.
	m_indexCount = 6;

	// Create the vertex array.
	vertices = new Vertex[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(Vertex)* m_vertexCount));

	// Load the index array with data.
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 1;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;


	//CreateFrameBuffers(device);

	return true;
}


void DepthOfField::ShutdownBuffers()
{
	// Release the index buffer.
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_vertexBuffer);

	m_DOFTexture->Shutdown();
	m_GaussianVTexture->Shutdown();
	m_GaussianHTexture->Shutdown();

	if (m_Vertice)
	{
		delete[] m_Vertice;
		m_Vertice = NULL;
	}

	return;
}


XMFLOAT4 DepthOfField::CalculateVertex(float X, float Y, XMFLOAT4X4 InvViewProj)
{
	XMFLOAT4 vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(X, Y, 0.5f, 1.0f), InvViewProj);

	vertex.x = vertex.x / vertex.w;
	vertex.y = vertex.y / vertex.w;
	vertex.z = vertex.z / vertex.w;
	vertex.w = 1.0f;
	return vertex;
}

bool DepthOfField::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj)
{
	float left, right, top, bottom;
	//Vertex* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Vertex* verticesPtr;
	HRESULT result;

	left = -1.0f + ((float)positionX / (float)m_screenWidth) * 2.0f;
	right = -1.0f + (((float)m_bitmapWidth + (float)positionX) / (float)m_screenWidth) *2.0f;

	top = 1.0f - ((float)positionY / (float)m_screenHeight) * 2.0f;
	bottom = 1.0f - (((float)m_bitmapHeight + (float)positionY) / (float)m_screenHeight) *2.0f;

	//left = -1.0f;
	//right = 1.0f;
	//top = 1.0f;
	//bottom = -1.0f;

	// Load the vertex array with data.	
	// First triangle.

	XMFLOAT4 vertex = CalculateVertex(left, top, InvViewProj);
	m_Vertice[0].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z);  // Top left.

	vertex = CalculateVertex(right, bottom, InvViewProj);
	m_Vertice[1].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z); // Bottom right.	

	vertex = CalculateVertex(left, bottom, InvViewProj);
	m_Vertice[2].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z); // Bottom left.	

	vertex = CalculateVertex(right, top, InvViewProj);
	m_Vertice[3].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z);  // Top right.	

																	 // Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (Vertex*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_Vertice, (sizeof(Vertex)* m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}


void DepthOfField::RenderBuffers(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTech, Effect* pEffect)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);

	//Render
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, deviceContext);
		deviceContext->DrawIndexed(GetIndexCount(), 0, 0);
	}

	//ReleaseTextureResourse
	Effects::ReleaseTextureResource(deviceContext, pTech, pEffect);

	return;
}

bool DepthOfField::CreateFrameBuffers(ID3D11Device* pd3dDevice)
{
	m_DOFTexture = new RenderTexture();
	m_DOFTexture->Initialize(pd3dDevice, m_bitmapWidth, m_bitmapHeight, 0, 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		FALSE, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS);

	m_GaussianVTexture = new RenderTexture();
	m_GaussianVTexture->Initialize(pd3dDevice, m_bitmapWidth, m_bitmapHeight, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		FALSE, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS);

	m_GaussianHTexture = new RenderTexture();
	m_GaussianHTexture->Initialize(pd3dDevice, m_bitmapWidth, m_bitmapHeight, 0, 1, DXGI_FORMAT_R8G8B8A8_UNORM,
		FALSE, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS);

	return true;
}

int DepthOfField::GetScreenWidth()
{
	return m_screenWidth;
}

int DepthOfField::GetScreenHeight()
{
	return m_screenHeight;
}