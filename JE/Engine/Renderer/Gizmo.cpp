////////////////////////////////////////////////////////////////////////////////
// Filename: DebugWindow.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Gizmo.h"

Gizmo::Gizmo()
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;

	m_TvertexBuffer = NULL;
	m_TindexBuffer = NULL;

	m_RvertexBuffer = NULL;
	m_RindexBuffer = NULL;

	m_SvertexBuffer = NULL;
	m_SindexBuffer = NULL;

	m_vertexCount = 30;
	m_indexCount = 30;

	m_TvertexCount = 42 * 3 + 36;
	m_TindexCount = 42 * 3 + 36;

	m_RvertexCount = 24 * 16 * 3;
	m_RindexCount = 24 * 16 * 3;

	m_SvertexCount = 60 * 3 + 36;
	m_SindexCount = 60 * 3 + 36;

	mTvertices = NULL;
	mSvertices = NULL;

	mBasicGizmoVertices = NULL;

	X_AXIS = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Y_AXIS = XMFLOAT3(0.0f, 1.0f, 0.0f);
	Z_AXIS = XMFLOAT3(0.0f, 0.0f, 1.0f);

	mGizmoStatus = 0;
	
}


Gizmo::~Gizmo()
{
	Shutdown();
	if (mTvertices)
		delete[] mTvertices;

	if (mRvertices)
		delete[] mRvertices;

	if (mSvertices)
		delete[] mSvertices;

	if (mBasicGizmoVertices)
		delete[] mBasicGizmoVertices;
	
}


