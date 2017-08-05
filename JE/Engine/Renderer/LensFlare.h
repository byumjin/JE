////////////////////////////////////////////////////////////////////////////////
// Filename: LensFlare.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LensFlare_H_
#define _LensFlare_H_

//////////////
// INCLUDES //
//////////////
#include "RenderCapsule.h"
#include "Scene.h"
#include "../Actor/Camera.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: Gizmo
////////////////////////////////////////////////////////////////////////////////
class LensFlare : public RenderCapsule
{
public:
	LensFlare();
	virtual ~LensFlare();

	RenderTexture* m_LensFlareTexture;
	
	Texture* m_DirtMaskTexture;
	Texture* m_StarBurstTexture;

	RenderTexture* m_HighLightTexture;
	RenderTexture* m_GaussianVTexture;
	RenderTexture* m_GaussianHTexture;

	ID3D11Device* pDevice;

	ID3D11BlendState* pBlendState;

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);

	void SetBlendState(bool bAlphaToCoverageEnable, bool bIndependentBlendEnable, bool bBlendEnable,
		D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask);

	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);

	void RenderBuffer(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effect* pEffect);

	bool CreateFrameBuffers(ID3D11Device* pd3dDevice);

	int GetIndexCount();

	int GetScreenWidth()
	{
		return m_screenWidth;
	}

	int GetScreenHeight()
	{
		return m_screenHeight;
	}

	
private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int, XMFLOAT4X4 InvViewProj);
	void RenderBuffers(ID3D11DeviceContext*, ID3DX11EffectTechnique* pTech, Effect* pEffect);
	XMFLOAT4 CalculateVertex(float X, float Y, XMFLOAT4X4 InvViewProj);

	int m_vertexCount, m_indexCount;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;

	Vertex *m_Vertice;	


};

#endif