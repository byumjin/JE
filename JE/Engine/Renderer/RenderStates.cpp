//***************************************************************************************
// RenderStates.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::NormalRS = 0;

ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::NoCullRS = 0;
ID3D11RasterizerState* RenderStates::ShadowRS = 0;



ID3D11BlendState*      RenderStates::AlphaToCoverageBS = 0;
ID3D11BlendState*      RenderStates::TransparentBS = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//
	// NormalRS
	//
	D3D11_RASTERIZER_DESC NormalDesc;
	ZeroMemory(&NormalDesc, sizeof(D3D11_RASTERIZER_DESC));
	NormalDesc.FillMode = D3D11_FILL_SOLID;
	NormalDesc.CullMode = D3D11_CULL_BACK;
	NormalDesc.FrontCounterClockwise = false;
	NormalDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&NormalDesc, &NormalRS));

	//
	// ShadowRS
	//

	D3D11_RASTERIZER_DESC ShadowDesc;
	ZeroMemory(&ShadowDesc, sizeof(D3D11_RASTERIZER_DESC));
	ShadowDesc.FillMode = D3D11_FILL_SOLID;
	ShadowDesc.CullMode = D3D11_CULL_BACK;
	ShadowDesc.DepthBias = 100000;
	ShadowDesc.DepthBiasClamp = 10.0f;
	ShadowDesc.SlopeScaledDepthBias = 1.0;
	ShadowDesc.FrontCounterClockwise = false;
	ShadowDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&ShadowDesc, &ShadowRS));

	

	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	//
	// NoCullRS
	//
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	//
	// AlphaToCoverageBS
	//

	D3D11_BLEND_DESC alphaToCoverageDesc = { 0 };
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

	//
	// TransparentBS
	//

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));
}

void RenderStates::DestroyAll()
{
	ReleaseCOM(NormalRS);
	ReleaseCOM(ShadowRS);
	ReleaseCOM(WireframeRS);
	ReleaseCOM(NoCullRS);
	ReleaseCOM(AlphaToCoverageBS);
	ReleaseCOM(TransparentBS);
}