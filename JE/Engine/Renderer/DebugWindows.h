////////////////////////////////////////////////////////////////////////////////
// Filename: DebugWindows.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DebugWindows_H_
#define _DebugWindows_H_


//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "DebugWindow.h"
#include "../Actor/DirectionalLightActor.h"
#include "../Actor/Camera.h"
#include "Scene.h"
#include "../Common/GameTimer.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: DebugWindows
////////////////////////////////////////////////////////////////////////////////
class DebugWindows : public DebugWindow
{

public:
	DebugWindows();
	DebugWindows(const DebugWindows&);
	virtual ~DebugWindows();

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);
	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);
	void PushSwitch(int x);

	DebugWindow m_DebugWindow[12];

	//bool *pBshowDebugWindows;

	//std::list<DirectionalLightActor*>::iterator *ppDLi;

	
	Scene *pScene;


	RenderTexture* m_BloomTexture;
	RenderTexture* m_LensFlareTexture;
	RenderTexture* m_HBAOTexture;

	RenderTexture* preflectionTexture;
	RenderTexture* pGaussianHTexture;

	RenderTexture* pShadowTexture;


	bool Switch[12];
private:
	
};

#endif