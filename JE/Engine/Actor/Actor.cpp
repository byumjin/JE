#include "Actor.h"

Actor::Actor()
{
	mVB = NULL;
	mIB = NULL;

	mWorldPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mRotate = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_WorldMatrix, I);
	
	m_VertexCount = 4;
	m_IndexCount = 6;

}

Actor::~Actor()
{
	delete[] m_Vertex;
	delete[] m_Indices;

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);

	ReleaseCOM(m_VertexBufferforActorGizmo);
	ReleaseCOM(m_IndexBufferforActorGizmo);
}

XMFLOAT3 Actor::GetWorldPosition()
{
	return mWorldPosition;
}

XMFLOAT3 Actor::GetOriginPointPos()
{
	return mWorldPosition;
}

void Actor::SetWorldPosition(XMFLOAT3 val)
{
	mWorldPosition = val;
}

XMFLOAT3 Actor::GetScale()
{
	return mScale;
}

void Actor::SetScale(XMFLOAT3 val)
{
	mScale = val;
}

XMFLOAT4 Actor::GetRotate()
{
	return mRotate;
}

void Actor::SetRotate(XMFLOAT4 val)
{
	mRotate = val;
}

XMFLOAT4X4 Actor::GetWorldMatrix()
{
	return m_WorldMatrix;
}

void Actor::SetWorldMatrix(XMFLOAT4X4 val)
{
	m_WorldMatrix = val;
}

void Actor::Translation(float x, float y, float z)
{
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&m_WorldMatrix));

	XMFLOAT4X4 RotMat;

	XMStoreFloat4x4(&RotMat, XMMatrixRotationQuaternion(rotQuat));
	XMFLOAT4 Dir = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT3(x, y, z), RotMat);
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), XMMatrixTranslation(Dir.x, Dir.y, Dir.z)));

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&m_WorldMatrix));

	mWorldPosition = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);

}
void Actor::Scale(float x, float y, float z)
{

	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&m_WorldMatrix));

	XMFLOAT3 Pos = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);
	XMFLOAT3 Scal = MathHelper::XMVECTOR_TO_XMFLOAT3(scale);

	//TO DO
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMMatrixIdentity(), XMMatrixScaling(Scal.x*x, Scal.y*y, Scal.z*z)));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), XMMatrixRotationQuaternion(rotQuat)));
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), XMMatrixTranslation(Pos.x, Pos.y, Pos.z)));

	mScale = XMFLOAT3(Scal.x*x, Scal.y*y, Scal.z*z);
}
void Actor::Rotation(FXMVECTOR axis, float angle)
{
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&m_WorldMatrix));
	rotQuat = XMQuaternionNormalize(rotQuat);

	XMFLOAT3 Pos = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);
	XMFLOAT3 S = MathHelper::XMVECTOR_TO_XMFLOAT3(scale);
	
	

	//TO DO
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMMatrixIdentity(), XMMatrixScaling(S.x, S.y, S.z)));

	// Get the rotation quaternion from the matrix.
	XMVECTOR Orientation = XMQuaternionRotationMatrix(XMMatrixRotationAxis(axis, MathHelper::ConvertToRadians(angle)));
	// Make sure it is normal (in case the vectors are slightly non-orthogonal).
	Orientation = XMQuaternionNormalize(Orientation);

	//Original rotation
	XMMATRIX R = XMMatrixRotationQuaternion(Orientation);

	//Second rotation
	R = XMMatrixMultiply(R, XMMatrixRotationQuaternion(rotQuat));
	// Rebuild the rotation matrix from the quaternion.
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), R));

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), XMMatrixTranslation(Pos.x, Pos.y, Pos.z)));

	mRotate = MathHelper::XMVECTOR_TO_XMFLOAT4(XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_WorldMatrix)));

}

