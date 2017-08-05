#include "Camera.h"

Camera::Camera() : mPosition(0.0f, 0.0f, -10.0f),
mRight(1.0f, 0.0f, 0.0f),
mUp(0.0f, 1.0f, 0.0f),
mLook(0.0f, 0.0f, 1.0f)
{
	bRotate1 = false;
	bRotate2 = false;

	bForward = false;
	bBackward = false;

	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

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

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mRotateMat, I);
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3 &v)
{
	mPosition = v;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight() const
{
	return mRight;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook() const
{
	return mLook;
}


void Camera::SetNearZ(float value)
{
	mNearZ = value;
}

void Camera::SetFarZ(float value)
{
	mFarZ = value;
}


float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ()const
{
	return mFarZ;
}

float Camera::GetAspect()const
{
	return mAspect;
}

void Camera::SetAspect(float Aspect)
{
	mAspect = Aspect;
}

void Camera::SetFovY(float FovY)
{
	mFovY = FovY;
}

float Camera::GetFovY()const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}
	
float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

void Camera::SetNearWindowHeight(float value)
{
	mNearWindowHeight = value;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

void Camera::SetFarWindowHeight(float value)
{
	mFarWindowHeight = value;
}

float Camera::GetDepthW()
{
	return -1.0f*((mFarZ*mNearZ) / (mFarZ - mNearZ));
}

void  Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f*mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f*mFovY);
	
	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void  Camera::SetLensOrthographic(float width, float height, float zn, float zf)
{
	// cache properties	
	mNearZ = zn;
	mFarZ = zf;
		
	XMMATRIX P = XMMatrixOrthographicLH(width, height, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

void Camera::SetFrustumFromProjection()
{
	BoundingFrustum::CreateFromMatrix(mFrustum, Proj());	
}

BoundingFrustum Camera::GetFrustum()
{
	return mFrustum;
}

void Camera::SetFrustum(BoundingFrustum Frustum)
{
	mFrustum = Frustum;
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, L);
	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}

XMMATRIX Camera::View()const
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::Proj()const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::ViewProj()const
{
	return XMMatrixMultiply(View(), Proj());
}


XMMATRIX Camera::InvView() const
{
	return MathHelper::Inverse(XMLoadFloat4x4(&mView));
}

XMMATRIX Camera::InvProj() const
{
	return MathHelper::Inverse(XMLoadFloat4x4(&mProj));
}

XMMATRIX Camera::InvViewProj() const
{
	return MathHelper::Inverse(XMMatrixMultiply(XMLoadFloat4x4(&mView),XMLoadFloat4x4(&mProj)));
}

void Camera::Strafe(float d)
{
	// mPosition += d*mRight
	
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, XMVector3Normalize(r), p));		
}

void Camera::Walk(float d)
{
	// mPosition += d*mLook
	
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, XMVector3Normalize(l), p));
	
}

void Camera::Pitch(float angle)
{
	
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&mRotateMat));

	// Rotate up and look vector about the right vector.	
	XMVECTOR Orientation = XMQuaternionRotationMatrix(XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle));
	Orientation = XMQuaternionNormalize(Orientation);
	XMMATRIX R = XMMatrixRotationQuaternion(Orientation);
	
	
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
		
	//Second rotation
	R = XMMatrixMultiply(R, XMMatrixRotationQuaternion(rotQuat));
	// Rebuild the rotation matrix from the quaternion.
	XMStoreFloat4x4(&mRotateMat, XMMatrixMultiply(XMLoadFloat4x4(&mRotateMat), R));
	
}

void Camera::RotateY(float angle)
{

	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;

	XMMatrixDecompose(&scale, &rotQuat, &translation, XMLoadFloat4x4(&mRotateMat));

	XMVECTOR Orientation = XMQuaternionRotationMatrix(XMMatrixRotationY(angle));
	Orientation = XMQuaternionNormalize(Orientation);
	XMMATRIX R = XMMatrixRotationQuaternion(Orientation);
	

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));	
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));



	//Second rotation
	R = XMMatrixMultiply(R, XMMatrixRotationQuaternion(rotQuat));
	// Rebuild the rotation matrix from the quaternion.
	XMStoreFloat4x4(&mRotateMat, XMMatrixMultiply(XMLoadFloat4x4(&mRotateMat), R));
	
}
void Camera::SetRight(const XMFLOAT3 &v)
{
	mRight = v;
}

void Camera::SetUp(const XMFLOAT3 &v)
{
	mUp = v;
}

void Camera::SetLook(const XMFLOAT3 &v)
{
	mLook = v;
}

void Camera::UpdateViewMatrix()
{
	
	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Normalize(XMVector3Cross(U, L));

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

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
}

