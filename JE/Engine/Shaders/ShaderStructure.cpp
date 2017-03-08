#include "ShaderStructure.h"
#include "Effects.h"

#pragma region InputLayoutDesc

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::VertexInputDesc[6] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::BRDFL_VID[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },	
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Line_VID[1] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }	
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::GIZMO_VID[4] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::VertexInputDesc = 0;
ID3D11InputLayout* InputLayouts::BRDFL_VID = 0;
ID3D11InputLayout* InputLayouts::Line_VID = 0;
ID3D11InputLayout* InputLayouts::GIZMO_VID = 0;

void InputLayouts::InitAll(ID3D11Device* device, Effects* pEffects)
{	
	D3DX11_PASS_DESC passDesc;
	
	pEffects->BasicFX->mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT(device->CreateInputLayout(InputLayoutDesc::VertexInputDesc, 6, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &VertexInputDesc));

	pEffects->BRDFLightingFX->mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT(device->CreateInputLayout(InputLayoutDesc::BRDFL_VID, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &BRDFL_VID));

	pEffects->LineDrawFx->mTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT(device->CreateInputLayout(InputLayoutDesc::Line_VID, 1, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Line_VID));

	pEffects->GizmoDrawFx->TransTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT(device->CreateInputLayout(InputLayoutDesc::GIZMO_VID, 4, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &GIZMO_VID));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(VertexInputDesc);
	ReleaseCOM(BRDFL_VID);
	ReleaseCOM(Line_VID);
	ReleaseCOM(GIZMO_VID);
}

#pragma endregion