bool Gizmo::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight)
{
	bool result;

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device, deviceContext);
	if (!result)
	{
		return false;
	}

	mBasicGizmoVertices = new Vertex[m_vertexCount];

	mBasicGizmoVertices[0].Position = XMFLOAT3(0.0f,0.0f,0.0f);
	mBasicGizmoVertices[1].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);

	mBasicGizmoVertices[2].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mBasicGizmoVertices[3].Position = XMFLOAT3(0.8f, 0.1f, 0.1f);

	mBasicGizmoVertices[4].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mBasicGizmoVertices[5].Position = XMFLOAT3(0.8f, -0.1f, 0.1f);
	
	mBasicGizmoVertices[6].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mBasicGizmoVertices[7].Position = XMFLOAT3(0.8f, 0.1f, -0.1f);

	mBasicGizmoVertices[8].Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mBasicGizmoVertices[9].Position = XMFLOAT3(0.8f, -0.1f, -0.1f);
	
	mBasicGizmoVertices[10].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mBasicGizmoVertices[11].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);

	mBasicGizmoVertices[12].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mBasicGizmoVertices[13].Position = XMFLOAT3(0.1f, 0.8f, 0.1f);
	
	mBasicGizmoVertices[14].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mBasicGizmoVertices[15].Position = XMFLOAT3(-0.1f, 0.8f, 0.1f);

	mBasicGizmoVertices[16].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mBasicGizmoVertices[17].Position = XMFLOAT3(0.1f, 0.8f, -0.1f);

	mBasicGizmoVertices[18].Position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mBasicGizmoVertices[19].Position = XMFLOAT3(-0.1f, 0.8f, -0.1f);

	mBasicGizmoVertices[20].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mBasicGizmoVertices[21].Position = XMFLOAT3(0.0f, 0.0f, 1.0f);

	mBasicGizmoVertices[22].Position = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mBasicGizmoVertices[23].Position = XMFLOAT3(0.1f, 0.1f, 0.8f);

	mBasicGizmoVertices[24].Position = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mBasicGizmoVertices[25].Position = XMFLOAT3(0.1f, -0.1f, 0.8f);

	mBasicGizmoVertices[26].Position = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mBasicGizmoVertices[27].Position = XMFLOAT3(-0.1f, 0.1f, 0.8f);

	mBasicGizmoVertices[28].Position = XMFLOAT3(0.0f, 0.0f, 1.0f);
	mBasicGizmoVertices[29].Position = XMFLOAT3(-0.1f, -0.1f, 0.8f);

	// X - axis
	for (int i = 0; i < 10; i++)
	{	
		mBasicGizmoVertices[i].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	//Y - axis
	for (int i = 10; i < 20; i++)
	{
		mBasicGizmoVertices[i].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	}

	//Z - axis
	for (int i = 20; i < 30; i++)
	{
		mBasicGizmoVertices[i].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	for (int i = 0; i < 30; i++)
	{
		mBasicGizmoVertices[i].Position = MathHelper::XMFLOAT3_MUL_SCALAR(mBasicGizmoVertices[i].Position, 15.0f);
	}

	// Lock the vertex buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Vertex* verticesPtr;

	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (Vertex*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)mBasicGizmoVertices, (sizeof(Vertex)* m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	mTvertices = new Vertex[m_TvertexCount];

	//a - 0.0 b - 3.0 c1 - 0.1 c2 - -0.1 d1 - 0.3 d2 - -0.3 e - 4.0
	mTvertices[0].Position = XMFLOAT3(0.0f, 0.1f, -0.1f);
	mTvertices[1].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mTvertices[2].Position = XMFLOAT3(0.0f, -0.1f, -0.1f);
	mTvertices[3].Position = XMFLOAT3(0.0f, 0.1f, -0.1f);
	mTvertices[4].Position = XMFLOAT3(3.0f, 0.1f, -0.1f);
	mTvertices[5].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mTvertices[6].Position = XMFLOAT3(0.0f, 0.1f, -0.1f);
	mTvertices[7].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mTvertices[8].Position = XMFLOAT3(3.0f, 0.1f, -0.1f);
	mTvertices[9].Position = XMFLOAT3(3.0f, 0.1f, -0.1f);
	mTvertices[10].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mTvertices[11].Position = XMFLOAT3(3.0f, 0.1f, 0.1f);
	mTvertices[12].Position = XMFLOAT3(0.0f, -0.1f, 0.1f);
	mTvertices[13].Position = XMFLOAT3(0.0f, -0.1f, -0.1f);
	mTvertices[14].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mTvertices[15].Position = XMFLOAT3(0.0f, -0.1f, 0.1f);
	mTvertices[16].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mTvertices[17].Position = XMFLOAT3(3.0f, -0.1f, 0.1f);
	mTvertices[18].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mTvertices[19].Position = XMFLOAT3(0.0f, -0.1f, 0.1f);
	mTvertices[20].Position = XMFLOAT3(3.0f, -0.1f, 0.1f);
	mTvertices[21].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mTvertices[22].Position = XMFLOAT3(3.0f, -0.1f, 0.1f);
	mTvertices[23].Position = XMFLOAT3(3.0f, 0.1f, 0.1f);
	mTvertices[24].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mTvertices[25].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mTvertices[26].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mTvertices[27].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mTvertices[28].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mTvertices[29].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mTvertices[30].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mTvertices[31].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mTvertices[32].Position = XMFLOAT3(4.0f, 0.0f, 0.0f);
	mTvertices[33].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mTvertices[34].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mTvertices[35].Position = XMFLOAT3(4.0f, 0.0f, 0.0f);
	mTvertices[36].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mTvertices[37].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mTvertices[38].Position = XMFLOAT3(4.0f, 0.0f, 0.0f);
	mTvertices[39].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mTvertices[40].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mTvertices[41].Position = XMFLOAT3(4.0f, 0.0f, 0.0f);

	XMFLOAT4X4 tempMat;

	for (int i = 0; i < 42; i++)
	{
		XMStoreFloat4x4(&tempMat, XMMatrixRotationAxis(MathHelper::XMFLOAT3_TO_XMVECTOR(XMFLOAT3(0.0f,0.0f,1.0f), 1.0f), MathHelper::ConvertToRadians(90.0f)));
		XMFLOAT4 result = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(mTvertices[i].Position, 1.0f), tempMat);
		mTvertices[42 + i].Position = XMFLOAT3(result.x, result.y, result.z);
	}

	for (int i = 0; i < 42; i++)
	{
		XMStoreFloat4x4(&tempMat, XMMatrixRotationAxis(MathHelper::XMFLOAT3_TO_XMVECTOR(XMFLOAT3(0.0f, 1.0f, 0.0f), 1.0f), MathHelper::ConvertToRadians(-90.0f)));
		XMFLOAT4 result = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(mTvertices[i].Position, 1.0f), tempMat);
		mTvertices[42*2 + i].Position = XMFLOAT3(result.x, result.y, result.z);
	}

	mTvertices[42 * 3].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 1].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 2].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 3].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 4].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 5].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);

	mTvertices[42 * 3 + 6].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 7].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 8].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 9].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 10].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 11].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);

	mTvertices[42 * 3 + 12].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 13].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 14].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);
	mTvertices[42 * 3 + 15].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 16].Position = XMFLOAT3(0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 17].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);

	mTvertices[42 * 3 + 18].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 19].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);
	mTvertices[42 * 3 + 20].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);
	mTvertices[42 * 3 + 21].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 22].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);
	mTvertices[42 * 3 + 23].Position = XMFLOAT3(0.3f, 0.3f, 0.3f);

	mTvertices[42 * 3 + 24].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 25].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 26].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mTvertices[42 * 3 + 27].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 28].Position = XMFLOAT3(0.3f, 0.3f, 0.3f);
	mTvertices[42 * 3 + 29].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);

	mTvertices[42 * 3 + 30].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);
	mTvertices[42 * 3 + 31].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 32].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 33].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);
	mTvertices[42 * 3 + 34].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mTvertices[42 * 3 + 35].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);

	for (int i = 0; i < m_TvertexCount; i++)
	{		
		mTvertices[i].Position = MathHelper::XMFLOAT3_MUL_SCALAR(mTvertices[i].Position, 30.0f);

		if (i < 42)
		{
			mTvertices[i].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

			if (24 < i && i < 30)
			{
				mTvertices[i].Color = XMFLOAT4(0.75f, 0.0f, 0.0f, 1.0f);
			}
		}			
		else if (i < 42 * 2)
		{
			mTvertices[i].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
			
			if (42 + 24 < i && i < 42 + 30)
			{
				mTvertices[i].Color = XMFLOAT4(0.0f, 0.75f, 0.0f, 1.0f);
			}
		}
			
		else if (i < 42 * 3)
		{
			mTvertices[i].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

			if (42*2 + 24 < i && i < 42*2 + 30)
			{
				mTvertices[i].Color = XMFLOAT4(0.0f, 0.0f, 0.75f, 1.0f);
			}
		}
			
		else
		{
			mTvertices[i].Color = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
		}
			
	}

	
	BoundingBox::CreateFromPoints(mAxis_X_AlignedBox, 42, &mTvertices[0].Position, sizeof(Vertex));
	BoundingBox::CreateFromPoints(mAxis_Y_AlignedBox, 42, &mTvertices[42].Position, sizeof(Vertex));
	BoundingBox::CreateFromPoints(mAxis_Z_AlignedBox, 42, &mTvertices[42 * 2].Position, sizeof(Vertex));
	BoundingBox::CreateFromPoints(mMiddle_AlignedBox, 36, &mTvertices[42 * 3].Position, sizeof(Vertex));

	// Lock the vertex buffer so it can be written to.
	if (FAILED(deviceContext->Map(m_TvertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
	return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (Vertex*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)mTvertices, (sizeof(Vertex)* m_TvertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_TvertexBuffer, 0);
	
	
	mRvertices = new Vertex[m_RvertexCount];

	float SinVal = sin(MathHelper::ConvertToRadians(11.25f));
	float CosVal = cos(MathHelper::ConvertToRadians(11.25f));

	mRvertices[0].Position = XMFLOAT3(CosVal, 0.1f, -SinVal);
	mRvertices[1].Position = XMFLOAT3(CosVal, 0.1f, SinVal);
	mRvertices[2].Position = XMFLOAT3(CosVal*1.1f, 0.1f, -SinVal*1.1f);
	
	mRvertices[3].Position = XMFLOAT3(CosVal, 0.1f, SinVal);
	mRvertices[4].Position = XMFLOAT3(CosVal*1.1f, 0.1f, SinVal*1.1f);
	mRvertices[5].Position = XMFLOAT3(CosVal*1.1f, 0.1f, -SinVal*1.1f);
	
	mRvertices[6].Position = XMFLOAT3(CosVal, -0.1f, SinVal);
	mRvertices[7].Position = XMFLOAT3(CosVal, -0.1f, -SinVal);
	mRvertices[8].Position = XMFLOAT3(CosVal*1.1f, -0.1f, -SinVal*1.1f);

	mRvertices[9].Position = XMFLOAT3(CosVal, -0.1f, SinVal);
	mRvertices[10].Position = XMFLOAT3(CosVal*1.1f, -0.1f, -SinVal*1.1f);
	mRvertices[11].Position = XMFLOAT3(CosVal*1.1f, -0.1f, SinVal*1.1f);
	
	mRvertices[12].Position = XMFLOAT3(CosVal, 0.1f, -SinVal);
	mRvertices[13].Position = XMFLOAT3(CosVal, -0.1f, SinVal);
	mRvertices[14].Position = XMFLOAT3(CosVal, 0.1f, SinVal);

	mRvertices[15].Position = XMFLOAT3(CosVal, -0.1f, SinVal);
	mRvertices[16].Position = XMFLOAT3(CosVal, 0.1f, -SinVal);
	mRvertices[17].Position = XMFLOAT3(CosVal, -0.1f, -SinVal);

	mRvertices[18].Position = XMFLOAT3(CosVal*1.1f, 0.1f, -SinVal*1.1f);
	mRvertices[19].Position = XMFLOAT3(CosVal*1.1f, 0.1f, SinVal*1.1f);
	mRvertices[20].Position = XMFLOAT3(CosVal*1.1f, -0.1f, SinVal*1.1f);

	mRvertices[21].Position = XMFLOAT3(CosVal*1.1f, -0.1f, SinVal*1.1f);
	mRvertices[22].Position = XMFLOAT3(CosVal*1.1f, -0.1f, -SinVal*1.1f);
	mRvertices[23].Position = XMFLOAT3(CosVal*1.1f, 0.1f, -SinVal*1.1f);

	for (int i = 0; i < 24; i++)
	{
		mRvertices[i].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		if (i >= 12 && i <= 17)
			mRvertices[i].Color = XMFLOAT4(0.75f, 0.0f, 0.0f, 1.0f);

		XMFLOAT3 tempPos = MathHelper::XMFLOAT3_MUL_SCALAR(mRvertices[i].Position, 90.0f);
		mRvertices[i].Position.x = tempPos.x;
		mRvertices[i].Position.y = tempPos.y*0.5f;
		mRvertices[i].Position.z = tempPos.z;
	}


	
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	for (int j = 1; j < 16; j++)
	{
		for (int i = 0; i < 24; i++)
		{		
			XMStoreFloat4x4(&tempMat, XMMatrixRotationAxis(MathHelper::XMFLOAT3_TO_XMVECTOR(up, 1.0f), MathHelper::ConvertToRadians(22.5f*(float)j)));
			XMFLOAT4 result = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(mRvertices[i].Position, 1.0f), tempMat);
			mRvertices[24*j + i].Position = XMFLOAT3(result.x, result.y, result.z);
			mRvertices[24 * j + i].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

			if (i>=12 && i <=17)
				mRvertices[24 * j + i].Color = XMFLOAT4(0.75f, 0.0f, 0.0f, 1.0f);
		}
	}

	XMFLOAT3 foward = XMFLOAT3(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 24*16; i++)
	{
		XMStoreFloat4x4(&tempMat, XMMatrixRotationAxis(MathHelper::XMFLOAT3_TO_XMVECTOR(foward, 1.0f), MathHelper::ConvertToRadians(90.0f)));
		XMFLOAT4 result = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(mRvertices[i].Position, 1.0f), tempMat);
		mRvertices[i + 24 * 16].Position = XMFLOAT3(result.x, result.y, result.z);
		
		mRvertices[i + 24 * 16].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		if (mRvertices[i].Color.x == 0.75f)
			mRvertices[i + 24 * 16].Color = XMFLOAT4(0.0f, 0.75f, 0.0f, 1.0f);
		
	}

	XMFLOAT3 right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	for (int i = 0; i < 24 * 16; i++)
	{
		XMStoreFloat4x4(&tempMat, XMMatrixRotationAxis(MathHelper::XMFLOAT3_TO_XMVECTOR(right, 1.0f), MathHelper::ConvertToRadians(90.0f)));
		XMFLOAT4 result = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(mRvertices[i].Position, 1.0f), tempMat);
		mRvertices[i + 24*16*2].Position = XMFLOAT3(result.x, result.y, result.z);

		mRvertices[i + 24 * 16 * 2].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

		if (mRvertices[i].Color.x == 0.75f)
			mRvertices[i + 24*16*2].Color = XMFLOAT4(0.0f, 0.0f, 0.75f, 1.0f);

	}

	BoundingBox::CreateFromPoints(mRAxis_X_AlignedBox, 24 * 16, &mRvertices[0].Position, sizeof(Vertex));
	BoundingBox::CreateFromPoints(mRAxis_Y_AlignedBox, 24 * 16, &mRvertices[24 * 16].Position, sizeof(Vertex));
	BoundingBox::CreateFromPoints(mRAxis_Z_AlignedBox, 24 * 16, &mRvertices[24 * 16 * 2].Position, sizeof(Vertex));
	
	
	

	// Lock the vertex buffer so it can be written to.
	if (FAILED(deviceContext->Map(m_RvertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (Vertex*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)mRvertices, (sizeof(Vertex)* m_RvertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_RvertexBuffer, 0);



	mSvertices = new Vertex[m_SvertexCount];

	//a - 0.0 b - 3.0 c1 - 0.1 c2 - -0.1 d1 - 0.3 d2 - -0.3 e - 4.0
	mSvertices[0].Position = XMFLOAT3(0.0f, 0.1f, -0.1f);
	mSvertices[1].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mSvertices[2].Position = XMFLOAT3(0.0f, -0.1f, -0.1f);
	mSvertices[3].Position = XMFLOAT3(0.0f, 0.1f, -0.1f);
	mSvertices[4].Position = XMFLOAT3(3.0f, 0.1f, -0.1f);
	mSvertices[5].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mSvertices[6].Position = XMFLOAT3(0.0f, 0.1f, -0.1f);
	mSvertices[7].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mSvertices[8].Position = XMFLOAT3(3.0f, 0.1f, -0.1f);
	mSvertices[9].Position = XMFLOAT3(3.0f, 0.1f, -0.1f);
	mSvertices[10].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mSvertices[11].Position = XMFLOAT3(3.0f, 0.1f, 0.1f);
	mSvertices[12].Position = XMFLOAT3(0.0f, -0.1f, 0.1f);
	mSvertices[13].Position = XMFLOAT3(0.0f, -0.1f, -0.1f);
	mSvertices[14].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mSvertices[15].Position = XMFLOAT3(0.0f, -0.1f, 0.1f);
	mSvertices[16].Position = XMFLOAT3(3.0f, -0.1f, -0.1f);
	mSvertices[17].Position = XMFLOAT3(3.0f, -0.1f, 0.1f);
	mSvertices[18].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mSvertices[19].Position = XMFLOAT3(0.0f, -0.1f, 0.1f);
	mSvertices[20].Position = XMFLOAT3(3.0f, -0.1f, 0.1f);
	mSvertices[21].Position = XMFLOAT3(0.0f, 0.1f, 0.1f);
	mSvertices[22].Position = XMFLOAT3(3.0f, -0.1f, 0.1f);
	mSvertices[23].Position = XMFLOAT3(3.0f, 0.1f, 0.1f);

	mSvertices[24].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mSvertices[25].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mSvertices[26].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mSvertices[27].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mSvertices[28].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mSvertices[29].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mSvertices[30].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mSvertices[31].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mSvertices[32].Position = XMFLOAT3(3.6f, -0.3f, -0.3f);
	mSvertices[33].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mSvertices[34].Position = XMFLOAT3(3.6f, 0.3f, -0.3f);
	mSvertices[35].Position = XMFLOAT3(3.6f, -0.3f, -0.3f);
	mSvertices[36].Position = XMFLOAT3(3.6f, -0.3f, -0.3f);
	mSvertices[37].Position = XMFLOAT3(3.6f, 0.3f, -0.3f);
	mSvertices[38].Position = XMFLOAT3(3.6f, -0.3f, 0.3f);
	mSvertices[39].Position = XMFLOAT3(3.6f, 0.3f, -0.3f);
	mSvertices[40].Position = XMFLOAT3(3.6f, 0.3f, 0.3f);
	mSvertices[41].Position = XMFLOAT3(3.6f, -0.3f, 0.3f);
	mSvertices[42].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mSvertices[43].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mSvertices[44].Position = XMFLOAT3(3.6f, -0.3f, 0.3f);
	mSvertices[45].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mSvertices[46].Position = XMFLOAT3(3.6f, -0.3f, 0.3f);
	mSvertices[47].Position = XMFLOAT3(3.6f, 0.3f, 0.3f);
	mSvertices[48].Position = XMFLOAT3(3.0f, 0.3f, -0.3f);
	mSvertices[49].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mSvertices[50].Position = XMFLOAT3(3.6f, 0.3f, -0.3f);
	mSvertices[51].Position = XMFLOAT3(3.0f, 0.3f, 0.3f);
	mSvertices[52].Position = XMFLOAT3(3.6f, 0.3f, 0.3f);
	mSvertices[53].Position = XMFLOAT3(3.6f, 0.3f, -0.3f);
	mSvertices[54].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mSvertices[55].Position = XMFLOAT3(3.0f, -0.3f, -0.3f);
	mSvertices[56].Position = XMFLOAT3(3.6f, -0.3f, -0.3f);
	mSvertices[57].Position = XMFLOAT3(3.0f, -0.3f, 0.3f);
	mSvertices[58].Position = XMFLOAT3(3.6f, -0.3f, -0.3f);
	mSvertices[59].Position = XMFLOAT3(3.6f, -0.3f, 0.3f);

	mSvertices[60].Position = XMFLOAT3(0.1f, 0.0f, -0.1f);
	mSvertices[60 + 1].Position = XMFLOAT3(-0.1f, 3.0f, -0.1f);
	mSvertices[60 + 2].Position = XMFLOAT3(-0.1f, 0.0f, -0.1f);
	mSvertices[60 + 3].Position = XMFLOAT3(0.1f, 0.0f, -0.1f);
	mSvertices[60 + 4].Position = XMFLOAT3(0.1f, 3.0f, -0.1f);
	mSvertices[60 + 5].Position = XMFLOAT3(-0.1f, 3.0f, -0.1f);
	mSvertices[60 + 6].Position = XMFLOAT3(0.1f, 0.0f, -0.1f);
	mSvertices[60 + 7].Position = XMFLOAT3(0.1f, 0.0f, 0.1f);
	mSvertices[60 + 8].Position = XMFLOAT3(0.1f, 3.0f, -0.1f);
	mSvertices[60 + 9].Position = XMFLOAT3(0.1f, 3.0f, -0.1f);
	mSvertices[60 + 10].Position = XMFLOAT3(0.1f, 0.0f, 0.1f);
	mSvertices[60 + 11].Position = XMFLOAT3(0.1f, 3.0f, 0.1f);
	mSvertices[60 + 12].Position = XMFLOAT3(-0.1f, 0.0f, 0.1f);
	mSvertices[60 + 13].Position = XMFLOAT3(-0.1f, 0.0f, -0.1f);
	mSvertices[60 + 14].Position = XMFLOAT3(-0.1f, 3.0f, -0.1f);
	mSvertices[60 + 15].Position = XMFLOAT3(-0.1f, 0.0f, 0.1f);
	mSvertices[60 + 16].Position = XMFLOAT3(-0.1f, 3.0f, -0.1f);
	mSvertices[60 + 17].Position = XMFLOAT3(-0.1f, 3.0f, 0.1f);
	mSvertices[60 + 18].Position = XMFLOAT3(0.1f, 0.0f, 0.1f);
	mSvertices[60 + 19].Position = XMFLOAT3(-0.1f, 0.0f, 0.1f);
	mSvertices[60 + 20].Position = XMFLOAT3(-0.1f, 3.0f, 0.1f);
	mSvertices[60 + 21].Position = XMFLOAT3(0.1f, 0.0f, 0.1f);
	mSvertices[60 + 22].Position = XMFLOAT3(-0.1f, 3.0f, 0.1f);
	mSvertices[60 + 23].Position = XMFLOAT3(0.1f, 3.0f, 0.1f);

	mSvertices[60 + 24].Position = XMFLOAT3(0.3f, 3.0f, -0.3f);
	mSvertices[60 + 25].Position = XMFLOAT3(-0.3f, 3.0f, -0.3f);
	mSvertices[60 + 26].Position = XMFLOAT3(0.3f, 3.0f, 0.3f);
	mSvertices[60 + 27].Position = XMFLOAT3(0.3f, 3.0f, 0.3f);
	mSvertices[60 + 28].Position = XMFLOAT3(-0.3f, 3.0f, -0.3f);
	mSvertices[60 + 29].Position = XMFLOAT3(-0.3f, 3.0f, 0.3f);
	mSvertices[60 + 30].Position = XMFLOAT3(-0.3f, 3.0f, -0.3f);
	mSvertices[60 + 31].Position = XMFLOAT3(0.3f, 3.0f, -0.3f);
	mSvertices[60 + 32].Position = XMFLOAT3(-0.3f, 3.6f, -0.3f);
	mSvertices[60 + 33].Position = XMFLOAT3(0.3f, 3.0f, -0.3f);
	mSvertices[60 + 34].Position = XMFLOAT3(0.3f, 3.6f, -0.3f);
	mSvertices[60 + 35].Position = XMFLOAT3(-0.3f, 3.6f, -0.3f);
	mSvertices[60 + 36].Position = XMFLOAT3(-0.3f, 3.6f, -0.3f);
	mSvertices[60 + 37].Position = XMFLOAT3(0.3f, 3.6f, -0.3f);
	mSvertices[60 + 38].Position = XMFLOAT3(-0.3f, 3.6f, 0.3f);
	mSvertices[60 + 39].Position = XMFLOAT3(0.3f, 3.6f, -0.3f);
	mSvertices[60 + 40].Position = XMFLOAT3(0.3f, 3.6f, 0.3f);
	mSvertices[60 + 41].Position = XMFLOAT3(-0.3f, 3.6f, 0.3f);
	mSvertices[60 + 42].Position = XMFLOAT3(0.3f, 3.0f, 0.3f);
	mSvertices[60 + 43].Position = XMFLOAT3(-0.3f, 3.0f, 0.3f);
	mSvertices[60 + 44].Position = XMFLOAT3(-0.3f, 3.6f, 0.3f);
	mSvertices[60 + 45].Position = XMFLOAT3(0.3f, 3.0f, 0.3f);
	mSvertices[60 + 46].Position = XMFLOAT3(-0.3f, 3.6f, 0.3f);
	mSvertices[60 + 47].Position = XMFLOAT3(0.3f, 3.6f, 0.3f);
	mSvertices[60 + 48].Position = XMFLOAT3(0.3f, 3.0f, -0.3f);
	mSvertices[60 + 49].Position = XMFLOAT3(0.3f, 3.0f, 0.3f);
	mSvertices[60 + 50].Position = XMFLOAT3(0.3f, 3.6f, -0.3f);
	mSvertices[60 + 51].Position = XMFLOAT3(0.3f, 3.0f, 0.3f);
	mSvertices[60 + 52].Position = XMFLOAT3(0.3f, 3.6f, 0.3f);
	mSvertices[60 + 53].Position = XMFLOAT3(0.3f, 3.6f, -0.3f);
	mSvertices[60 + 54].Position = XMFLOAT3(-0.3f, 3.0f, 0.3f);
	mSvertices[60 + 55].Position = XMFLOAT3(-0.3f, 3.0f, -0.3f);
	mSvertices[60 + 56].Position = XMFLOAT3(-0.3f, 3.6f, -0.3f);
	mSvertices[60 + 57].Position = XMFLOAT3(-0.3f, 3.0f, 0.3f);
	mSvertices[60 + 58].Position = XMFLOAT3(-0.3f, 3.6f, -0.3f);
	mSvertices[60 + 59].Position = XMFLOAT3(-0.3f, 3.6f, 0.3f);

	mSvertices[60*2].Position = XMFLOAT3(0.1f, -0.1f, 0.0f);
	mSvertices[60*2 + 1].Position = XMFLOAT3(-0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 2].Position = XMFLOAT3(-0.1f, -0.1f, 0.0f);
	mSvertices[60*2 + 3].Position = XMFLOAT3(0.1f, -0.1f, 0.0f);
	mSvertices[60*2 + 4].Position = XMFLOAT3(0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 5].Position = XMFLOAT3(-0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 6].Position = XMFLOAT3(0.1f, -0.1f, 0.0f);
	mSvertices[60*2 + 7].Position = XMFLOAT3(0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 8].Position = XMFLOAT3(0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 9].Position = XMFLOAT3(0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 10].Position = XMFLOAT3(0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 11].Position = XMFLOAT3(0.1f, 0.1f, 3.0f);
	mSvertices[60*2 + 12].Position = XMFLOAT3(-0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 13].Position = XMFLOAT3(-0.1f, -0.1f, 0.0f);
	mSvertices[60*2 + 14].Position = XMFLOAT3(-0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 15].Position = XMFLOAT3(-0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 16].Position = XMFLOAT3(-0.1f, -0.1f, 3.0f);
	mSvertices[60*2 + 17].Position = XMFLOAT3(-0.1f, 0.1f, 3.0f);
	mSvertices[60*2 + 18].Position = XMFLOAT3(0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 19].Position = XMFLOAT3(-0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 20].Position = XMFLOAT3(-0.1f, 0.1f, 3.0f);
	mSvertices[60*2 + 21].Position = XMFLOAT3(0.1f, 0.1f, 0.0f);
	mSvertices[60*2 + 22].Position = XMFLOAT3(-0.1f, 0.1f, 3.0f);
	mSvertices[60*2 + 23].Position = XMFLOAT3(0.1f, 0.1f, 3.0f);

	mSvertices[60*2 + 24].Position = XMFLOAT3(0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 25].Position = XMFLOAT3(-0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 26].Position = XMFLOAT3(0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 27].Position = XMFLOAT3(0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 28].Position = XMFLOAT3(-0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 29].Position = XMFLOAT3(-0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 30].Position = XMFLOAT3(-0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 31].Position = XMFLOAT3(0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 32].Position = XMFLOAT3(-0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 33].Position = XMFLOAT3(0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 34].Position = XMFLOAT3(0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 35].Position = XMFLOAT3(-0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 36].Position = XMFLOAT3(-0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 37].Position = XMFLOAT3(0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 38].Position = XMFLOAT3(-0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 39].Position = XMFLOAT3(0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 40].Position = XMFLOAT3(0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 41].Position = XMFLOAT3(-0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 42].Position = XMFLOAT3(0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 43].Position = XMFLOAT3(-0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 44].Position = XMFLOAT3(-0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 45].Position = XMFLOAT3(0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 46].Position = XMFLOAT3(-0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 47].Position = XMFLOAT3(0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 48].Position = XMFLOAT3(0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 49].Position = XMFLOAT3(0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 50].Position = XMFLOAT3(0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 51].Position = XMFLOAT3(0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 52].Position = XMFLOAT3(0.3f, 0.3f, 3.6f);
	mSvertices[60*2 + 53].Position = XMFLOAT3(0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 54].Position = XMFLOAT3(-0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 55].Position = XMFLOAT3(-0.3f, -0.3f, 3.0f);
	mSvertices[60*2 + 56].Position = XMFLOAT3(-0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 57].Position = XMFLOAT3(-0.3f, 0.3f, 3.0f);
	mSvertices[60*2 + 58].Position = XMFLOAT3(-0.3f, -0.3f, 3.6f);
	mSvertices[60*2 + 59].Position = XMFLOAT3(-0.3f, 0.3f, 3.6f);

	mSvertices[60 * 3].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 1].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 2].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 3].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 4].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 5].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);

	mSvertices[60 * 3 + 6].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 7].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 8].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 9].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 10].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 11].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);

	mSvertices[60 * 3 + 12].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 13].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 14].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);
	mSvertices[60 * 3 + 15].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 16].Position = XMFLOAT3(0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 17].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);

	mSvertices[60 * 3 + 18].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 19].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);
	mSvertices[60 * 3 + 20].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);
	mSvertices[60 * 3 + 21].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 22].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);
	mSvertices[60 * 3 + 23].Position = XMFLOAT3(0.3f, 0.3f, 0.3f);

	mSvertices[60 * 3 + 24].Position = XMFLOAT3(-0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 25].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 26].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);
	mSvertices[60 * 3 + 27].Position = XMFLOAT3(-0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 28].Position = XMFLOAT3(0.3f, 0.3f, 0.3f);
	mSvertices[60 * 3 + 29].Position = XMFLOAT3(0.3f, 0.3f, -0.3f);

	mSvertices[60 * 3 + 30].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);
	mSvertices[60 * 3 + 31].Position = XMFLOAT3(-0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 32].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 33].Position = XMFLOAT3(-0.3f, -0.3f, 0.3f);
	mSvertices[60 * 3 + 34].Position = XMFLOAT3(0.3f, -0.3f, -0.3f);
	mSvertices[60 * 3 + 35].Position = XMFLOAT3(0.3f, -0.3f, 0.3f);


	for (int i = 0; i < m_SvertexCount; i++)
	{
		mSvertices[i].Position = MathHelper::XMFLOAT3_MUL_SCALAR(mSvertices[i].Position, 30.0f);

		if (i < 60)
			mSvertices[i].Color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		else if (i < 120)
			mSvertices[i].Color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
		else if (i < 180)
			mSvertices[i].Color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		else
			mSvertices[i].Color = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	}


	// Lock the vertex buffer so it can be written to.
	if (FAILED(deviceContext->Map(m_SvertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (Vertex*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)mSvertices, (sizeof(Vertex)* m_SvertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_SvertexBuffer, 0);
	

	return true;
}


int Gizmo::GetselectedTAxis()
{
	return selectedTAxis;
}

void Gizmo::SetselectedTAxis(int para)
{
	selectedTAxis = para;
}

void Gizmo::SetPushedAxis(int para)
{
	bPushedAxis = para;
}

int Gizmo::GetPushedAxis()
{
	return bPushedAxis;
}

Object* Gizmo::GetSelectedObj()
{
	return pSelectedObject;
}

void Gizmo::SetSelectedObj(Object* pObj)
{
	pSelectedObject = pObj;
}

Actor* Gizmo::GetSelectedActor()
{
	return pSelectedActor;
}

void Gizmo::SetSelectedActor(Actor* pActor)
{
	pSelectedActor = pActor;
}
XMFLOAT3 Gizmo::Get_Axis_TO_SCREEN(XMFLOAT3 vec, XMFLOAT4X4 WorldViewProj)
{
	XMFLOAT4 Vertex = XMFLOAT4(vec.x, vec.y, vec.z, 1.0f);
	Vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(Vertex, WorldViewProj);
	Vertex = MathHelper::XMFLOAT4_DIV_W(Vertex);

	XMFLOAT4 OriVertex = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	OriVertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(OriVertex, WorldViewProj);
	OriVertex = MathHelper::XMFLOAT4_DIV_W(OriVertex);


	return MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(Vertex.x - OriVertex.x, Vertex.y - OriVertex.y, 0.0f));
}

XMFLOAT3 Gizmo::Get_PIXEL_TO_LOCAL(XMFLOAT2 ScreenPos, XMFLOAT2 ScreenSize, XMFLOAT4X4 InvWorldViewProj)
{
	XMFLOAT3 ScreenPoint = Get_PIXEL_TO_SCREEN(ScreenPos, ScreenSize);

	XMFLOAT4 Vertex = XMFLOAT4(ScreenPoint.x, ScreenPoint.y, ScreenPoint.z, 1.0f);
	Vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(Vertex, InvWorldViewProj);
	Vertex = MathHelper::XMFLOAT4_DIV_W(Vertex);

	XMFLOAT4 OriVertex = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	OriVertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(Vertex, InvWorldViewProj);
	OriVertex = MathHelper::XMFLOAT4_DIV_W(OriVertex);

	return MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(Vertex.x - OriVertex.x, Vertex.y - OriVertex.y, 0.0f));
}

