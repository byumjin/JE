#ifndef RENDERTEXTURE_H
#define RENDERTEXTURE_H

//////////////
// INCLUDES //
//////////////
#include "Texture.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTexture
////////////////////////////////////////////////////////////////////////////////
class RenderTexture
{
public:
	RenderTexture();
	RenderTexture(const RenderTexture&);
	~RenderTexture();

	bool Initialize(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
		DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	void OnResize(UINT textureWidth, UINT textureHeight, ID3D11DeviceContext* d3dImmediateContext, ID3D11DepthStencilView* DepthStencilView);


	//Set Rendertarget to Texture
	void RenderToTexture(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* DepthStencilView)
	{
		SetRenderTarget(deviceContext, DepthStencilView);
		ClearRenderTarget(deviceContext, DepthStencilView, 0.3f, 0.6f, 1.0f, 1.0f);
	}

	ID3D11RenderTargetView* GetRenderTargetView();
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11Texture2D* GetRenderTargetTexture();

private:

	TextureInfo mTextureInfo;

	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;
};

class MultiRenderTexture
{
public:
	MultiRenderTexture();
	MultiRenderTexture(const MultiRenderTexture&);
	~MultiRenderTexture();

	bool Initialize(ID3D11Device* device, UINT textureNum);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* DepthStencilView);
	void ReleaseRenderTarget(ID3D11DeviceContext* deviceContext);
	
	void UnbindDepthStencil(ID3D11DeviceContext* deviceContext);

	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	void OnResize(UINT textureWidth, UINT textureHeight, ID3D11DeviceContext* d3dImmediateContext, ID3D11DepthStencilView* DepthStencilView);
	
	HRESULT CreateRenderTexture(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
		DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags, UINT Num);

	ID3D11ShaderResourceView* GetShaderResourceView(int Num);
	ID3D11RenderTargetView* GetRenderTargetView(int Num);

	ID3D11RenderTargetView** GetRenderTargetViewArray()
	{
		return m_renderTargetViewArray;
	}

	//Set Rendertarget to Texture
	void RenderToTexture(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* DepthStencilView)
	{
		SetRenderTarget(deviceContext, DepthStencilView);
		ClearRenderTarget(deviceContext, DepthStencilView, 0.3f, 0.6f, 1.0f, 1.0f);
	}

	bool Reset();


	

private:
	UINT mInum;

	TextureInfo* mTextureInfo;

	ID3D11Texture2D** m_renderTargetTextureArray;
	ID3D11RenderTargetView** m_renderTargetViewArray;
	ID3D11ShaderResourceView** m_shaderResourceViewArray;
};

class RenderTextureManger
{
public:

	RenderTextureManger();
	~RenderTextureManger();

	std::vector<RenderTexture*> mRT_List;

	HRESULT LoadRT(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
		DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags);

	void OnResize(UINT width, UINT height, ID3D11DeviceContext* pd3dImmediateContext, ID3D11DepthStencilView* pDepthStencilView);
	

	
};

#endif