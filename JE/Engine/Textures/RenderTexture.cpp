#include "RenderTexture.h"

RenderTexture::RenderTexture()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;

	m_depthStencilTexture = 0;
	m_depthStencilView = 0;
}


RenderTexture::RenderTexture(const RenderTexture& other)
{
}


RenderTexture::~RenderTexture()
{
	Shutdown();
}

bool RenderTexture::Initialize(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
	DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags)
{

	mTextureInfo.device = device;
	mTextureInfo.textureDesc.Width = textureWidth;
	mTextureInfo.textureDesc.Height = textureHeight;
	mTextureInfo.textureDesc.MipLevels = MipLevels;
	mTextureInfo.textureDesc.ArraySize = ArraySize;
	mTextureInfo.textureDesc.Format = Format;
	mTextureInfo.bEnable4xMsaa = bEnable4xMsaa;
	mTextureInfo.m4xMsaaQuality = m4xMsaaQuality;
	mTextureInfo.textureDesc.Usage = Usage;
	mTextureInfo.textureDesc.BindFlags = BindFlags;
	mTextureInfo.textureDesc.CPUAccessFlags = CPUAccessFlags;
	mTextureInfo.textureDesc.MiscFlags = MiscFlags;

	//D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
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
	result = device->CreateTexture2D(&mTextureInfo.textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = mTextureInfo.textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = mTextureInfo.textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = -1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = textureWidth;
	depthStencilDesc.Height = textureHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DXGI_FORMAT_R32G8X24_TYPELESS;// DXGI_FORMAT_R32_TYPELESS; // Use a typeless type here so that it can be both depth-stencil and shader resource.
															// This will fail if we try a format like D32_FLOAT

															// Use 4X MSAA? --must match swap chain MSAA values.
	if (bEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;// | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;


	result = device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilTexture);
	if (FAILED(result))
	{
		return false;
	}
	

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	if (1 == depthStencilDesc.SampleDesc.Count) {
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	else {
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}

	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DXGI_FORMAT_D32_FLOAT_S8X24_UINT;// DXGI_FORMAT_D32_FLOAT;	// Make the view see this as D32_FLOAT instead of typeless
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	result = device->CreateDepthStencilView(m_depthStencilTexture, 0, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}



	return true;
}

void RenderTexture::Shutdown()
{
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_renderTargetTexture);
	ReleaseCOM(m_shaderResourceView);

	ReleaseCOM(m_depthStencilTexture);
	ReleaseCOM(m_depthStencilView);


	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;

	m_depthStencilTexture = 0;
	m_depthStencilView = 0;
	return;
}


void RenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	//deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	return;
}

void RenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
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
	//deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	return;
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView()
{
	return m_shaderResourceView;
}

ID3D11Texture2D* RenderTexture::GetRenderTargetTexture()
{
	return m_renderTargetTexture;
}

ID3D11RenderTargetView* RenderTexture::GetRenderTargetView()
{
	return  m_renderTargetView;
}

ID3D11DepthStencilView* RenderTexture::GetDepthStencilView()
{
	return  m_depthStencilView;
}

void RenderTexture::OnResize(UINT textureWidth, UINT textureHeight, ID3D11DeviceContext* d3dImmediateContext, ID3D11DepthStencilView* DepthStencilView)
{
	ReleaseCOM(m_renderTargetView);
	ReleaseCOM(m_renderTargetTexture);
	ReleaseCOM(m_shaderResourceView);
	ReleaseCOM(m_depthStencilTexture);
	ReleaseCOM(m_depthStencilView);

	Initialize(mTextureInfo.device, textureWidth, textureHeight, mTextureInfo.textureDesc.MipLevels, mTextureInfo.textureDesc.ArraySize, mTextureInfo.textureDesc.Format, mTextureInfo.bEnable4xMsaa,
		mTextureInfo.m4xMsaaQuality, mTextureInfo.textureDesc.Usage, mTextureInfo.textureDesc.BindFlags, mTextureInfo.textureDesc.CPUAccessFlags, mTextureInfo.textureDesc.MiscFlags);

	mTextureInfo.textureDesc.Width = textureWidth;
	mTextureInfo.textureDesc.Height = textureHeight;

	SetRenderTarget(d3dImmediateContext, DepthStencilView);
}



MultiRenderTexture::MultiRenderTexture()
{
	m_renderTargetTextureArray = NULL;
	m_renderTargetViewArray = NULL;
	m_shaderResourceViewArray = NULL;
	mTextureInfo = NULL;
}
MultiRenderTexture::MultiRenderTexture(const MultiRenderTexture&)
{

}
MultiRenderTexture::~MultiRenderTexture()
{
	Shutdown();
}

