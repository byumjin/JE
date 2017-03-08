#ifndef REFLECTION_H
#define REFLECTION_H

#include "../../stdafx.h"
#include "../Object/Object.h"
#include "../Textures/RenderTexture.h"
#include "Actor.h"
#include "Camera.h"

static const int CubeMapSize = 256;

struct ReflectionData
{
	float radius;
};

struct ReflectionActorInfo
{
	ActorInfo m_ActorInfo;
	ReflectionData m_RAData;
};


__declspec(align(16)) class ReflectionActor : public Actor
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

	ReflectionActor(ID3D11Device* pd3dDevice);
	virtual ~ReflectionActor();
	
	int Check_IntersectAxisAlignedBoxFrustum(Object* pObject, XMFLOAT4X4* WorldMat, XMMATRIX* ViewMat, Camera* pCamera);
	void DrawScene(ID3D11DeviceContext* pd3dImmediateContext, ObjectManager* pObjectManager);	
	virtual void BuildActorBuffers(ID3D11Device *pd3dDevice);
	void DrawActorGizmo(ID3D11DeviceContext* pd3dImmediateContext, Effects* pEffects, XMMATRIX* paraWorld, XMFLOAT4X4* paraWVP, FLOAT X, FLOAT Y, float paraLength);

	bool CreateCubeMap(ID3D11Device *pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, ObjectManager* pObjectManager, Effects* pEffects,
		ID3D11Buffer* m_VBforSkyBox, ID3D11Buffer* m_IBforSkyBox, ID3D11ShaderResourceView* SkyBoxSRV, XMMATRIX* pworldforSky, XMFLOAT3 gEyePos,
		ID3D11ShaderResourceView* WPSRV, ID3D11ShaderResourceView* WNSRV);

	ID3D11ShaderResourceView* GetCubeMapSRV();

	virtual void Translation(float x, float y, float z);
	
	FLOAT GetRadius()
	{
		return m_radius;
	}

	void SetRadius(float val)
	{
		m_radius = val;
	}

	void MnRadius(float val)
	{
		float result = m_radius + val;
		if (result < 1.0f)
			result = 1.0f;

		m_radius = result;
	}


private:
	
	void BuildCubeFaceCamera(float x, float y, float z);
	void BuildDynamicCubeMapViews(ID3D11Device* pd3dDevice);
	

	//RenderTexture m_RenderTexture[6];

	ID3D11Texture2D* m_RenderTexture[6];

	//ID3D11DepthStencilView* m_DynamicCubeMapDSV;
	ID3D11DepthStencilView* m_DynamicCubeMapDSV[6];

	ID3D11RenderTargetView* m_DynamicCubeMapRTV[6];

	ID3D11ShaderResourceView* m_DynamicCubeMapSRV;	
	ID3D11ShaderResourceView* m_DynamicCubeMapDSRV;
	
	
	D3D11_VIEWPORT m_CubeMapViewport;


	//ID3D11Texture2D* m_RenderTargetTexture[6];
	Camera m_CubeMapCamera[6];
	FLOAT m_radius;
};


class ReflectionActorManager
{
public:

	

	ReflectionActorManager()
	{}

	~ReflectionActorManager()
	{}

	std::list<ReflectionActor*> m_RA_List;

	HRESULT LoadRA(ID3D11Device *pd3dDevice)
	{
		ReflectionActor *m_RA = new ReflectionActor(pd3dDevice);
				
		m_RA_List.push_back(m_RA);	

		return true;
	}

private:


};


#endif