XMFLOAT2 Gizmo::Get_SCREEN_TO_PIXEL(XMFLOAT2 ScreenPos, XMFLOAT2 ScreenSize)
{
	return XMFLOAT2(((ScreenPos.x + 1.0f) *0.5f)*ScreenSize.x, (1.0f - ((ScreenPos.y + 1.0f) *0.5f))*ScreenSize.y);
}

XMFLOAT3 Gizmo::Get_PIXEL_TO_SCREEN(XMFLOAT2 PixelPos, XMFLOAT2 ScreenSize)
{	
	return XMFLOAT3((PixelPos.x / ScreenSize.x) * 2.0f - 1.0f, (1.0f - (PixelPos.y / ScreenSize.y)) * 2.0f - 1.0f , 0.0f);
}

XMFLOAT3 Gizmo::Get_OBj_ORIGINPOINT_TO_SCREEN(XMFLOAT4X4 WorldViewProj)
{
	XMFLOAT4 OriVertex = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	OriVertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(OriVertex, WorldViewProj);
	OriVertex = MathHelper::XMFLOAT4_DIV_W(OriVertex);

	return XMFLOAT3(OriVertex.x, OriVertex.y, 0.0f);
}
















void Gizmo::OnResize(int ScreenWidth, int ScreenHeight)
{
	m_screenWidth = ScreenWidth;
	m_screenHeight = ScreenHeight;	
}

