#ifndef OBJECT_H
#define OBJECT_H

#include "../../stdafx.h"
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "../Textures/Texture.h"
#include "../Common/Geometry.h"

struct ObjectInfo
{
	//char m_strPath[MAX_PATH];

	WCHAR m_GeometryPath[MAX_PATH];
	WCHAR m_MaterialPath[MAX_MATERIAL][MAX_PATH];
	//WCHAR m_TexturePath[MAX_MATERIAL][MAX_TEXTURE][MAX_PATH];

	UINT m_LayerCount;
	//UINT m_MaterialClass[MAX_MATERIAL];

	//wchar_t m_strTexturePath[MAX_MATERIAL][MAX_TEXTURE][MAX_PATH];
	//UINT m_TextureListCount[MAX_MATERIAL];
	
	XMFLOAT4X4 m_World;
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Scale;
	XMFLOAT4 m_Rotate;
};

struct TextureManageInfo
{
	Texture* m_TextureList;
	UINT m_TextureListCount;
};

class Object
{
public:
	Object();
	virtual ~Object();

	virtual void Shutdown();


	void GetObjectInfo(ObjectInfo &Info);
	void SetObjectInfo(ObjectInfo &Info);
	

	virtual void Import(const char* path)
	{
		
	}

	virtual void Draw(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc);

	void DrawBoundingBox(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc);


	void Translation(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Rotation(FXMVECTOR axis, float angle);

	//UINT GetIndexCount();
	XMFLOAT4X4 GetWorldMatrix();	
	void SetWorldMatrix(CXMMATRIX WorldMat);

	XMFLOAT3 GetOriginPointPos();
	XMFLOAT3 GetScale();
	XMFLOAT3 GetReciprocalScale();
	XMFLOAT4 GetRotate();
	XMFLOAT3 GetX_Axis();
	XMFLOAT3 GetY_Axis();
	XMFLOAT3 GetZ_Axis();

	void GetTBN();

	void SetLayerCount(UINT val);
	UINT GetLayerCount();
	

	UINT Object::GetiKind(UINT index);	

	void CreateMaterials(UINT uSize);
	void SetMaterials(UINT index, Effect* pEffect, UINT iKind);
	Effect* GetMaterials(UINT index);

	Triangle* GetTriangleArray();
	void SetTriangleArray(Triangle* p);

	BoundingBox GetAABB();

	MeshData* GetMeshData();
	void SetMeshData(MeshData paraMeshdata);

	void CalculateTriangleArray(long vertexCount, const XMFLOAT3 *vertex, const XMFLOAT2 *texcoord, long triangleCount, const Triangle *triangle);	
	Object* GetThisObject();

	std::vector<std::vector<UINT>> OriginalIndexArray;

	void Update(float deltaTime)
	{
		if (bRotate)
		{
			Rotation(MathHelper::XMFLOAT3_TO_XMVECTOR(XMFLOAT3(0,0,1), 1.0f), deltaTime);
		}
	}

	
	UINT uSmoothingGroupMax;

	bool bSelected;
	bool bRotate;

	Geometry* pGeo;

	BasicMaterialEffect_Instance* pMI;

protected:
	BoundingBox mAxisAlignedBox;
	
private:	

	XMFLOAT4X4 mWorld;
	XMFLOAT3 mPosition;
	XMFLOAT3 mScale;
	XMFLOAT4 mRotate;

	XMFLOAT3 mX_Axis;
	XMFLOAT3 mY_Axis;
	XMFLOAT3 mZ_Axis;

	MeshData mMeshData;

	Effect** mEffectArray;	
	UINT* mEffectKind;
};


class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	std::list<Object*> mObj_List;
private:
};

class UIObjectManager
{
public:
	UIObjectManager();
	~UIObjectManager();

	std::vector<Object*> mUIObj_List;
private:
};

#endif