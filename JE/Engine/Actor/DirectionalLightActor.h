#ifndef DIRECTIONALLIGHTACTOR_H
#define DIRECTIONALLIGHTACTOR_H

#include "LightActor.h"
#include "../Textures/RenderTexture.h"

struct DirectionalLightActorInfo
{
	ActorInfo m_ActorInfo;
	DirectionalLight m_LightData;
};

__declspec(align(16)) class DirectionalLightActor : public LightActor
{
public:

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	DirectionalLightActor(ID3D11Device *pd3dDevice);
	virtual ~DirectionalLightActor();

	XMFLOAT4 GetLightColor();
	void SetLightColor(XMFLOAT3 val);

	XMFLOAT3 GetLightDirection();
	HRESULT SetLightDirection(XMFLOAT3 val);

	DirectionalLight GetDirLightData()
	{
		return mLightData;
	}

	void SetDirLightData(DirectionalLight data)
	{
		mLightData = data;
	}

	virtual void Rotation(FXMVECTOR axis, float angle);

	virtual void Translation(float x, float y, float z);

	void UpdateViewMatrix();

	void UpdateRLU();
	

	void UpdateFrustrum(UINT width, UINT height);
	

	XMMATRIX SunView() const;
	XMMATRIX SunViewProj() const;

	XMMATRIX View() const;
	XMMATRIX Proj() const;
	XMMATRIX ViewProj() const;

	XMMATRIX InvView() const;
	XMMATRIX InvProj() const;
	XMMATRIX InvViewProj() const;

	RenderTexture* m_Shadow_Texture;
private:	

	DirectionalLight mLightData;

	//UINT width;
	//UINT height;

	/*
	XMFLOAT3 mPosition;
	*/
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;

	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	XMFLOAT4X4 m_SunViewMatrix;
	XMFLOAT3 m_SunWorldPosition;

	BoundingFrustum mFrustum;

};

class DirectionalLightActorManager
{
public:
	DirectionalLightActorManager();
	~DirectionalLightActorManager();

	std::list<DirectionalLightActor*> mDLA_List;

	HRESULT LoadDLA(ID3D11Device *pd3dDevice);

private:

	
};









#endif