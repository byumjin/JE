#include "DirectionalLightActor.h"

DirectionalLightActor::DirectionalLightActor(ID3D11Device *pd3dDevice) : /*mPosition(0.0f, 0.0f, -10.0f),*/
mRight(1.0f, 0.0f, 0.0f),
mUp(0.0f, 1.0f, 0.0f),
mLook(0.0f, 0.0f, 1.0f)
{
	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mWorldPosition);

	//XMFLOAT3 mWorldPosition;
	//XMFLOAT3 mScale;
	//XMFLOAT4 mRotate;

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;

	mLightData.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLightData.Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	BuildBuffers(pd3dDevice);

	m_Shadow_Texture = NULL;
	//width = 1024;
	//height = 1024;
	
	
	

}

DirectionalLightActor::~DirectionalLightActor()
{
	m_Shadow_Texture->Shutdown();
	delete m_Shadow_Texture;
}

XMFLOAT4 DirectionalLightActor::GetLightColor()
{
	return mLightData.Color;
}

void DirectionalLightActor::SetLightColor(XMFLOAT3 val)
{
	mLightData.Color = XMFLOAT4(val.x, val.y, val.z, 1.0f);	
}

XMFLOAT3 DirectionalLightActor::GetLightDirection()
{	
	return mLightData.Direction;
}

HRESULT DirectionalLightActor::SetLightDirection(XMFLOAT3 val)
{
	mLightData.Direction = val;
	return true;
}

void DirectionalLightActor::UpdateRLU()
{
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&GetWorldMatrix()));

	XMMATRIX R = XMMatrixRotationQuaternion(rotQuat);
	XMFLOAT4X4 RotationMat;
	XMStoreFloat4x4(&RotationMat, R);
	mLightData.Direction = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), RotationMat));

	XMFLOAT3 Up = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), RotationMat));
	XMFLOAT3 Right = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), RotationMat));

	XMStoreFloat3(&mUp, XMLoadFloat3(&Up));
	XMStoreFloat3(&mLook, XMLoadFloat3(&mLightData.Direction));
	XMStoreFloat3(&mRight, XMLoadFloat3(&Right));

	UpdateViewMatrix();
}

void DirectionalLightActor::UpdateFrustrum(UINT width, UINT height)
{
	//XMMATRIX PM = XMMatrixOrthographicLH((float)width, (float)height, 1.0f, 400.0f);
	XMMATRIX PM = XMMatrixOrthographicLH((float)350, (float)350, 1.0f, 400.0f);
	//XMMATRIX PM = XMMatrixOrthographicOffCenterLH(-(float)width, (float)width, -(float)height, (float)height, 1.0f, 400.0f);
	XMStoreFloat4x4(&mProj, PM);
	BoundingFrustum::CreateFromMatrix(mFrustum, PM);
}


void DirectionalLightActor::Translation(float x, float y, float z)
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

	UpdateViewMatrix();
}


void DirectionalLightActor::Rotation(FXMVECTOR axis, float angle)
{
	Actor::Rotation(axis, angle);

	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&GetWorldMatrix()));
	
	XMMATRIX R = XMMatrixRotationQuaternion(rotQuat);
	XMFLOAT4X4 RotationMat;
	XMStoreFloat4x4(&RotationMat, R);
	mLightData.Direction = MathHelper::XMFLOAT4_TO_XMFLOAT3( MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), RotationMat));

	XMFLOAT3 Up = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), RotationMat));
	XMFLOAT3 Right = MathHelper::XMFLOAT4_TO_XMFLOAT3(MathHelper::XMFLOAT4_MUL_XMFLOAT4X4(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), RotationMat));
	
	XMStoreFloat3(&mUp, XMLoadFloat3(&Up));
	XMStoreFloat3(&mLook, XMLoadFloat3(&mLightData.Direction));
	XMStoreFloat3(&mRight, XMLoadFloat3(&Right));
	
	UpdateViewMatrix();
}

XMMATRIX DirectionalLightActor::SunView() const
{
	return XMLoadFloat4x4(&m_SunViewMatrix);
}

XMMATRIX DirectionalLightActor::SunViewProj() const
{
	return XMMatrixMultiply(SunView(), Proj());
}


XMMATRIX DirectionalLightActor::View()const
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX DirectionalLightActor::Proj()const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX DirectionalLightActor::ViewProj()const
{
	return XMMatrixMultiply(View(), Proj());
}


XMMATRIX DirectionalLightActor::InvView() const
{
	return MathHelper::Inverse(XMLoadFloat4x4(&mView));
}

XMMATRIX DirectionalLightActor::InvProj() const
{
	return MathHelper::Inverse(XMLoadFloat4x4(&mProj));
}

XMMATRIX DirectionalLightActor::InvViewProj() const
{
	return MathHelper::Inverse(XMMatrixMultiply(XMLoadFloat4x4(&mView), XMLoadFloat4x4(&mProj)));
}

void DirectionalLightActor::UpdateViewMatrix()
{
	
	m_SunWorldPosition = mWorldPosition;// mLook *100000.0f;

	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mWorldPosition);

	XMVECTOR S = XMLoadFloat3(&m_SunWorldPosition);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Normalize(XMVector3Cross(U, L));

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	// Fill in the view matrix entries.
	float sx = -XMVectorGetX(XMVector3Dot(S, R));
	float sy = -XMVectorGetX(XMVector3Dot(S, U));
	float sz = -XMVectorGetX(XMVector3Dot(S, L));

	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
	XMStoreFloat3(&mLook, L);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;


	m_SunViewMatrix(0, 0) = mRight.x;
	m_SunViewMatrix(1, 0) = mRight.y;
	m_SunViewMatrix(2, 0) = mRight.z;
	m_SunViewMatrix(3, 0) = sx;

	m_SunViewMatrix(0, 1) = mUp.x;
	m_SunViewMatrix(1, 1) = mUp.y;
	m_SunViewMatrix(2, 1) = mUp.z;
	m_SunViewMatrix(3, 1) = sy;

	m_SunViewMatrix(0, 2) = mLook.x;
	m_SunViewMatrix(1, 2) = mLook.y;
	m_SunViewMatrix(2, 2) = mLook.z;
	m_SunViewMatrix(3, 2) = sz;

	m_SunViewMatrix(0, 3) = 0.0f;
	m_SunViewMatrix(1, 3) = 0.0f;
	m_SunViewMatrix(2, 3) = 0.0f;
	m_SunViewMatrix(3, 3) = 1.0f;	
}


DirectionalLightActorManager::DirectionalLightActorManager()
{
}

DirectionalLightActorManager::~DirectionalLightActorManager()
{
}

HRESULT DirectionalLightActorManager::LoadDLA(ID3D11Device *pd3dDevice)
{
	DirectionalLightActor *mDLA = new DirectionalLightActor(pd3dDevice);

	//HR(mDLA->SetLightDirection(XMFLOAT3(dirx, diry, dirz)));
	mDLA_List.push_back(mDLA);

	//delete mDLA;

	return true;
}