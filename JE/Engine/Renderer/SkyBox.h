////////////////////////////////////////////////////////////////////////////////
// Filename: SkyBox.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SkyBox_H_
#define _SkyBox_H_


//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "RenderCapsule.h"
#include "../Actor/Camera.h"
#include "../Common/GameTimer.h"
////////////////////////////////////////////////////////////////////////////////
// Class name: SkyBox
////////////////////////////////////////////////////////////////////////////////
class SkyBox : public RenderCapsule
{

public:
	SkyBox();
	SkyBox(const SkyBox&);
	virtual ~SkyBox();

	virtual bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int resolutionWidth, int resolutionHeight);
	virtual void Shutdown();
	virtual void OnResize(int ScreenWidth, int ScreenHeight, int MapWidth, int MapHeight);
	virtual bool Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY, XMFLOAT4X4 InvViewProj, ID3DX11EffectTechnique* pTech, Effects* pEffects);

	void BuildSkyBox(ID3D11Device* md3dDevice);	

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;	

	ID3D11Buffer* m_VBforSkyBox; //Vertex buffer
	ID3D11Buffer* m_IBforSkyBox; //Index buffer
	ID3D11ShaderResourceView* m_SkyCubeMap_SRV;

private:
	

	ID3D11Texture2D* m_SkyCubeMap_Texture;
	
	

	
};

#endif