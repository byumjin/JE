#include "Object.h"

Object::Object() : bRotate(false)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);	

	uSmoothingGroupMax = 0;

	mEffectKind = NULL;

	bSelected = false;

	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	mRotate = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mX_Axis = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mY_Axis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mZ_Axis = XMFLOAT3(0.0f, 0.0f, 1.0f);
}

void Object::Shutdown()
{
	mMeshData.Vertices.clear();
	std::vector<Vertex> vVertexClear;
	mMeshData.Vertices.swap(vVertexClear);


	mMeshData.Indices.clear();
	std::vector<UINT> vIndexClear;
	mMeshData.Indices.swap(vIndexClear);


	std::vector<std::vector<UINT>>::iterator OI;
	UINT count = 0;

	for (count = 0, OI = OriginalIndexArray.begin(); count < OriginalIndexArray.size(); count++, OI++)
	{
		OI->clear();;
		std::vector<UINT> vOIClearInner;
		OI->swap(vOIClearInner);
	}

	OriginalIndexArray.clear();
	std::vector<std::vector<UINT>> vOIClear;
	OriginalIndexArray.swap(vOIClear);

	if (mEffectArray)
	{
		delete[] mEffectArray;
		mEffectArray = NULL;
	}

	if (mEffectKind)
	{
		delete[] mEffectKind;
		mEffectKind = NULL;
	}
	
}

Object::~Object()
{
	Shutdown();	
}

void Object::GetObjectInfo(ObjectInfo &Info)
{
	Info.m_LayerCount = GetLayerCount();

	for (UINT i = 0; i < Info.m_LayerCount; i++)
	{
		WCHAR* tempWcharforMat = ConverCtoWC(pGeo->m_strPath);

		wcscpy_s(Info.m_GeometryPath, tempWcharforMat);
		delete[] tempWcharforMat;

		//now only for basic material
		for (UINT i = 0; i < 1; i++)
		{
			wcscpy_s(Info.m_MaterialPath[i], pMI->m_strTitle);
		}

		Info.m_Position = mPosition;
		Info.m_World = mWorld;
		Info.m_Scale = mScale;
		Info.m_Rotate = mRotate;
	}
}

void Object::SetObjectInfo(ObjectInfo &Info)
{
	//strcpy_s(m_strPath, Info.m_strPath);
	SetLayerCount(Info.m_LayerCount);

	for (UINT i = 0; i < Info.m_LayerCount; i++)
	{
		mPosition = Info.m_Position;
		mWorld = Info.m_World;
		mScale = Info.m_Scale;
		mRotate = Info.m_Rotate;
	}
}

Object* Object::GetThisObject()
{
	return this;
}

void Object::SetLayerCount(UINT val)
{
	pGeo->SetLayerCount(val);
}

UINT Object::GetLayerCount()
{
	return pGeo->GetLayerCount();
}

MeshData* Object::GetMeshData()
{
	return &mMeshData;
}

void Object::SetMeshData(MeshData paraMeshdata)
{
	mMeshData = paraMeshdata;
}

void Object::SetMaterials(UINT index, Effect* pEffect, UINT iKind)
{
	if (pGeo->mLayerCount > index)
	{
		mEffectArray[index] = pEffect;
		mEffectKind[index] = iKind;
	}	
}

void Object::CreateMaterials(UINT uSize)
{
	//Deep Copy
	mEffectArray = new Effect*[uSize];
	mEffectKind = new UINT[uSize];	
}

Effect* Object::GetMaterials(UINT index)
{
	return mEffectArray[index];
}

UINT Object::GetiKind(UINT index)
{
	return mEffectKind[index];
}

BoundingBox Object::GetAABB()
{
	return pGeo->mAxisAlignedBox;
	//return mAxisAlignedBox;	
}