void Actor::BuildBuffers(ID3D11Device *pd3dDevice)
{	
	m_Vertex = new Vertex[m_VertexCount];
	m_Vertex[0].TexCoord = (XMFLOAT2(0.0f, 0.0f));
	m_Vertex[1].TexCoord = (XMFLOAT2(1.0f, 1.0f));
	m_Vertex[2].TexCoord = (XMFLOAT2(0.0f, 1.0f));
	m_Vertex[3].TexCoord = (XMFLOAT2(1.0f, 0.0f));


	m_Vertex[0].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_Vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	m_Vertex[2].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_Vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);
	//m_Vertex[0].Position = GetWorldPosition();
	//m_Vertex[1].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//m_Vertex[2].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//m_Vertex[3].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	BoundingBox::CreateFromPoints(m_AxisAlignedBox, m_VertexCount, &m_Vertex[0].Position, sizeof(Vertex));

	m_Indices = new UINT[m_IndexCount];
	m_Indices[0] = 0;
	m_Indices[1] = 1;
	m_Indices[2] = 2;
	m_Indices[3] = 0;
	m_Indices[4] = 3;
	m_Indices[5] = 1;

	BuildVertexBuffers(pd3dDevice, &m_Vertex[0], D3D11_USAGE_DYNAMIC, m_VertexCount, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, &mVB);
	BuildIndexBuffers(pd3dDevice, &m_Indices[0], D3D11_USAGE_IMMUTABLE, m_IndexCount, D3D11_BIND_INDEX_BUFFER, 0, 0, &mIB);


}

BoundingBox Actor::GetAABB()
{
	return m_AxisAlignedBox;
}

void Actor::CalculateRotaionMatrix(XMFLOAT3 CameraPos, XMFLOAT4X4 ViewMat)
{
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&ViewMat));
	//XMFLOAT3 Pos = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);

	XMMATRIX R = XMMatrixRotationQuaternion(rotQuat);
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMMatrixIdentity(), R));
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), XMMatrixTranslation(mWorldPosition.x, mWorldPosition.y, mWorldPosition.z)));
}

void Actor::UpdateBuffer(ID3D11DeviceContext* deviceContext, XMFLOAT3 EyeWorldPos, XMFLOAT3 LookVec)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Vertex* verticesPtr;

	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	//XMFLOAT3 look = XMFLOAT3( EyeWorldPos.x - GetWorldPosition().x, EyeWorldPos.y - GetWorldPosition().y, EyeWorldPos.z - GetWorldPosition().z);
	//XMFLOAT3 look = EyeWorldPos - GetWorldPosition();

	XMFLOAT3 look = LookVec;

	//XMFLOAT4X4 InvLookMatrix;
	//XMStoreFloat4x4(&InvLookMatrix, MathHelper::Inverse(m_LookMat));
	//XMFLOAT4 lookW = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(look.x, look.y, look.z, 1.0f), InvLookMatrix);


	//look.y = 0.0f; // y-axis aligned, so project to xz-plane
	look = MathHelper::XMFLOAT3_NORMALIZE(look);
	XMFLOAT3 right = MathHelper::XMFLOAT3_NORMALIZE(MathHelper::XMFLOAT3_CROSS(look, up));
	up = MathHelper::XMFLOAT3_NORMALIZE(MathHelper::XMFLOAT3_CROSS(right, look));
	/*
	XMFLOAT3 inverseEyePos = XMFLOAT3(-EyeWorldPos.x, -EyeWorldPos.x, -EyeWorldPos.z);

	XMFLOAT4X4 temp = { right.x, up.x, look.x, 0.0f,
		right.y, up.y, look.y, 0.0f,
		right.z, up.z, look.z, 0.0f,
		//MathHelper::XMFLOAT3_DOT(right,inverseEyePos),
		//MathHelper::XMFLOAT3_DOT(up,inverseEyePos),
		//MathHelper::XMFLOAT3_DOT(look,inverseEyePos),
		-1.0f*MathHelper::XMFLOAT3_DOT(right,EyeWorldPos),
		-1.0f*MathHelper::XMFLOAT3_DOT(up,EyeWorldPos),
		-1.0f*MathHelper::XMFLOAT3_DOT(look,EyeWorldPos),
		1.0f
	};


	m_WorldMat = XMLoadFloat4x4(&m_WorldMatrix);
	m_LookMat = XMLoadFloat4x4(&temp);
	//m_LookMat = XMMatrixLookAtLH(MathHelper::XMFLOAT3_TO_XMVECTOR(EyeWorldPos, 1.0f), MathHelper::XMFLOAT3_TO_XMVECTOR(GetWorldPosition(), 1.0f), MathHelper::XMFLOAT3_TO_XMVECTOR(XMFLOAT3(0.0f,1.0f,0.0f), 1.0f));
	m_ProjMat = XMLoadFloat4x4(&ProjMat);

	*/

	/*
	m_Vertex[0].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_Vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	m_Vertex[2].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_Vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	
	for (UINT i = 0; i < m_VertexCount; i++)
	{
		m_Vertex[i].Position = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(m_Vertex[i].Position, 1.0f), WorldMat));
	}
	*/
	
	Vertex CenterVertex;
	CenterVertex.Position = GetWorldPosition();
		
	//	MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(MathHelper::XMFLOAT3_TO_XMFLOAT4(XMFLOAT3(0.0f,0.0f,0.0f) , 1.0f), m_WorldMatrix));

	FLOAT halfWidth = 1.0f;
	FLOAT halfHeight = 1.0f;

	m_Vertex[0].Position = CenterVertex.Position + (halfWidth * right) + (halfHeight * up);
	m_Vertex[1].Position = CenterVertex.Position - (halfWidth * right) - (halfHeight * up);
	m_Vertex[2].Position = CenterVertex.Position + (halfWidth * right) - (halfHeight * up);
	m_Vertex[3].Position = CenterVertex.Position - (halfWidth * right) + (halfHeight * up);
	

	BoundingBox::CreateFromPoints(m_AxisAlignedBox, m_VertexCount, &m_Vertex[0].Position, sizeof(Vertex));
	
	// Lock the vertex buffer so it can be written to.
	
	HR(deviceContext->Map(mVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	verticesPtr = (Vertex*)mappedResource.pData;
	memcpy(verticesPtr, (void*)m_Vertex, (sizeof(Vertex)* m_VertexCount));
	deviceContext->Unmap(mVB, 0);
	
	
}



void Actor::BuildVertexBuffers(ID3D11Device *pd3dDevice, Vertex *pvertices, D3D11_USAGE USAGE, UINT totalVertexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS, ID3D11Buffer** pVertexBuffer)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = USAGE;
	vbd.ByteWidth = sizeof(Vertex)* totalVertexCount;
	vbd.BindFlags = BINDFALGS;
	vbd.CPUAccessFlags = CPUACCESSFLAGS;
	vbd.MiscFlags = MISCFLAGS;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = pvertices;
	HR(pd3dDevice->CreateBuffer(&vbd, &vinitData, pVertexBuffer));
}