void Gizmo::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

bool Gizmo::Render(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTech, Effects* pEffects, XMFLOAT4X4 World, XMFLOAT4X4 WorldViewProj, FLOAT X, FLOAT Y, FLOAT L, int Flag)
{
	bool result;

	result = UpdateBuffers(deviceContext, pEffects, World, WorldViewProj, X, Y, L, Flag);
	if (!result)
	{
		return false;
	}
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	if (Flag == 0)
		RenderBuffers(deviceContext, m_vertexBuffer, m_indexBuffer, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	else if (Flag == 1)
		RenderBuffers(deviceContext, m_TvertexBuffer, m_TindexBuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	else if (Flag == 2)
		RenderBuffers(deviceContext, m_RvertexBuffer, m_RindexBuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	else if (Flag == 3)
		RenderBuffers(deviceContext, m_SvertexBuffer, m_SindexBuffer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, deviceContext);
		if (Flag == 0)
			deviceContext->DrawIndexed(GetIndexCount(), 0, 0);
		else if (Flag == 1)
			deviceContext->DrawIndexed(GetTIndexCount(), 0, 0);
		else if (Flag == 2)
			deviceContext->DrawIndexed(GetRIndexCount(), 0, 0);
		else if (Flag == 3)
			deviceContext->DrawIndexed(GetSIndexCount(), 0, 0);
	}

	return true;
}

int Gizmo::GetIndexCount()
{
	return m_indexCount;
}

int Gizmo::GetTIndexCount()
{
	return m_TindexCount;
}

int Gizmo::GetRIndexCount()
{
	return m_RindexCount;
}

int Gizmo::GetSIndexCount()
{
	return m_SindexCount;
}

bool Gizmo::InitializeBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT VertexCount, UINT IndexCount, ID3D11Buffer** VertexBuffer, ID3D11Buffer** IndexBuffer)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	UINT i;

	// Create the vertex array.
	vertices = new Vertex[VertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[IndexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(Vertex)* VertexCount));

	// Load the index array with data.
	for (i = 0; i < IndexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex)* VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}



	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, IndexBuffer);
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