HRESULT MultiRenderTexture::CreateRenderTexture(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
	DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags, UINT Num)
{

	mTextureInfo[Num].device = device;
	mTextureInfo[Num].textureDesc.Width = textureWidth;
	mTextureInfo[Num].textureDesc.Height = textureHeight;
	mTextureInfo[Num].textureDesc.MipLevels = MipLevels;
	mTextureInfo[Num].textureDesc.ArraySize = ArraySize;
	mTextureInfo[Num].textureDesc.Format = Format;
	mTextureInfo[Num].bEnable4xMsaa = bEnable4xMsaa;
	mTextureInfo[Num].m4xMsaaQuality = m4xMsaaQuality;
	mTextureInfo[Num].textureDesc.Usage = Usage;
	mTextureInfo[Num].textureDesc.BindFlags = BindFlags;
	mTextureInfo[Num].textureDesc.CPUAccessFlags = CPUAccessFlags;
	mTextureInfo[Num].textureDesc.MiscFlags = MiscFlags;
	mTextureInfo[Num].TextureNum = Num;

	//D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	//ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	//textureDesc.Width = mTextureInfo[Num].textureDesc.Width;
	//textureDesc.Height = mTextureInfo[Num].textureDesc.Height;
	//textureDesc.MipLevels = mTextureInfo[Num].textureDesc.MipLevels;
	//textureDesc.ArraySize = mTextureInfo[Num].textureDesc.ArraySize;
	//textureDesc.Format = mTextureInfo[Num].textureDesc.Format;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mTextureInfo[Num].bEnable4xMsaa)
	{
		mTextureInfo[Num].textureDesc.SampleDesc.Count = 4;
		mTextureInfo[Num].textureDesc.SampleDesc.Quality = mTextureInfo[Num].m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		mTextureInfo[Num].textureDesc.SampleDesc.Count = 1;
		mTextureInfo[Num].textureDesc.SampleDesc.Quality = 0;
	}

	//textureDesc.SampleDesc.Count = Count;
	mTextureInfo[Num].textureDesc.Usage = Usage;
	mTextureInfo[Num].textureDesc.BindFlags = BindFlags;
	mTextureInfo[Num].textureDesc.CPUAccessFlags = CPUAccessFlags;
	mTextureInfo[Num].textureDesc.MiscFlags =  MiscFlags;

	// Create the render target texture.
	result = device->CreateTexture2D(&mTextureInfo[Num].textureDesc, NULL, &m_renderTargetTextureArray[Num]);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = mTextureInfo[Num].textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = device->CreateRenderTargetView(m_renderTargetTextureArray[Num], &renderTargetViewDesc, &m_renderTargetViewArray[Num]);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = mTextureInfo[Num].textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = device->CreateShaderResourceView(m_renderTargetTextureArray[Num], &shaderResourceViewDesc, &m_shaderResourceViewArray[Num]);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool MultiRenderTexture::Initialize(ID3D11Device* device, UINT textureNum)
{
	mInum = textureNum;
	mTextureInfo = new TextureInfo[mInum];
	m_renderTargetTextureArray = new ID3D11Texture2D*[mInum];
	m_renderTargetViewArray = new ID3D11RenderTargetView*[mInum];
	m_shaderResourceViewArray = new ID3D11ShaderResourceView*[mInum];
	
	return true;	
}

bool MultiRenderTexture::Reset()
{	
	m_renderTargetTextureArray = new ID3D11Texture2D*[mInum];
	m_renderTargetViewArray = new ID3D11RenderTargetView*[mInum];
	m_shaderResourceViewArray = new ID3D11ShaderResourceView*[mInum];

	return true;
}

void MultiRenderTexture::Shutdown()
{
	if (m_shaderResourceViewArray)
	{

		for (UINT i = 0; i < mInum; i++)
		{
			ReleaseCOM(m_shaderResourceViewArray[i]);
		}

		delete[] m_shaderResourceViewArray;
		//m_shaderResourceViewArray = NULL;
	}

	if (m_renderTargetViewArray)
	{

		for (UINT i = 0; i < mInum; i++)
		{
			ReleaseCOM(m_renderTargetViewArray[i]);
		}

		delete[] m_renderTargetViewArray;
		//m_renderTargetViewArray = NULL;
	}

	if (m_renderTargetTextureArray)
	{

		for (UINT i = 0; i < mInum; i++)
		{
			ReleaseCOM(m_renderTargetTextureArray[i]);
		}
		
		delete [] m_renderTargetTextureArray;
		//m_renderTargetTextureArray = NULL;
	}

	if (mTextureInfo)
	{
		delete[] mTextureInfo;
		//mTextureInfo = NULL;
	}

	return;
}

void MultiRenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	deviceContext->OMSetRenderTargets(mInum, m_renderTargetViewArray, depthStencilView);
	
	return;
}

