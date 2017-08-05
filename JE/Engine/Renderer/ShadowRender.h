////////////////////////////////////////////////////////////////////////////////
// Filename: ShadowRender.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ShadowRender_H_
#define _ShadowRender_H_

#define SHADOWMAP_SIZE 8192

//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "RenderCapsule.h"
#include "../Actor/Camera.h"
#include "../Common/GameTimer.h"
#include "../Actor/DirectionalLightActor.h"
#include "RenderStates.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: ShadowRender
////////////////////////////////////////////////////////////////////////////////
class ShadowRender : public RenderCapsule
{

public:
	ShadowRender();
	ShadowRender(const ShadowRender&);
	virtual ~ShadowRender();

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);
	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);

	
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;

private:
	
};

#endif