void Object::CalculateTriangleArray(long vertexCount, const XMFLOAT3 *vertex,
	const XMFLOAT2 *texcoord, long triangleCount, const Triangle *triangle)
{
	XMFLOAT3 *tan1 = new XMFLOAT3[vertexCount * 3];
	XMFLOAT3 *tan2 = tan1 + vertexCount;
	XMFLOAT3 *tan3 = tan2 + vertexCount;
	ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3)* 3);

	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const XMFLOAT3& p0 = vertex[i1];
		const XMFLOAT3& p1 = vertex[i2];
		const XMFLOAT3& p2 = vertex[i3];

		const XMFLOAT2& w0 = texcoord[i1];
		const XMFLOAT2& w1 = texcoord[i2];
		const XMFLOAT2& w2 = texcoord[i3];		

		float e0x = p1.x - p0.x;
		float e1x = p2.x - p0.x;

		float e0y = p1.y - p0.y;
		float e1y = p2.y - p0.y;

		float e0z = p1.z - p0.z;
		float e1z = p2.z - p0.z;

		float u0 = w1.x - w0.x;
		float u1 = w2.x - w0.x;

		float v0 = w1.y - w0.y;
		float v1 = w2.y - w0.y;

		float dino = u0 * v1 - v0 * u1;

		if (dino != 0.0f)
		{
			float r = 1.0f / (dino);
			XMFLOAT3 TANGENT((v1 * e0x - v0 * e1x) * r,
							 (v1 * e0y - v0 * e1y) * r,
				     		 (v1 * e0z - v0 * e1z) * r);
			XMFLOAT3 BINORMAL((u0 * e1x - u1 * e0x) * r,
							  (u0 * e1y - u1 * e0y) * r,
							  (u0 * e1z - u1 * e0z) * r);
			

			TANGENT = MathHelper::XMFLOAT3_NORMALIZE(TANGENT);
			BINORMAL = MathHelper::XMFLOAT3_NORMALIZE(BINORMAL);

			XMFLOAT3 NORMAL;
						
			// Calculate handedness
			XMFLOAT3 fFaceNormal;
			fFaceNormal = MathHelper::XMFLOAT3_CROSS(MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(e0x, e0y, e0z)), MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(e1x, e1y, e1z)));

			fFaceNormal = MathHelper::XMFLOAT3_NORMALIZE(fFaceNormal);
			
			//U flip
			if (MathHelper::XMFLOAT3_DOT(MathHelper::XMFLOAT3_CROSS(TANGENT, BINORMAL), fFaceNormal) < 0.0f)
			{
				TANGENT = XMFLOAT3(-TANGENT.x, -TANGENT.y, -TANGENT.z);
			}

			NORMAL = MathHelper::XMFLOAT3_CROSS(TANGENT, BINORMAL);

			//TANGENT
			tan1[i1].x += TANGENT.x;
			tan1[i1].y += TANGENT.y;
			tan1[i1].z += TANGENT.z;

			tan1[i2].x += TANGENT.x;
			tan1[i2].y += TANGENT.y;
			tan1[i2].z += TANGENT.z;

			tan1[i3].x += TANGENT.x;
			tan1[i3].y += TANGENT.y;
			tan1[i3].z += TANGENT.z;

			//BINORMAL
			tan2[i1].x += BINORMAL.x;
			tan2[i1].y += BINORMAL.y;
			tan2[i1].z += BINORMAL.z;

			tan2[i2].x += BINORMAL.x;
			tan2[i2].y += BINORMAL.y;
			tan2[i2].z += BINORMAL.z;

			tan2[i3].x += BINORMAL.x;
			tan2[i3].y += BINORMAL.y;
			tan2[i3].z += BINORMAL.z;

			//NORMAL
			tan3[i1].x += NORMAL.x;
			tan3[i1].y += NORMAL.y;
			tan3[i1].z += NORMAL.z;

			tan3[i2].x += NORMAL.x;
			tan3[i2].y += NORMAL.y;
			tan3[i2].z += NORMAL.z;

			tan3[i3].x += NORMAL.x;
			tan3[i3].y += NORMAL.y;
			tan3[i3].z += NORMAL.z;
		}
		triangle++;
	}

	//Soomthing
	for (UINT k = 0; k < uSmoothingGroupMax + 1; k++)
	{
		for (UINT i = 0; i < OriginalIndexArray.size(); i++)
		{
			XMFLOAT3 TotalTANGENT = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMFLOAT3 TotalBINORMAL = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMFLOAT3 TotalNORMAL = XMFLOAT3(0.0f, 0.0f, 0.0f);

			//check, if a vertex has same smoothing group 
			for (UINT j = 0; j < OriginalIndexArray.at(i).size(); j++)
			{
				if (GetMeshData()->Vertices[OriginalIndexArray.at(i).at(j)].SmoothingID == k)
				{
					TotalTANGENT.x += tan1[OriginalIndexArray.at(i).at(j)].x;
					TotalTANGENT.y += tan1[OriginalIndexArray.at(i).at(j)].y;
					TotalTANGENT.z += tan1[OriginalIndexArray.at(i).at(j)].z;

					TotalBINORMAL.x += tan2[OriginalIndexArray.at(i).at(j)].x;
					TotalBINORMAL.y += tan2[OriginalIndexArray.at(i).at(j)].y;
					TotalBINORMAL.z += tan2[OriginalIndexArray.at(i).at(j)].z;

					TotalNORMAL.x += tan3[OriginalIndexArray.at(i).at(j)].x;
					TotalNORMAL.y += tan3[OriginalIndexArray.at(i).at(j)].y;
					TotalNORMAL.z += tan3[OriginalIndexArray.at(i).at(j)].z;
				}
			}

			for (UINT j = 0; j < OriginalIndexArray.at(i).size(); j++)
			{
				if (GetMeshData()->Vertices[OriginalIndexArray.at(i).at(j)].SmoothingID == k)
				{
					tan1[OriginalIndexArray.at(i).at(j)] = TotalTANGENT;
					tan2[OriginalIndexArray.at(i).at(j)] = TotalBINORMAL;
					tan3[OriginalIndexArray.at(i).at(j)] = TotalNORMAL;
				}
			}
		}
	}
	

	for (long a = 0; a < vertexCount; a++)
	{
		XMVECTOR t = MathHelper::XMFLOAT3_TO_XMVECTOR(tan1[a], 1.0f);
		t = XMVector3Normalize(t);

		XMVECTOR n = MathHelper::XMFLOAT3_TO_XMVECTOR(tan3[a], 1.0f);
		n = XMVector3Normalize(n);

		XMVECTOR b = MathHelper::XMFLOAT3_TO_XMVECTOR(tan2[a], 1.0f);
		b = XMVector3Normalize(b);

		// Gram-Schmidt orthogonalize
		t = XMVector3Normalize(t - XMVector3Dot(t, n)*n);
		b = XMVector3Cross(n, t);
				
		XMFLOAT3 ftempTangent = MathHelper::XMVECTOR_TO_XMFLOAT3(t);				
		mMeshData.Vertices.at(a).TangentU = ftempTangent;

		XMFLOAT3 ftempBi = MathHelper::XMVECTOR_TO_XMFLOAT3(b);	
		mMeshData.Vertices.at(a).BiNormalV = ftempBi;

		XMFLOAT3 ftempNormal = MathHelper::XMVECTOR_TO_XMFLOAT3(n);
		mMeshData.Vertices.at(a).Normal = ftempNormal;		
	}

	delete[] tan1;
	tan1 = NULL;

}

