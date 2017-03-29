////////////////////////////////////////////////////////////////////////////////
// Filename: Scene.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Scene.h"


Scene::Scene()
{


	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Create the vertex array.
	m_Vertice = new Vertex[m_vertexCount];
	
	
	m_Vertice[0].TexCoord = (XMFLOAT2(0.0f, 0.0f));
	m_Vertice[1].TexCoord = (XMFLOAT2(1.0f, 1.0f));
	m_Vertice[2].TexCoord = (XMFLOAT2(0.0f, 1.0f));
	m_Vertice[3].TexCoord = (XMFLOAT2(0.0f, 0.0f));
	m_Vertice[4].TexCoord = (XMFLOAT2(1.0f, 0.0f));
	m_Vertice[5].TexCoord = (XMFLOAT2(1.0f, 1.0f));
	
}

Scene::Scene(const Scene& other)
{

}

Scene::~Scene()
{
	Shutdown();
}

bool Scene::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight)
{
	bool result;

	// Store the screen size.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = resolutionWidth;
	m_bitmapHeight = resolutionHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	
	if (!result)
		return false;
	

	return true;
}


void Scene::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}
void Scene::OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight)
{
	m_screenWidth = ScreenWidth / m_ScreenDivisonX;
	m_screenHeight = ScreenHeight / m_ScreenDivisonY;
	m_bitmapWidth = MapWidth/ m_TextureDivisonX;
	m_bitmapHeight = MapHeight/ m_TextureDivisonY;

	//UpdateBuffers(deviceContext, positionX, positionY, InvViewProj);
}

void Scene::RenderBuffer(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effect* pEffect)
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

bool Scene::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects)
{	

	//RenderBuffer(deviceContext, positionX, positionY, InvViewProj, pTech, pEffects->BRDFLightingFX);

	
	if(pTech == pEffects->BRDFLightingFX->mTech)
		RenderBuffer(deviceContext, positionX, positionY, InvViewProj, pTech, pEffects->BRDFLightingFX);	
	else if (pTech == pEffects->SceneFX->SceneTech)
		RenderBuffer(deviceContext, positionX, positionY, InvViewProj, pTech, pEffects->SceneFX);	

	return true;
}


int Scene::GetIndexCount()
{
	return m_indexCount;
}


bool Scene::InitializeBuffers(ID3D11Device* device)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

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
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

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
	
	return true;
}


void Scene::ShutdownBuffers()
{
	// Release the index buffer.
	ReleaseCOM(m_indexBuffer);
	ReleaseCOM(m_vertexBuffer);
	
	if (m_Vertice)
	{
		delete[] m_Vertice;
		m_Vertice = NULL;
	}

	return;
}


XMFLOAT4 Scene::CalculateVertex(float X, float Y, XMFLOAT4X4 InvViewProj)
{
	XMFLOAT4 vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(X, Y, 0.5f, 1.0f), InvViewProj);

	vertex.x = vertex.x / vertex.w;
	vertex.y = vertex.y / vertex.w;
	vertex.z = vertex.z / vertex.w;
	vertex.w = 1.0f;
	return vertex;
}

bool Scene::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj)
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

	// Load the vertex array with data.
	XMFLOAT4 vertex = CalculateVertex(left, top, InvViewProj);

	// First triangle.
	m_Vertice[0].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z);  // Top left.
	
	vertex = CalculateVertex(right, bottom, InvViewProj);

	m_Vertice[1].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z); // Bottom right.	

	vertex = CalculateVertex(left, bottom, InvViewProj);

	m_Vertice[2].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z); // Bottom left.	

	vertex = CalculateVertex(left, top, InvViewProj);

	// Second triangle.
	m_Vertice[3].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z);  // Top left.	

	vertex = CalculateVertex(right, top, InvViewProj);

	m_Vertice[4].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z);  // Top right.	

	vertex = CalculateVertex(right, bottom, InvViewProj);

	m_Vertice[5].Position = XMFLOAT3(vertex.x, vertex.y, vertex.z);  // Bottom right.	

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


void Scene::RenderBuffers(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTech, Effect* pEffect)
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
