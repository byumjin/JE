#ifndef TEXTURE_H
#define TEXTURE_H

//////////////
// INCLUDES //
//////////////
#include "../Common/D3DUtil.h"
#include "../../Assets/Asset.h"

struct TextureInfo
{
	D3D11_TEXTURE2D_DESC textureDesc;
	
	ID3D11Device* device;
	bool bEnable4xMsaa;
	UINT m4xMsaaQuality;
	UINT TextureNum;
};


////////////////////////////////////////////////////////////////////////////////
// Class name: Texture
////////////////////////////////////////////////////////////////////////////////
class Texture : public Asset
{
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	void LoadFromImage(ID3D11Device* device, LPCWSTR path)
	{
		Initialize(device, path);
	}

	void LoadFromFilename(ID3D11Device* device, LPCWSTR path)
	{
		Initialize(device, path);
	}

	bool Initialize(ID3D11Device* device, LPCWSTR path);
	bool Initialize(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
		DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags);
	void Shutdown();

	//void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	//void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	
	//void OnResize(UINT textureWidth, UINT textureHeight, ID3D11DeviceContext* d3dImmediateContext, ID3D11DepthStencilView* DepthStencilView);


	//Set Rendertarget to Texture
	/*
	void RenderToTexture(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* DepthStencilView)
	{
		SetRenderTarget(deviceContext, DepthStencilView);
		ClearRenderTarget(deviceContext, DepthStencilView, 0.3f, 0.6f, 1.0f, 1.0f);
	}
	*/

	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11Texture2D* GetTexture();
	
	void SetShaderResourceView(ID3D11ShaderResourceView* SRV);
	
	wchar_t m_strPath[MAX_PATH];
private:

	TextureInfo mTextureInfo;

	ID3D11Texture2D* mTexture;
	//ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* mshaderResourceView;
};

#endif