XMFLOAT4X4 Object::GetWorldMatrix()
{
	return mWorld;
}

XMFLOAT3 Object::GetOriginPointPos()
{
	return mPosition;
	//return XMFLOAT3(mWorld._11, mWorld._22, mWorld._33);
}

XMFLOAT3 Object::GetScale()
{
	return mScale;
}

XMFLOAT3 Object::GetReciprocalScale()
{
	return XMFLOAT3(1.0f / mScale.x, 1.0f / mScale.y, 1.0f / mScale.z);
}

XMFLOAT4 Object::GetRotate()
{
	return mRotate;
}
XMFLOAT3 Object::GetX_Axis()
{
	return mX_Axis;
}
XMFLOAT3 Object::GetY_Axis()
{
	return mY_Axis;
}
XMFLOAT3 Object::GetZ_Axis()
{
	return mZ_Axis;
}

void Object::SetWorldMatrix(CXMMATRIX WorldMat)
{
	XMStoreFloat4x4(&mWorld, WorldMat);
}

void Object::Translation(float x, float y, float z)
{
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&mWorld));
	
	XMFLOAT4X4 RotMat;
	rotQuat = XMQuaternionNormalize(rotQuat);
	XMStoreFloat4x4(&RotMat, XMMatrixRotationQuaternion(rotQuat));
	XMFLOAT4 Dir = MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT3(x, y, z), RotMat);
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMLoadFloat4x4(&mWorld), XMMatrixTranslation(Dir.x, Dir.y, Dir.z)));

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&mWorld));
	
	mPosition = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);

}
void Object::Scale(float x, float y, float z)
{

	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&mWorld));

	XMFLOAT3 Pos = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);
	//XMFLOAT3 Scal = MathHelper::XMVECTOR_TO_XMFLOAT3(scale);
	XMFLOAT3 Scal = mScale;
		
	Scal.x *= x;
	Scal.y *= y;
	Scal.z *= z;

	if (Scal.x < 0.01f)
		Scal.x = 0.01f;

	if (Scal.y < 0.01f)
		Scal.y = 0.01f;

	if (Scal.z < 0.01f)
		Scal.z = 0.01f;

	//TO DO
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMMatrixIdentity(), XMMatrixScaling(Scal.x, Scal.y, Scal.z)));
	
	rotQuat = XMQuaternionNormalize(rotQuat);
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMLoadFloat4x4(&mWorld), XMMatrixRotationQuaternion(rotQuat)));
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMLoadFloat4x4(&mWorld), XMMatrixTranslation(Pos.x, Pos.y, Pos.z)));	

	mScale = XMFLOAT3(Scal.x, Scal.y, Scal.z);
}
void Object::Rotation(FXMVECTOR axis, float angle)
{
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&mWorld));

	XMFLOAT3 Pos = MathHelper::XMVECTOR_TO_XMFLOAT3(translation);	
	XMFLOAT3 S = MathHelper::XMVECTOR_TO_XMFLOAT3(scale);

	
	//TO DO
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMMatrixIdentity(), XMMatrixScaling(S.x, S.y, S.z)));
	
	// Get the rotation quaternion from the matrix.
	XMVECTOR Orientation = XMQuaternionRotationMatrix(XMMatrixRotationAxis(axis, MathHelper::ConvertToRadians(angle)));
	// Make sure it is normal (in case the vectors are slightly non-orthogonal).
	Orientation = XMQuaternionNormalize(Orientation);
	
	//Original rotation
	XMMATRIX R = XMMatrixRotationQuaternion(Orientation);

	rotQuat = XMQuaternionNormalize(rotQuat);

	//Second rotation
	R = XMMatrixMultiply(R, XMMatrixRotationQuaternion(rotQuat));
	// Rebuild the rotation matrix from the quaternion.
	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMLoadFloat4x4(&mWorld), R));

	XMStoreFloat4x4(&mWorld, XMMatrixMultiply(XMLoadFloat4x4(&mWorld), XMMatrixTranslation(Pos.x, Pos.y, Pos.z)));
	
	mRotate = MathHelper::XMVECTOR_TO_XMFLOAT4(XMQuaternionRotationMatrix(XMLoadFloat4x4(&mWorld)));

}

