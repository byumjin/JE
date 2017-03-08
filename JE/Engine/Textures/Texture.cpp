#include "Texture.h"

Texture::Texture()
{
	mTexture = 0;
	//m_renderTargetView = 0;
	mshaderResourceView = 0;

	mTextureInfo.device = NULL;
	mTextureInfo.textureDesc.Width = 0;
	mTextureInfo.textureDesc.Height = 0;
	mTextureInfo.textureDesc.MipLevels = 0;
	mTextureInfo.textureDesc.ArraySize = 0;
	mTextureInfo.textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mTextureInfo.bEnable4xMsaa = false;
	mTextureInfo.m4xMsaaQuality = 0;
	mTextureInfo.textureDesc.Usage = D3D11_USAGE_DEFAULT;
	mTextureInfo.textureDesc.BindFlags = 0;
	mTextureInfo.textureDesc.CPUAccessFlags = 0;
	mTextureInfo.textureDesc.MiscFlags = 0;
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{
	Shutdown();
}

bool Texture::Initialize(ID3D11Device* device, LPCWSTR path)
{
	wcscpy_s(m_strPath, path);

	//DirectX::LoadFromTGAFile()

	//D3DX11CreateTextureFromFile D3DX11_IMAGE_LOAD_INFO 
	//HR(D3DX11CreateShaderResourceViewFromFile(device, path, NULL, NULL, &mshaderResourceView, NULL));
	
	HR(d3dHelper::CreateShaderResourceViewFromFile(device, path, &mshaderResourceView));
	mshaderResourceView->GetResource((ID3D11Resource **) &mTexture);
	
	D3D11_TEXTURE2D_DESC texDesc;
	mTexture->GetDesc(&texDesc);

	mTextureInfo.textureDesc = texDesc;

	mTextureInfo.device = device;
		
	return true;
}

bool Texture::Initialize(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
	DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags)
{

	mTextureInfo.device = device;

	mTextureInfo.textureDesc.Width = textureWidth;
	mTextureInfo.textureDesc.Height = textureHeight;
	mTextureInfo.textureDesc.MipLevels = MipLevels;
	mTextureInfo.textureDesc.ArraySize = ArraySize;
	mTextureInfo.textureDesc.Format = Format;
	mTextureInfo.textureDesc.Usage = Usage;
	mTextureInfo.textureDesc.BindFlags = BindFlags;
	mTextureInfo.textureDesc.CPUAccessFlags = CPUAccessFlags;
	mTextureInfo.textureDesc.MiscFlags = MiscFlags;

	mTextureInfo.bEnable4xMsaa = bEnable4xMsaa;
	mTextureInfo.m4xMsaaQuality = m4xMsaaQuality;

	//D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	//ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	//textureDesc.Width = textureWidth;
	//textureDesc.Height = textureHeight;
	//textureDesc.MipLevels = MipLevels;
	//textureDesc.ArraySize = ArraySize;
	//textureDesc.Format = Format;
	
	// Use 4X MSAA? --must match swap chain MSAA values.
	if (bEnable4xMsaa)
	{
		mTextureInfo.textureDesc.SampleDesc.Count = 4;
		mTextureInfo.textureDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		mTextureInfo.textureDesc.SampleDesc.Count = 1;
		mTextureInfo.textureDesc.SampleDesc.Quality = 0;
	}

	//textureDesc.SampleDesc.Count = Count;
	mTextureInfo.textureDesc.Usage = Usage;
	mTextureInfo.textureDesc.BindFlags = BindFlags;
	mTextureInfo.textureDesc.CPUAccessFlags = CPUAccessFlags;
	mTextureInfo.textureDesc.MiscFlags = MiscFlags;

	// Create the render target texture.
	result = device->CreateTexture2D(&mTextureInfo.textureDesc, NULL, &mTexture);

	if (FAILED(result))
	{
		return false;
	}

	/*
	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.	
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}
	*/

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = mTextureInfo.textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(mTexture, &shaderResourceViewDesc, &mshaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Texture::Shutdown()
{
	ReleaseCOM(mshaderResourceView);
	ReleaseCOM(mTexture)
	
	return;
}

/*
void Texture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);

	return;
}

void Texture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

*/
ID3D11ShaderResourceView* Texture::GetShaderResourceView()
{
	return mshaderResourceView;
}


void Texture::SetShaderResourceView(ID3D11ShaderResourceView* SRV)
{
	mshaderResourceView = SRV;
}

ID3D11Texture2D* Texture::GetTexture()
{
	return mTexture;
}
/*
void Texture::OnResize(UINT textureWidth, UINT textureHeight, ID3D11DeviceContext* d3dImmediateContext, ID3D11DepthStencilView* DepthStencilView)
{
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_renderTargetTexture);
	ReleaseCOM(m_shaderResourceView);

	Initialize(mTextureInfo.device, textureWidth, textureHeight, mTextureInfo.MipLevels, mTextureInfo.ArraySize, mTextureInfo.Format, mTextureInfo.bEnable4xMsaa,
		mTextureInfo.m4xMsaaQuality, mTextureInfo.Usage, mTextureInfo.BindFlags, mTextureInfo.CPUAccessFlags, mTextureInfo.MiscFlags);

	mTextureInfo.textureWidth = textureWidth;
	mTextureInfo.textureHeight = textureHeight;

	SetRenderTarget(d3dImmediateContext, DepthStencilView);
}
*/
