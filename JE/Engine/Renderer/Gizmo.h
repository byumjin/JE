////////////////////////////////////////////////////////////////////////////////
// Filename: Gizmo.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GIZMO_H_
#define _GIZMO_H_


//////////////
// INCLUDES //
//////////////
#include "../../stdafx.h"
#include "../Common/D3DUtil.h"
#include "../Object/Object.h"
#include "../Actor/Actor.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: Gizmo
////////////////////////////////////////////////////////////////////////////////
class Gizmo
{
private:	

public:
	Gizmo();
	~Gizmo();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight);
	void Shutdown();
	void OnResize(int ScreenWidth, int ScreenHeight);
	bool Render(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTech, Effects* pEffects, XMFLOAT4X4 World, XMFLOAT4X4 WorldViewProj, FLOAT X, FLOAT Y, FLOAT L, int Flag);

	int GetIndexCount();
	int GetTIndexCount();
	int GetRIndexCount();
	int GetSIndexCount();

	int GetselectedTAxis();

	void SetselectedTAxis(int para);
	void SetPushedAxis(int para);	

	int GetPushedAxis();
	Object* GetSelectedObj();
	void SetSelectedObj(Object* pObj);	

	Actor* GetSelectedActor();
	void SetSelectedActor(Actor* pObj);


	XMFLOAT3 Get_Axis_TO_SCREEN(XMFLOAT3 vec, XMFLOAT4X4 WorldViewProj);
	XMFLOAT3 Get_PIXEL_TO_LOCAL(XMFLOAT2 ScreenPos, XMFLOAT2 ScreenSize, XMFLOAT4X4 InvWorldViewProj);
	XMFLOAT2 Get_SCREEN_TO_PIXEL(XMFLOAT2 ScreenPos, XMFLOAT2 ScreenSize);	
	XMFLOAT3 Get_PIXEL_TO_SCREEN(XMFLOAT2 PixelPos, XMFLOAT2 ScreenSize);
	XMFLOAT3 Get_OBj_ORIGINPOINT_TO_SCREEN( XMFLOAT4X4 WorldViewProj);

	void HoverT(int sx, int sy, float WorldPositionX, float WorldPositionY, XMFLOAT4X4 Proj, XMFLOAT4X4 ViewMat, XMFLOAT4X4 WorldMat);

	void OnMouseMove(float DeltaTime, int x, int y, int LastMousePosX, int LastMousePosY, int ClientWidth, int ClientHeight, XMFLOAT4X4* pWorld, XMFLOAT4X4* pWVP, XMFLOAT3 GizmoLookVec, UINT iKind);
	void PushedAxisProcessing(XMFLOAT3 vDragged, XMFLOAT3 TransStandardAxis, XMFLOAT3 RotStandardAxis, XMFLOAT3 vRadius, XMFLOAT3 GizmoLookVec, float dist, XMFLOAT4X4* pWorld, XMFLOAT4X4* pWVP, UINT iKind);
	int mGizmoStatus;

	XMFLOAT2 m_SelectedObjScreenPos;

private:
	bool InitializeBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool InitializeBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT VertexCount, UINT IndexCount, ID3D11Buffer** VertexBuffer, ID3D11Buffer** IndexBuffer);
	void ShutdownBuffers();

	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, Effects* pEffects, XMFLOAT4X4 World, XMFLOAT4X4 WorldViewProj, FLOAT X, FLOAT Y, FLOAT L, int Flag);
	void RenderBuffers(ID3D11DeviceContext* deviceContext, ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, D3D11_PRIMITIVE_TOPOLOGY KIND);
	
	XMFLOAT4X4 CalculateTransformMat(float X, float Y, XMFLOAT4 OriginPoint);
	XMFLOAT4 CalculateVertex(float x, float y, float z, XMFLOAT4X4 WorldViewProj, XMFLOAT4X4 TransformMat, XMFLOAT4X4 InvWorldViewProj);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Vertex *mBasicGizmoVertices;

	ID3D11Buffer *m_TvertexBuffer, *m_TindexBuffer;
	int m_TvertexCount, m_TindexCount;

	XMFLOAT4X4 mTransform;
	XMFLOAT4X4 mGizmoWorld;

	Vertex *mTvertices;

	BoundingBox mAxis_X_AlignedBox;
	BoundingBox mAxis_Y_AlignedBox;
	BoundingBox mAxis_Z_AlignedBox;
	BoundingBox mMiddle_AlignedBox;
	XMFLOAT3 X_AXIS;
	XMFLOAT3 Y_AXIS;
	XMFLOAT3 Z_AXIS;

	int selectedTAxis;
	int bPushedAxis;

	ID3D11Buffer *m_RvertexBuffer, *m_RindexBuffer;
	int m_RvertexCount, m_RindexCount;

	Vertex *mRvertices;

	ID3D11Buffer *m_SvertexBuffer, *m_SindexBuffer;
	int m_SvertexCount, m_SindexCount;

	Vertex *mSvertices;

	BoundingBox mRAxis_X_AlignedBox;
	BoundingBox mRAxis_Y_AlignedBox;
	BoundingBox mRAxis_Z_AlignedBox;

	int m_screenWidth, m_screenHeight;

	Object *pSelectedObject;	
	Actor *pSelectedActor;

};

#endif