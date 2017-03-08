#ifndef ACTOR_H
#define ACTOR_H

#include "../../stdafx.h"
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
struct ActorInfo
{
	XMFLOAT3 m_WorldPosition;
	XMFLOAT3 m_Scale;
	XMFLOAT4 m_Rotate;
	XMFLOAT4X4 m_WorldMat;
};

class Actor
{
public:
	Actor();


	virtual ~Actor();
	
	virtual void DrawUISprite(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc);
	virtual void DrawActorGizmo(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc);
	XMFLOAT3 GetWorldPosition();
	void SetWorldPosition(XMFLOAT3 val);

	XMFLOAT3 GetScale();
	void SetScale(XMFLOAT3 val);

	XMFLOAT4 GetRotate();
	void SetRotate(XMFLOAT4 val);

	
	XMFLOAT4X4 GetWorldMatrix();	
	void SetWorldMatrix(XMFLOAT4X4 val);

	BoundingBox GetAABB();

	XMFLOAT3 GetOriginPointPos();
	void CalculateRotaionMatrix(XMFLOAT3 CameraPos, XMFLOAT4X4 mView);
	virtual void Translation(float x, float y, float z);
	void Scale(float x, float y, float z);
	virtual void Rotation(FXMVECTOR axis, float angle);

	virtual void BuildBuffers(ID3D11Device *pd3dDevice);
	virtual void BuildActorBuffers(ID3D11Device *pd3dDevice) {};
	void BuildVertexBuffers(ID3D11Device *pd3dDevice, Vertex *pvertices, D3D11_USAGE USAGE, UINT totalVertexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS, ID3D11Buffer** pVertexBuffer);
	void BuildIndexBuffers(ID3D11Device *pd3dDevice, UINT *pindices, D3D11_USAGE USAGE, UINT totalIndexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS, ID3D11Buffer** pIndexBuffer);
	void UpdateBuffer(ID3D11DeviceContext* deviceContext, XMFLOAT3 EyeWorldPos);

	void Render(ID3D11DeviceContext* pd3dImmediateContext, XMFLOAT3 EyeWorldPos, Effects* pEffects, XMFLOAT4X4 ViewMat, XMFLOAT4X4 ProjMat, ID3DX11EffectTechnique* pTech,
		D3DX11_TECHNIQUE_DESC &techDesc, ID3D11ShaderResourceView* pShaderResourceView);

	bool bSelected;

	UINT GetIndexCount()
	{
		return m_IndexCount;
	}

	UINT* GetIndex()
	{
		return m_Indices;
	}

	Vertex* GetVertex()
	{
		return m_Vertex;
	}

	Actor* GetThisActor()
	{
		return this;
	}

	XMFLOAT4X4 CalculateTransformMat(float X, float Y, XMFLOAT4 OriginPoint)
	{
		return XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-OriginPoint.x + X, -OriginPoint.y + Y, 0.0f, 1.0f);
	}

protected:
	BoundingBox m_AxisAlignedBox;

	ID3D11Buffer* m_VertexBufferforActorGizmo; //Vertex buffer
	ID3D11Buffer* m_IndexBufferforActorGizmo; //Index buffer	
	Vertex* m_VertexforActorGizmo;
	UINT*  m_IndicesforActorGizmo;
	UINT m_VertexCountforActorGizmo;
	UINT m_IndexCountforActorGizmo;

	XMFLOAT4X4 m_WorldMatrix;
	XMFLOAT3 mWorldPosition;
	XMFLOAT3 mScale;
	XMFLOAT4 mRotate;

	XMMATRIX m_WorldMat;
	XMMATRIX m_LookMat;
	XMMATRIX m_ProjMat;

private:

	ID3D11Buffer* mVB; //Vertex buffer
	ID3D11Buffer* mIB; //Index buffer	
	Vertex* m_Vertex;
	UINT*  m_Indices;
	UINT m_VertexCount;
	UINT m_IndexCount;


	


	//Texture* m_pTexture;
	

};

#endif