bool Gizmo::InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	InitializeBuffer(device, deviceContext, m_vertexCount, m_indexCount, &m_vertexBuffer, &m_indexBuffer);
	InitializeBuffer(device, deviceContext, m_TvertexCount, m_TindexCount, &m_TvertexBuffer, &m_TindexBuffer);
	InitializeBuffer(device, deviceContext, m_RvertexCount, m_RindexCount, &m_RvertexBuffer, &m_RindexBuffer);
	InitializeBuffer(device, deviceContext, m_SvertexCount, m_SindexCount, &m_SvertexBuffer, &m_SindexBuffer);

	return true;
}


void Gizmo::ShutdownBuffers()
{

	ReleaseCOM(m_TvertexBuffer);
	ReleaseCOM(m_TindexBuffer);

	ReleaseCOM(m_RvertexBuffer);
	ReleaseCOM(m_RindexBuffer);

	ReleaseCOM(m_SvertexBuffer);
	ReleaseCOM(m_SindexBuffer);

	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}



void Gizmo::RenderBuffers(ID3D11DeviceContext* deviceContext, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, D3D11_PRIMITIVE_TOPOLOGY KIND)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(KIND);

	return;
}

XMFLOAT4X4 Gizmo::CalculateTransformMat(float X, float Y, XMFLOAT4 OriginPoint)
{
	return XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						-OriginPoint.x + X, -OriginPoint.y + Y, 0.0f, 1.0f);
}

