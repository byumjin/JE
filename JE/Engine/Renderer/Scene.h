////////////////////////////////////////////////////////////////////////////////
// Filename: Scene.h
////////////////////////////////////////////////////////////////////////////////
#pragma  once

//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "RenderCapsule.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Scene
////////////////////////////////////////////////////////////////////////////////
class Scene : public RenderCapsule
{

public:
	Scene();
	Scene(const Scene&);
	virtual ~Scene();

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);
	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);

	void RenderBuffer(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effect* pEffect);

	int GetIndexCount();

	

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int, XMFLOAT4X4 InvViewProj);
	void RenderBuffers(ID3D11DeviceContext*, ID3DX11EffectTechnique* pTech, Effect* pEffect);
	XMFLOAT4 CalculateVertex(float X, float Y, XMFLOAT4X4 InvViewProj);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;

	

	Vertex *m_Vertice;
};

