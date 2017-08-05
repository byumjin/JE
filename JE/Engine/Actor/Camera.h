#ifndef CAMERA_H
#define CAMERA_H

//#include "../stdafx.h"
//#include "../Common/D3DUtil.h"
#include "Actor.h"

#define NEAR_DIST 0.1f
#define FAR_DIST 1000.0f

struct CameraActorInfo
{
	ActorInfo m_ActorInfo;
	
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Right;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Look;

	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY;
	float m_NearWindowHeight;
	float m_FarWindowHeight;

	XMFLOAT4X4 m_View;
	XMFLOAT4X4 m_Proj;

	BoundingFrustum m_Frustum;
};

class Camera : public Actor
{
public:
	Camera();
	~Camera();

	//
	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 &v);

	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	void SetRight(const XMFLOAT3 &v);

	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	void SetUp(const XMFLOAT3 &v);
	
	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;
	void SetLook(const XMFLOAT3 &v);

	void SetNearZ(float value);
	void SetFarZ(float value);
	

	float GetNearZ() const;
	float GetFarZ() const;

	float GetAspect() const;
	void SetAspect(float Aspect);

	float GetFovY() const;
	void SetFovY(float FovY);

	float GetFovX() const;
	float GetDepthW();

	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	void SetNearWindowHeight(float value);
	void SetFarWindowHeight(float value);

	void SetLens(float fovY, float aspect, float zn, float zf);
	void SetLensOrthographic(float width, float height, float zn, float zf);
	void SetFrustumFromProjection();
	
	BoundingFrustum Camera::GetFrustum();
	void SetFrustum(BoundingFrustum Frustum);

	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3 &pos, const XMFLOAT3& target, const XMFLOAT3& up);

	XMFLOAT4X4 GetView()
	{
		return mView;
	}

	XMFLOAT4X4 GetProj()
	{
		return mProj;
	}

	void SetCameraActorInfo(CameraActorInfo &CameraInfo)
	{
		SetWorldPosition(CameraInfo.m_ActorInfo.m_WorldPosition);
		SetRotate(CameraInfo.m_ActorInfo.m_Rotate);
		SetScale(CameraInfo.m_ActorInfo.m_Scale);

		SetAspect(CameraInfo.m_Aspect);
		SetNearWindowHeight(CameraInfo.m_NearWindowHeight);
		SetFarWindowHeight(CameraInfo.m_FarWindowHeight);
		SetFarZ(CameraInfo.m_FarZ);
		SetNearZ(CameraInfo.m_NearZ);
		SetFovY(CameraInfo.m_FovY);

		SetPosition(CameraInfo.m_Position);
		SetLook(CameraInfo.m_Look);
		SetRight(CameraInfo.m_Right);
		SetUp(CameraInfo.m_Up);

		SetFrustum(CameraInfo.m_Frustum);
	}

	void GetCameraActorInfo(CameraActorInfo &CameraInfo)
	{
		CameraInfo.m_ActorInfo.m_WorldPosition = GetWorldPosition();
		CameraInfo.m_ActorInfo.m_Rotate = GetRotate();
		CameraInfo.m_ActorInfo.m_Scale = GetScale();

		CameraInfo.m_Aspect = GetAspect();
		CameraInfo.m_FarWindowHeight = GetFarWindowHeight();
		CameraInfo.m_NearWindowHeight = GetNearWindowHeight();
		CameraInfo.m_FarZ = GetFarZ();
		CameraInfo.m_NearZ = GetNearZ();
		CameraInfo.m_FovY = GetFovY();

		CameraInfo.m_Position = GetPosition();
		CameraInfo.m_Look = GetLook();
		CameraInfo.m_Right = GetRight();
		CameraInfo.m_Up = GetUp();

		CameraInfo.m_Proj = GetProj();
		CameraInfo.m_View = GetView();

		CameraInfo.m_Frustum = GetFrustum();
	}

	XMMATRIX View() const;
	XMMATRIX Proj() const;
	XMMATRIX ViewProj() const;

	XMMATRIX InvView() const;
	XMMATRIX InvProj() const;
	XMMATRIX InvViewProj() const;

	void Strafe(float d);
	void Walk(float d);

	void Pitch(float angle);
	void RotateY(float angle);

	void UpdateViewMatrix();

	XMFLOAT4X4 GetRotateMat()
	{
		return mRotateMat;
	}

	XMFLOAT3 mLook;
	bool bRotate1;
	bool bRotate2;

	bool bForward;
	bool bBackward;
private:

	XMFLOAT3 mPosition;
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	

	XMFLOAT4 mRotate;
	XMFLOAT4X4 mRotateMat;


	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	BoundingFrustum mFrustum;

	
};

#endif