XMFLOAT4 Gizmo::CalculateVertex(float x, float y, float z, XMFLOAT4X4 WorldViewProj, XMFLOAT4X4 TransformMat, XMFLOAT4X4 InvWorldViewProj)
{
	XMFLOAT4 Vertex = XMFLOAT4(x, y, z, 1.0f);
	Vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(Vertex, WorldViewProj);
	Vertex = MathHelper::XMFLOAT4_DIV_W(Vertex);
	Vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(Vertex, TransformMat);
	Vertex = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(Vertex, InvWorldViewProj);
	Vertex = MathHelper::XMFLOAT4_DIV_W(Vertex);
	return Vertex;
}


bool Gizmo::UpdateBuffers(ID3D11DeviceContext* deviceContext, Effects* pEffects, XMFLOAT4X4 World, XMFLOAT4X4 WorldViewProj, FLOAT X, FLOAT Y, FLOAT L, int Flag)
{	
	
	XMFLOAT4 originPOSITION;

	originPOSITION = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	originPOSITION = MathHelper::XMFLOAT4_DIV_W(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(originPOSITION, WorldViewProj));
	mTransform = CalculateTransformMat(X, Y, originPOSITION);

	if (Flag == 0)
		pEffects->GizmoDrawFx->SetAxisTechVariables(XMLoadFloat4x4(&World), XMLoadFloat4x4(&WorldViewProj), XMLoadFloat4x4(&mTransform), L);
	else if (Flag == 1)
		pEffects->GizmoDrawFx->SetTransTechVariables(XMLoadFloat4x4(&World), XMLoadFloat4x4(&WorldViewProj), XMLoadFloat4x4(&mTransform), L, selectedTAxis);
	else if (Flag == 2)
		pEffects->GizmoDrawFx->SetTransTechVariables(XMLoadFloat4x4(&World), XMLoadFloat4x4(&WorldViewProj), XMLoadFloat4x4(&mTransform), L, selectedTAxis);
	else if (Flag == 3)
		pEffects->GizmoDrawFx->SetTransTechVariables(XMLoadFloat4x4(&World), XMLoadFloat4x4(&WorldViewProj), XMLoadFloat4x4(&mTransform), L, selectedTAxis);

	return true;
}