void Actor::BuildIndexBuffers(ID3D11Device *pd3dDevice, UINT *pindices, D3D11_USAGE USAGE, UINT totalIndexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS, ID3D11Buffer** pIndexBuffer)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = USAGE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = BINDFALGS;
	ibd.CPUAccessFlags = CPUACCESSFLAGS;
	ibd.MiscFlags = MISCFLAGS;
	D3D11_SUBRESOURCE_DATA iinitData;
	

	// Give the subresource structure a pointer to the index data.
	iinitData.pSysMem = pindices;
	iinitData.SysMemPitch = 0;
	iinitData.SysMemSlicePitch = 0;


	HR(pd3dDevice->CreateBuffer(&ibd, &iinitData, pIndexBuffer));
}

void Actor::Render(ID3D11DeviceContext* pd3dImmediateContext, XMFLOAT3 EyeWorldPos, XMFLOAT3 LookVec, Effects* pEffects, XMFLOAT4X4 ViewMat, XMFLOAT4X4 ProjMat, ID3DX11EffectTechnique* pTech,
	D3DX11_TECHNIQUE_DESC &techDesc, ID3D11ShaderResourceView* pShaderResourceView)
{
	

	
	//m_WorldMat = XMLoadFloat4x4(&m_WorldMatrix);
	
	m_WorldMat = XMMatrixIdentity();
	m_LookMat = XMLoadFloat4x4(&ViewMat);
	m_ProjMat = XMLoadFloat4x4(&ProjMat);
	//XMMATRIX MVP = XMMatrixMultiply(XMMatrixMultiply(m_WorldMat, m_LookMat), m_ProjMat);
	XMMATRIX worldViewProj = m_WorldMat* m_LookMat * m_ProjMat;

	UpdateBuffer(pd3dImmediateContext, EyeWorldPos, LookVec);

	pEffects->UISpriteFx->SetAllVariables(XMLoadFloat4x4(&GetWorldMatrix()), m_LookMat*m_ProjMat, worldViewProj, EyeWorldPos, bSelected, pShaderResourceView);


	DrawUISprite(pd3dImmediateContext, pTech, techDesc);
}


void Actor::DrawUISprite(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	pd3dImmediateContext->IASetInputLayout(InputLayouts::BRDFL_VID);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(m_IndexCount, 0, 0);
	}
}

void Actor::DrawActorGizmo(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	pd3dImmediateContext->IASetInputLayout(InputLayouts::BRDFL_VID);
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pd3dImmediateContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(m_IndexCount, 0, 0);
	}
}