void MultiRenderTexture::UnbindDepthStencil(ID3D11DeviceContext* deviceContext)
{
	deviceContext->OMSetRenderTargets(mInum, m_renderTargetViewArray, NULL);

	return;
}

void MultiRenderTexture::ReleaseRenderTarget(ID3D11DeviceContext* deviceContext)
{	
	ID3D11RenderTargetView* pNullRTV[] = { NULL, NULL, NULL, NULL, NULL};
	deviceContext->OMSetRenderTargets(mInum, pNullRTV, NULL);	

	ID3D11ShaderResourceView *pSRV[] = { NULL, NULL, NULL, NULL, NULL};
	deviceContext->PSSetShaderResources(0, mInum, pSRV);


}

void MultiRenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	for (UINT i = 0; i < mInum; i++)
	{
		//For skyBox
		//if(i == 0)
		//	continue;
		//else
			deviceContext->ClearRenderTargetView(m_renderTargetViewArray[i], color);
	}
	
	//For skyBox
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

ID3D11ShaderResourceView* MultiRenderTexture::GetShaderResourceView(int Num)
{
	return m_shaderResourceViewArray[Num];
}

ID3D11RenderTargetView* MultiRenderTexture::GetRenderTargetView(int Num)
{
	return m_renderTargetViewArray[Num];
}

void MultiRenderTexture::OnResize(UINT textureWidth, UINT textureHeight, ID3D11DeviceContext* d3dImmediateContext, ID3D11DepthStencilView* DepthStencilView)
{
	if (m_shaderResourceViewArray)
	{

		for (UINT i = 0; i < mInum; i++)
		{
			(ID3D11ShaderResourceView*)m_shaderResourceViewArray[i]->Release();
		}

		delete[] m_shaderResourceViewArray;
	}

	if (m_renderTargetViewArray)
	{

		for (UINT i = 0; i < mInum; i++)
		{
			(ID3D11RenderTargetView*)m_renderTargetViewArray[i]->Release();
		}

		delete[] m_renderTargetViewArray;
	}

	if (m_renderTargetTextureArray)
	{

		for (UINT i = 0; i < mInum; i++)
		{
			(ID3D11Texture2D*)m_renderTargetTextureArray[i]->Release();
		}

		delete[] m_renderTargetTextureArray;
	}


	if (mTextureInfo)
	{
		Reset();
		//Initialize(mTextureInfo[0].device, mInum);

		for (UINT i = 0; i < mInum; i++)
		{
			CreateRenderTexture(mTextureInfo[i].device, textureWidth, textureHeight, mTextureInfo[i].textureDesc.MipLevels, mTextureInfo[i].textureDesc.ArraySize, mTextureInfo[i].textureDesc.Format, mTextureInfo[i].bEnable4xMsaa,
				mTextureInfo[i].m4xMsaaQuality, mTextureInfo[i].textureDesc.Usage, mTextureInfo[i].textureDesc.BindFlags, mTextureInfo[i].textureDesc.CPUAccessFlags, mTextureInfo[i].textureDesc.MiscFlags, mTextureInfo[i].TextureNum);

			mTextureInfo[i].textureDesc.Width = textureWidth;
			mTextureInfo[i].textureDesc.Height = textureHeight;
		}


		SetRenderTarget(d3dImmediateContext, DepthStencilView);
	}
	else
	{
		return;
	}
}

RenderTextureManger::RenderTextureManger()
{

}

RenderTextureManger::~RenderTextureManger()
{
	for (UINT i = 0; i < mRT_List.size(); i++)
	{
		delete mRT_List.at(i);
	}
}

HRESULT RenderTextureManger::LoadRT(ID3D11Device* device, UINT textureWidth, UINT textureHeight, UINT MipLevels, UINT ArraySize,
	DXGI_FORMAT Format, bool bEnable4xMsaa, UINT m4xMsaaQuality, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags)
{
	RenderTexture *mRT = new RenderTexture();
	mRT->Initialize(device, textureWidth, textureHeight, MipLevels, ArraySize,
		Format, bEnable4xMsaa, m4xMsaaQuality, Usage, BindFlags, CPUAccessFlags, MiscFlags);
	
	mRT_List.push_back(mRT);

	//delete mRT;

	return true;
}


void RenderTextureManger::OnResize(UINT width, UINT height, ID3D11DeviceContext* pd3dImmediateContext, ID3D11DepthStencilView* pDepthStencilView)
{
	if (mRT_List.size() > 0)
	{
		for (UINT i = 0; i < mRT_List.size(); i++)
		{
			mRT_List.at(i)->OnResize(width, height, pd3dImmediateContext, pDepthStencilView);
		}
	}
}