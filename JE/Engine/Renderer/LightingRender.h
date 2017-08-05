////////////////////////////////////////////////////////////////////////////////
// Filename: LightingRender.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LightingRender_H_
#define _LightingRender_H_




//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "RenderCapsule.h"
#include "../Actor/Camera.h"
#include "../Common/GameTimer.h"
#include "RenderStates.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: LightingRender
////////////////////////////////////////////////////////////////////////////////
class LightingRender : public RenderCapsule
{

public:
	LightingRender();
	LightingRender(const LightingRender&);
	virtual ~LightingRender();

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);
	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);
		
	void SetBlendState(bool bAlphaToCoverageEnable, bool bIndependentBlendEnable, bool bBlendEnable,
		D3D11_BLEND SrcBlend, D3D11_BLEND DestBlend, D3D11_BLEND_OP BlendOp, D3D11_BLEND SrcBlendAlpha, D3D11_BLEND DestBlendAlpha, D3D11_BLEND_OP BlendOpAlpha, UINT8 RenderTargetWriteMask);

	ID3D11Device* pDevice;
	ID3D11BlendState* pBlendState;

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;	

private:
		
};

#endif