void Gizmo::HoverT(int sx, int sy, float WorldPositionX, float WorldPositionY, XMFLOAT4X4 Proj, XMFLOAT4X4 ViewMat, XMFLOAT4X4 WorldMat)
{
	// Compute picking ray in view space.
	// -1 to 1
	float vx = (+2.0f*sx / m_screenWidth - 1.0f);
	float vy = (-2.0f*sy / m_screenHeight + 1.0f);

	vx -= WorldPositionX;
	vy -= WorldPositionY;
	// Tranform ray to local space of Mesh.
	XMMATRIX View = XMLoadFloat4x4(&ViewMat);

	int mPickedMesh = 0;

	selectedTAxis = 0;

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(vx, vy, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	//rayDir = XMVector3Normalize(rayDir);


	XMMATRIX W = XMLoadFloat4x4(&WorldMat);
	XMMATRIX P = XMLoadFloat4x4(&Proj);
	//XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);
	XMMATRIX toLocal = MathHelper::Inverse( XMMatrixMultiply(XMMatrixMultiply(W, View), P));

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	float tmin = MathHelper::Infinity;
	float dist;

	if (mGizmoStatus == 0)
	{
		for (UINT i = 0; i < 3; i++)
		{

			BoundingBox tempAABB;

			if (i == 0)
				tempAABB = mAxis_X_AlignedBox;
			else if (i == 1)
				tempAABB = mAxis_Y_AlignedBox;
			else if (i == 2)
				tempAABB = mAxis_Z_AlignedBox;

			//BoundingFrustum::Intersects(rayOrigin, rayDir, &tempAABB, &dist)
			if (tempAABB.Intersects(rayOrigin, rayDir, dist))
			{
				// Find the nearest ray/triangle intersection.
				//tmin = MathHelper::Infinity;
				for (UINT j = 14 * i; j < 14 * (i + 1); ++j)
				{
					// Indices for this triangle.
					UINT i0 = j * 3 + 0;
					UINT i1 = j * 3 + 1;
					UINT i2 = j * 3 + 2;

					// Vertices for this triangle.
					XMVECTOR v0 = XMLoadFloat3(&mTvertices[i0].Position);
					XMVECTOR v1 = XMLoadFloat3(&mTvertices[i1].Position);
					XMVECTOR v2 = XMLoadFloat3(&mTvertices[i2].Position);

					// We have to iterate over all the triangles in order to find the nearest intersection.
					float t = 0.0f;

					//XNA::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t)
					if (MathHelper::MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
					{
						if (t < tmin)
						{
							// This is the new nearest picked triangle.
							tmin = t;
							mPickedMesh = (int)i + 1;
						}
					}
				}
			}
		}
	}
	else if (mGizmoStatus == 1)
	{
		for (UINT i = 0; i < 3; i++)
		{

			BoundingBox tempAABB;

			if (i == 0)
				tempAABB = mRAxis_X_AlignedBox;
			else if (i == 1)
				tempAABB = mRAxis_Y_AlignedBox;
			else if (i == 2)
				tempAABB = mRAxis_Z_AlignedBox;


			if (tempAABB.Intersects(rayOrigin, rayDir, dist))
			{
				// Find the nearest ray/triangle intersection.
				//tmin = MathHelper::Infinity;
				for (UINT j = 8 * 16 * i; j < 8 * 16 * (i + 1); ++j)
				{
					// Indices for this triangle.
					UINT i0 = j * 3 + 0;
					UINT i1 = j * 3 + 1;
					UINT i2 = j * 3 + 2;

					// Vertices for this triangle.
					XMVECTOR v0 = XMLoadFloat3(&mRvertices[i0].Position);
					XMVECTOR v1 = XMLoadFloat3(&mRvertices[i1].Position);
					XMVECTOR v2 = XMLoadFloat3(&mRvertices[i2].Position);

					// We have to iterate over all the triangles in order to find the nearest intersection.
					float t = 0.0f;
					if (MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
					{
						if (t < tmin)
						{
							// This is the new nearest picked triangle.
							tmin = t;
							mPickedMesh = (int)i + 1;
						}
					}
				}
			}
		}
	}
	else if (mGizmoStatus == 2)
	{
		for (UINT i = 0; i < 4; i++)
		{

			BoundingBox tempAABB;

			if (i == 0)
				tempAABB = mAxis_X_AlignedBox;
			else if (i == 1)
				tempAABB = mAxis_Y_AlignedBox;
			else if (i == 2)
				tempAABB = mAxis_Z_AlignedBox;
			else if (i == 3)
				tempAABB = mMiddle_AlignedBox;

			if (tempAABB.Intersects(rayOrigin, rayDir, dist))
			{
				// Find the nearest ray/triangle intersection.
				//tmin = MathHelper::Infinity;
				if (i < 3)
				{
					for (UINT j = 20 * i; j < 20 * (i + 1); ++j)
					{
						// Indices for this triangle.
						UINT i0 = j * 3 + 0;
						UINT i1 = j * 3 + 1;
						UINT i2 = j * 3 + 2;

						// Vertices for this triangle.
						XMVECTOR v0 = XMLoadFloat3(&mSvertices[i0].Position);
						XMVECTOR v1 = XMLoadFloat3(&mSvertices[i1].Position);
						XMVECTOR v2 = XMLoadFloat3(&mSvertices[i2].Position);

						// We have to iterate over all the triangles in order to find the nearest intersection.
						float t = 0.0f;
						if (MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
						{
							if (t < tmin)
							{
								// This is the new nearest picked triangle.
								tmin = t;
								mPickedMesh = (int)i + 1;
							}
						}
					}
				}
				else if (i == 3)
				{
					for (UINT j = 20 * i; j < 20 * i + 12; ++j)
					{
						// Indices for this triangle.
						UINT i0 = j * 3 + 0;
						UINT i1 = j * 3 + 1;
						UINT i2 = j * 3 + 2;

						// Vertices for this triangle.
						XMVECTOR v0 = XMLoadFloat3(&mSvertices[i0].Position);
						XMVECTOR v1 = XMLoadFloat3(&mSvertices[i1].Position);
						XMVECTOR v2 = XMLoadFloat3(&mSvertices[i2].Position);

						// We have to iterate over all the triangles in order to find the nearest intersection.
						float t = 0.0f;
						if (MathHelper::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
						{
							if (t < tmin)
							{
								// This is the new nearest picked triangle.
								tmin = t;
								mPickedMesh = (int)i + 1;
							}
						}
					}
				}

			}
		}
	}




	selectedTAxis = mPickedMesh;
}

void Gizmo::OnMouseMove(float DeltaTime, int x, int y, int LastMousePosX, int LastMousePosY, int ClientWidth, int ClientHeight, XMFLOAT4X4* pWorld, XMFLOAT4X4* pWVP, XMFLOAT3 GizmoLookVec, UINT iKind)
{
	int PushedAxis = GetPushedAxis();	
	
	if (PushedAxis != 0)
	{

		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(5000.0f*DeltaTime*static_cast<float>(x - LastMousePosX));
		float dy = XMConvertToRadians(5000.0f*DeltaTime*static_cast<float>(y - LastMousePosY));

		float dist = sqrt(dx*dx + dy*dy)*1.0f;

		XMFLOAT3 v1 = Get_PIXEL_TO_SCREEN(XMFLOAT2((float)x, (float)y), XMFLOAT2((float)ClientWidth, (float)ClientHeight));
		XMFLOAT3 v2 = Get_PIXEL_TO_SCREEN(XMFLOAT2((float)LastMousePosX, (float)LastMousePosY), XMFLOAT2((float)ClientWidth, (float)ClientHeight));

		//ScreenSpace, Not Pixel
		XMFLOAT3 vDragged = MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(v1.x - v2.x, v1.y - v2.y, 0.0f));

		XMFLOAT3 vec4 = Get_PIXEL_TO_SCREEN(XMFLOAT2((float)LastMousePosX, (float)LastMousePosY), XMFLOAT2((float)ClientWidth, (float)ClientHeight));
		XMFLOAT3 vRadius = Get_OBj_ORIGINPOINT_TO_SCREEN(*pWVP);

		vRadius.x += m_SelectedObjScreenPos.x;
		vRadius.y += m_SelectedObjScreenPos.y;

		vec4 = MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(vec4.x - vRadius.x, vec4.y - vRadius.y, 0.0f));
		vRadius = XMFLOAT3(vec4.y, -vec4.x, 0.0f);

		GizmoLookVec = XMFLOAT3(-GizmoLookVec.x, -GizmoLookVec.y, -GizmoLookVec.z);

		if (PushedAxis == 1)
		{
			PushedAxisProcessing(vDragged, XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), vRadius, GizmoLookVec, dist, pWorld, pWVP, iKind);
		}
		else if (PushedAxis == 2)
		{
			PushedAxisProcessing(vDragged, XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), vRadius, GizmoLookVec, dist, pWorld, pWVP, iKind);
		}
		else if (PushedAxis == 3)
		{
			PushedAxisProcessing(vDragged, XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), vRadius, GizmoLookVec, dist, pWorld, pWVP, iKind);
		}
		else if (PushedAxis == 4)
		{
			XMFLOAT3 vec2 = Get_Axis_TO_SCREEN(XMFLOAT3(0.0f, 0.0f, 0.0f), *pWVP);

			vec2.x += m_SelectedObjScreenPos.x;
			vec2.y += m_SelectedObjScreenPos.y;

			XMFLOAT2 Pixel = Get_SCREEN_TO_PIXEL(XMFLOAT2(vec2.x, vec2.y), XMFLOAT2((float)ClientWidth, (float)ClientHeight));
			

			float delta1 = x - Pixel.x;
			float delta2 = y - Pixel.y;
			float dist3 = sqrt(delta1*delta1 + delta2*delta2);

			delta1 = LastMousePosX - Pixel.x;
			delta2 = LastMousePosY - Pixel.y;

			float dist2 = sqrt(delta1*delta1 + delta2*delta2);

			dist = dist*0.05f;

			if (iKind == 0)
			{
				if (dist3 < dist2)
					GetSelectedObj()->Scale(1.0f - dist, 1.0f - dist, 1.0f - dist);
				else
					GetSelectedObj()->Scale(1.0f + dist, 1.0f + dist, 1.0f + dist);
			}
			else if (iKind == 1)
			{
				if (dist3 < dist2)
					GetSelectedActor()->Scale(1.0f - dist, 1.0f - dist, 1.0f - dist);
				else
					GetSelectedActor()->Scale(1.0f + dist, 1.0f + dist, 1.0f + dist);
			}

			
		}
	}
}

