////////////////////////////////////////////////////////////////////////////////
// Filename: CubeMapRender.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CubeMapRender_H_
#define _CubeMapRender_H_


//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "RenderCapsule.h"
#include "../Actor/Camera.h"
#include "../Common/GameTimer.h"
#include "../Actor/ReflectionActor.h"
#include "RenderStates.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: CubeMapRender
////////////////////////////////////////////////////////////////////////////////
class CubeMapRender : public RenderCapsule
{

public:
	CubeMapRender();
	CubeMapRender(const CubeMapRender&);
	virtual ~CubeMapRender();

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);
	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	

private:
	
	
};

#endif