void Object::DrawBoundingBox(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	pd3dImmediateContext->IASetVertexBuffers(0, 1, &pGeo->mVBforBB, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(pGeo->mIBforBB, DXGI_FORMAT_R32_UINT, 0);

	//D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(24, 0, 0);
	}
}

void Object::Draw(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	pd3dImmediateContext->IASetVertexBuffers(0, 1, &pGeo->mVB, &stride, &offset);
	pd3dImmediateContext->IASetIndexBuffer(pGeo->mIB, DXGI_FORMAT_R32_UINT, 0);

	pTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		pTech->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
		pd3dImmediateContext->DrawIndexed(pGeo->GetIndexCount(), 0, 0);
	}
}


ObjectManager::ObjectManager()
{

}

ObjectManager::~ObjectManager()
{
	std::list<Object*>::iterator li;

	for (li = mObj_List.begin(); li != mObj_List.end(); li++)
	{
		//CFbx* pFbx = dynamic_cast<CFbx*>(*li);
		//if (pFbx)
		//{
		//	delete pFbx;
		//}
	}
	mObj_List.clear();
}

UIObjectManager::UIObjectManager()
{

}

UIObjectManager::~UIObjectManager()
{
	for (UINT i = 0; i < mUIObj_List.size(); i++)
	{
		//CFbx* pFbx = dynamic_cast<CFbx*>(mUIObj_List.at(i));
		//if (pFbx)
		//{
		//	delete pFbx;
		//}
	}

	mUIObj_List.clear();
}