void Gizmo::PushedAxisProcessing(XMFLOAT3 vDragged, XMFLOAT3 TransStandardAxis, XMFLOAT3 RotStandardAxis, XMFLOAT3 vRadius, XMFLOAT3 GizmoLookVec, float dist, XMFLOAT4X4* pWorld, XMFLOAT4X4* pWVP, UINT iKind)
{
	XMFLOAT3 TransStandardAxisforUse = MathHelper::XMFLOAT3_MUL_SCALAR(TransStandardAxis, dist);

	TransStandardAxis = Get_Axis_TO_SCREEN(TransStandardAxis, *pWVP);
	
	if (iKind == 0)
	{
		if (mGizmoStatus == 0)
		{
			if (MathHelper::XMFLOAT3_DOT(vDragged, TransStandardAxis) < 0)
				GetSelectedObj()->Translation(-TransStandardAxisforUse.x, -TransStandardAxisforUse.y, -TransStandardAxisforUse.z);
			else
				GetSelectedObj()->Translation(TransStandardAxisforUse.x, TransStandardAxisforUse.y, TransStandardAxisforUse.z);
		}
		else if (mGizmoStatus == 1)
		{
			RotStandardAxis = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(RotStandardAxis, 1.0f), *pWorld));

			if (MathHelper::XMFLOAT3_DOT(RotStandardAxis, GizmoLookVec) > 0)
			{
				if (MathHelper::XMFLOAT3_DOT(vDragged, vRadius) < 0)
					GetSelectedObj()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), -dist);
				else
					GetSelectedObj()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), dist);
			}
			else
			{
				if (MathHelper::XMFLOAT3_DOT(vDragged, vRadius) < 0)
					GetSelectedObj()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), dist);
				else
					GetSelectedObj()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), -dist);
			}
		}
		else if (mGizmoStatus == 2)
		{
			TransStandardAxisforUse = MathHelper::XMFLOAT3_MUL_SCALAR(TransStandardAxisforUse, 0.05f);

			if (MathHelper::XMFLOAT3_DOT(vDragged, TransStandardAxis) < 0)
				GetSelectedObj()->Scale(1.0f - TransStandardAxisforUse.x, 1.0f - TransStandardAxisforUse.y, 1.0f - TransStandardAxisforUse.z);
			else
				GetSelectedObj()->Scale(1.0f + TransStandardAxisforUse.x, 1.0f + TransStandardAxisforUse.y, 1.0f + TransStandardAxisforUse.z);
		}
	}
	else if (iKind == 1)
	{
		if (mGizmoStatus == 0)
		{
			if (MathHelper::XMFLOAT3_DOT(vDragged, TransStandardAxis) < 0)
				GetSelectedActor()->Translation(-TransStandardAxisforUse.x, -TransStandardAxisforUse.y, -TransStandardAxisforUse.z);
			else
				GetSelectedActor()->Translation(TransStandardAxisforUse.x, TransStandardAxisforUse.y, TransStandardAxisforUse.z);
		}
		else if (mGizmoStatus == 1)
		{
			RotStandardAxis = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(RotStandardAxis, 1.0f), *pWorld));

			if (MathHelper::XMFLOAT3_DOT(RotStandardAxis, GizmoLookVec) > 0)
			{
				if (MathHelper::XMFLOAT3_DOT(vDragged, vRadius) < 0)
					GetSelectedActor()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), -dist);
				else
					GetSelectedActor()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), dist);
			}
			else
			{
				if (MathHelper::XMFLOAT3_DOT(vDragged, vRadius) < 0)
					GetSelectedActor()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), dist);
				else
					GetSelectedActor()->Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(RotStandardAxis, 1.0f), -dist);
			}
		}
		else if (mGizmoStatus == 2)
		{
			TransStandardAxisforUse = MathHelper::XMFLOAT3_MUL_SCALAR(TransStandardAxisforUse, 0.05f);

			if (MathHelper::XMFLOAT3_DOT(vDragged, TransStandardAxis) < 0)
				GetSelectedActor()->Scale(1.0f - TransStandardAxisforUse.x, 1.0f - TransStandardAxisforUse.y, 1.0f - TransStandardAxisforUse.z);
			else
				GetSelectedActor()->Scale(1.0f + TransStandardAxisforUse.x, 1.0f + TransStandardAxisforUse.y, 1.0f + TransStandardAxisforUse.z);
		}
	}

	
}