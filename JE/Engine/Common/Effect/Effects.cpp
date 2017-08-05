//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

//#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)



#pragma region Effect
Effect::Effect(ID3D11Device* device, const LPCTSTR/*const std::wstring&*/ filename)
: mFX(0)
{
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* compilationMsgs = 0;
	
	HR(D3DX11CompileEffectFromFile(filename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlags, NULL,
		device, &mFX, &compilationMsgs));	
}

Effect::~Effect()
{	
	ReleaseCOM(mTech);
	ReleaseCOM(mFX);	
}

void Effect::SetAllVariables()
{
	
}

#pragma endregion

#pragma region LineDrawEffect

LineDrawEffect::LineDrawEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 0;

	mTech = mFX->GetTechniqueByName("LineTech");
	mbSelected = mFX->GetVariableByName("bSelected")->AsScalar();	
	
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	
}

LineDrawEffect::~LineDrawEffect()
{

}

void LineDrawEffect::SetAllVariables(CXMMATRIX paraWVP, bool paraSelected)
{
	SetMatrix(paraWVP, mWorldViewProj);
	SetScalar(paraSelected, mbSelected);
	
};

#pragma endregion

#pragma region OutLineDrawEffect
OutLineDrawEffect::OutLineDrawEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 0;
	//mTech = mFX->GetTechniqueByName("OutLineTech");

	FillSilhouetteTech = mFX->GetTechniqueByName("FillSilhouetteTech");
	OutLineTech = mFX->GetTechniqueByName("OutLineTech");
	


	
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	

	mStencilTexture = mFX->GetVariableByName("StencilTexture")->AsShaderResource();

	mTextureSize = mFX->GetVariableByName("gTextureSize")->AsVector();

}

OutLineDrawEffect::~OutLineDrawEffect()
{
	ReleaseCOM(FillSilhouetteTech);
	ReleaseCOM(OutLineTech);
}

void OutLineDrawEffect::SetAllVariables(CXMMATRIX paraViewProj, CXMMATRIX paraWVP, ID3D11ShaderResourceView* paraStencilTexture, XMFLOAT2 paraTextureSize)
{
	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraWVP, mWorldViewProj);

	SetTexture(paraStencilTexture, mStencilTexture);
	
	SetVector2(paraTextureSize, mTextureSize);
	

};
#pragma endregion

#pragma region GizmoDrawEffect

GizmoDrawEffect::GizmoDrawEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 0;

	mTech = mFX->GetTechniqueByName("AxisTech");
	TransTech = mFX->GetTechniqueByName("TransTech");


	mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();	
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	

	mOriginTranslation = mFX->GetVariableByName("gOriginTranslation")->AsMatrix();
	mLength = mFX->GetVariableByName("gLength")->AsScalar();
	mSelectedAxis = mFX->GetVariableByName("gSelectedAxis")->AsScalar();
}

GizmoDrawEffect::~GizmoDrawEffect()
{
	ReleaseCOM(TransTech);	
}

void GizmoDrawEffect::SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraWVP, CXMMATRIX paraOriginTranslation, float paraLength)
{
	SetMatrix(paraWorld, mWorld);	
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraOriginTranslation, mOriginTranslation);
	SetScalar(paraLength, mLength);	
}

void GizmoDrawEffect::SetAxisTechVariables(CXMMATRIX paraWorld, CXMMATRIX paraWVP, CXMMATRIX paraOriginTranslation, float paraLength)
{
	SetMatrix(paraWorld, mWorld);

	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraOriginTranslation, mOriginTranslation);
	SetScalar(paraLength, mLength);
}

void GizmoDrawEffect::SetTransTechVariables(CXMMATRIX paraWorld, CXMMATRIX paraWVP, CXMMATRIX paraOriginTranslation, float paraLength, int paraSelectedAxis)
{
	SetMatrix(paraWorld, mWorld);
	
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraOriginTranslation, mOriginTranslation);
	SetScalar(paraLength, mLength);
	SetScalar((float)paraSelectedAxis, mSelectedAxis);
}


#pragma endregion

#pragma region BasicMaterialEffect
BasicMaterialEffect::BasicMaterialEffect(ID3D11Device* device, const LPCWSTR filename)
	: Effect(device, filename)
{	
	m_TextureParameterCount = 3;

	mTech = mFX->GetTechniqueByName("BasicMaterialTech");
	SkyBoxTech = mFX->GetTechniqueByName("SkyBoxTech");


	mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mWorldView = mFX->GetVariableByName("gWorldView")->AsMatrix();
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	
	mInvView = mFX->GetVariableByName("gInvView")->AsMatrix();
	mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mInvViewProj = mFX->GetVariableByName("gInvViewProj")->AsMatrix();
	mInvProj = mFX->GetVariableByName("gInvProj")->AsMatrix();

	mEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	mTime = mFX->GetVariableByName("gTime")->AsScalar();


	mDiffuseMap		  = mFX->GetVariableByName("DiffuseMap")->AsShaderResource();
	mSpecularMap		= mFX->GetVariableByName("SpecularMap")->AsShaderResource();
	mNormalMap		  = mFX->GetVariableByName("NormalMap")->AsShaderResource();

	m_SkyBoxCubeMap = mFX->GetVariableByName("SkyBoxCubeMap")->AsShaderResource();

	

	mbSelected = mFX->GetVariableByName("bSelected")->AsScalar();
	
}

void BasicMaterialEffect::SetSkyBoxVariables(CXMMATRIX paraWorld, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
	XMFLOAT3 paraEyePosW, float paraTime, ID3D11ShaderResourceView* paraSkyBoxCubeMap)
{

	SetMatrix(paraWorld, mWorld);

	

	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraWIT, mWorldInvTranspose);
	SetMatrix(paraIVP, mInvViewProj);
	SetMatrix(paraIP, mInvProj);

	SetVector(paraEyePosW, mEyePosW);
	SetScalar(paraTime, mTime);

	SetTexture(paraSkyBoxCubeMap, m_SkyBoxCubeMap);
}

void BasicMaterialEffect::SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraWorldView, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraIV, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
	XMFLOAT3 paraEyePosW, float paraTime, bool parabSelected, ID3D11ShaderResourceView* DiffuseTexture, ID3D11ShaderResourceView* SpecularTexture,
	ID3D11ShaderResourceView* NormalTexture)
{
	//Effect::SetAllVariables(paraWorld, paraViewProj, paraWVP, paraWIT, paraIVP, paraIP, paraEyePosW, paraTime);
	
	SetMatrix(paraWorld, mWorld);
	SetMatrix(paraWorldView, mWorldView);
	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraWVP, mWorldViewProj);

	SetMatrix(paraIV, mInvView);
	

	SetMatrix(paraWIT, mWorldInvTranspose);
	SetMatrix(paraIVP, mInvViewProj);
	SetMatrix(paraIP, mInvProj);

	SetVector(paraEyePosW, mEyePosW);
	SetScalar(paraTime, mTime);
	
	//SetScalar(paraEnvCubeMapCounter, m_iEnvCubeCounter);

	SetTexture(DiffuseTexture, mDiffuseMap);
	SetTexture(SpecularTexture, mSpecularMap);
	SetTexture(NormalTexture, mNormalMap);
	
	SetBool(parabSelected, mbSelected);

	//SetVector(paraWorldCenterPosition, m_WorldCenterPosition);
}

/*
void BasicMaterialEffect::SetAllEnvTextures(ID3D11ShaderResourceView* EnvCubeMap ,UINT Count)
{
	if(Count == 0)
		SetTexture(EnvCubeMap, m_EnvCubeMap01);
	if (Count == 1)
		SetTexture(EnvCubeMap, m_EnvCubeMap02);
	if (Count == 2)
		SetTexture(EnvCubeMap, m_EnvCubeMap03);
	if (Count == 3)
		SetTexture(EnvCubeMap, m_EnvCubeMap04);
	if (Count == 4)
		SetTexture(EnvCubeMap, m_EnvCubeMap05);
	if (Count == 5)
		SetTexture(EnvCubeMap, m_EnvCubeMap06);
}
*/

BasicMaterialEffect::~BasicMaterialEffect()
{
	ReleaseCOM(SkyBoxTech);
}
#pragma endregion

#pragma region BRDFLightingEffect
BRDFLightingEffect::BRDFLightingEffect(ID3D11Device* device, const LPCWSTR filename)
: Effect(device, filename)
{
	m_TextureParameterCount = 0;

	mTech = mFX->GetTechniqueByName("DirLightsTech");

	mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mInvViewProj = mFX->GetVariableByName("gInvViewProj")->AsMatrix();
	mInvProj = mFX->GetVariableByName("gInvProj")->AsMatrix();

	m_ShadowVP = mFX->GetVariableByName("gShadowVP")->AsMatrix();

	m_DepthInvProj = mFX->GetVariableByName("gInvDepthViewProj")->AsMatrix();

	mEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	mTime = mFX->GetVariableByName("gTime")->AsScalar();

	DirLights = mFX->GetVariableByName("gDirLight");	
	
	mBasicColorTexture = mFX->GetVariableByName("BasicColorTexture")->AsShaderResource();
	m_SpecularTexture = mFX->GetVariableByName("SpecularTexture")->AsShaderResource();
	mNormalTexture = mFX->GetVariableByName("NormalTexture")->AsShaderResource();	

	m_ReflectionTexture = mFX->GetVariableByName("ReflectionTexture")->AsShaderResource();

	mDepthInfoTexture = mFX->GetVariableByName("DepthInfoTexture")->AsShaderResource();

	m_BRDFLUTTexture = mFX->GetVariableByName("BRDFLUTTexture")->AsShaderResource();

	m_ShadowTexture = mFX->GetVariableByName("ShadowTexture")->AsShaderResource();

	
}

BRDFLightingEffect::~BRDFLightingEffect()
{
	
}

void BRDFLightingEffect::SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
	XMFLOAT3 paraEyePosW, float paraTime, CXMMATRIX paraShadowVP, CXMMATRIX paraDepthIVP,
	ID3D11ShaderResourceView* paraBasicColorTexture,
	ID3D11ShaderResourceView* param_SpecularTexture,
	ID3D11ShaderResourceView* paraNormalTexture,
	ID3D11ShaderResourceView* paraDepthInfoTexture,
	ID3D11ShaderResourceView* ReflectionTexture,
	ID3D11ShaderResourceView* paraLUTTexture,
	ID3D11ShaderResourceView* paraShadowTexture
	)
{
	
	SetMatrix(paraWorld, mWorld);
	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraWIT, mWorldInvTranspose);
	SetMatrix(paraIVP, mInvViewProj);
	SetMatrix(paraIP, mInvProj);

	SetMatrix(paraShadowVP, m_ShadowVP);

	SetMatrix(paraDepthIVP, m_DepthInvProj);

	SetVector(paraEyePosW, mEyePosW);
	SetScalar(paraTime, mTime);

	SetTexture(paraBasicColorTexture, mBasicColorTexture);
	SetTexture(param_SpecularTexture, m_SpecularTexture);
	SetTexture(paraNormalTexture, mNormalTexture);	
	SetTexture(ReflectionTexture, m_ReflectionTexture);


	SetTexture(paraDepthInfoTexture, mDepthInfoTexture);
	SetTexture(paraLUTTexture, m_BRDFLUTTexture);

	SetTexture(paraShadowTexture, m_ShadowTexture);

	
}
#pragma endregion

#pragma region DebugWindowEffect


DebugWindowEffect::DebugWindowEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 0;

	//mTech = mFX->GetTechniqueByName("LineTech");
	mBasicTexture = mFX->GetVariableByName("BasicTexture")->AsShaderResource();
	m_SpecularTexture = mFX->GetVariableByName("SpecularTexture")->AsShaderResource();
	mNormalTexture = mFX->GetVariableByName("NormalTexture")->AsShaderResource();
	mDepthTexture = mFX->GetVariableByName("DepthTexture")->AsShaderResource();
	mAOTexture = mFX->GetVariableByName("AOTexture")->AsShaderResource();
	m_UVTexture = mFX->GetVariableByName("UVTexture")->AsShaderResource();
	m_EnvTexture = mFX->GetVariableByName("EnvTexture")->AsShaderResource();
	m_BloomTexture = mFX->GetVariableByName("BloomTexture")->AsShaderResource();
	m_ShadowTexture = mFX->GetVariableByName("ShadowTexture")->AsShaderResource();
	m_LensFlareTexture = mFX->GetVariableByName("LensFlareTexture")->AsShaderResource();

	mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mInvViewProj = mFX->GetVariableByName("gInvViewProj")->AsMatrix();
	mInvProj = mFX->GetVariableByName("gInvProj")->AsMatrix();
	
	m_InvDepthViewProj = mFX->GetVariableByName("gInvDepthViewProj")->AsMatrix();

	mEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	mTime = mFX->GetVariableByName("gTime")->AsScalar();

	BasicTech = mFX->GetTechniqueByName("BasicTech");
	SpecTech = mFX->GetTechniqueByName("SpecTech");
	NormalTech = mFX->GetTechniqueByName("NormalTech");
	RoughnessTech = mFX->GetTechniqueByName("RoughnessTech");
	MetallicTech = mFX->GetTechniqueByName("MetallicTech");
	WorldPositionTech = mFX->GetTechniqueByName("WorldPositionTech");
	DepthTech = mFX->GetTechniqueByName("DepthTech");
	SelectedTech = mFX->GetTechniqueByName("SelectedTech");
	OpacityTech = mFX->GetTechniqueByName("OpacityTech");

	LensFlareTech = mFX->GetTechniqueByName("LensFlareTech");

	AOTech = mFX->GetTechniqueByName("AOTech");
	EnvTech = mFX->GetTechniqueByName("EnvTech");

	BloomTech = mFX->GetTechniqueByName("BloomTech");

	Custom01Tech = mFX->GetTechniqueByName("Custom01Tech");
	//Custom02Tech = mFX->GetTechniqueByName("Custom02Tech");


}

DebugWindowEffect::~DebugWindowEffect()
{
	ReleaseCOM(BasicTech);
	ReleaseCOM(SpecTech);
	ReleaseCOM(NormalTech);
	ReleaseCOM(RoughnessTech);
	ReleaseCOM(MetallicTech);
	ReleaseCOM(WorldPositionTech);
	ReleaseCOM(DepthTech);
	ReleaseCOM(SelectedTech);
	ReleaseCOM(OpacityTech);
	ReleaseCOM(AOTech);
	ReleaseCOM(EnvTech);
	ReleaseCOM(BloomTech);

	
	ReleaseCOM(Custom01Tech);

	ReleaseCOM(LensFlareTech);
}

void DebugWindowEffect::SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
	XMFLOAT3 paraEyePosW, float paraTime, ID3D11ShaderResourceView* paraBasicTexture, ID3D11ShaderResourceView* paraSpecularTexture, ID3D11ShaderResourceView* paraNormalTexture, ID3D11ShaderResourceView* paraDepthTexture,
	ID3D11ShaderResourceView* paraAOTexture, ID3D11ShaderResourceView* paraUVTexture, ID3D11ShaderResourceView* paraEnvTexture, ID3D11ShaderResourceView* paraBloomTexture, ID3D11ShaderResourceView* paraLensFlareTexture,
	ID3D11ShaderResourceView* paraShadowTexture,
	CXMMATRIX paraInvDepthViewProj)
{
	SetMatrix(paraWorld, mWorld);
	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraWIT, mWorldInvTranspose);
	SetMatrix(paraIVP, mInvViewProj);
	SetMatrix(paraIP, mInvProj);

	SetMatrix(paraInvDepthViewProj, m_InvDepthViewProj);

	SetVector(paraEyePosW, mEyePosW);
	SetScalar(paraTime, mTime);

	SetTexture(paraBasicTexture, mBasicTexture);
	SetTexture(paraSpecularTexture, m_SpecularTexture);
	SetTexture(paraNormalTexture, mNormalTexture);
	SetTexture(paraDepthTexture, mDepthTexture);
	SetTexture(paraAOTexture, mAOTexture);
	SetTexture(paraUVTexture, m_UVTexture);

	SetTexture(paraLensFlareTexture, m_LensFlareTexture);

	SetTexture(paraEnvTexture, m_EnvTexture);	
	SetTexture(paraBloomTexture, m_BloomTexture);
	SetTexture(paraShadowTexture, m_ShadowTexture);
};
#pragma endregion





#pragma region UISpriteffect

UISpriteEffect::UISpriteEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 0;
	m_UISpriteMap = mFX->GetVariableByName("UISpriteMap")->AsShaderResource();
		
	mEyePosW = mFX->GetVariableByName("gEyeWorldPos")->AsVector();

	mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	
	UISpriteTech = mFX->GetTechniqueByName("UISpriteTech");

	m_bSelected = mFX->GetVariableByName("bSelected")->AsScalar();
}

UISpriteEffect::~UISpriteEffect()
{
	ReleaseCOM(UISpriteTech);
}

void UISpriteEffect::SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraPV, CXMMATRIX paraWVP, XMFLOAT3 paraEyeWorldPos, bool paraSelected, ID3D11ShaderResourceView* paraUISpriteTexture)
{
	SetBool(paraSelected, m_bSelected);
	SetMatrix(paraWorld, mWorld);	
	SetMatrix(paraPV, mViewProj);
	SetMatrix(paraWVP, mWorldViewProj);
	SetVector(paraEyeWorldPos, mEyePosW);	
	SetTexture(paraUISpriteTexture, m_UISpriteMap);
};
#pragma endregion


#pragma region ReflectionActorEffect
ReflectionActorEffect::ReflectionActorEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;

	m_DiffuseMap = mFX->GetVariableByName("DiffuseMap")->AsShaderResource();	
	m_SkyBoxCubeMap = mFX->GetVariableByName("SkyBoxCubeMap")->AsShaderResource();
	
	mEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();

	
	m_NormalMap = mFX->GetVariableByName("NormalMap")->AsShaderResource();
	m_SpecularMap = mFX->GetVariableByName("SpecularMap")->AsShaderResource();

	m_EnvCubeMap01 = mFX->GetVariableByName("EnvCubeMap01")->AsShaderResource();
	m_EnvCubeMap02 = mFX->GetVariableByName("EnvCubeMap02")->AsShaderResource();
	m_EnvCubeMap03 = mFX->GetVariableByName("EnvCubeMap03")->AsShaderResource();
	m_EnvCubeMap04 = mFX->GetVariableByName("EnvCubeMap04")->AsShaderResource();
	m_EnvCubeMap05 = mFX->GetVariableByName("EnvCubeMap05")->AsShaderResource();
	m_EnvCubeMap06 = mFX->GetVariableByName("EnvCubeMap06")->AsShaderResource();



	m_iEnvCubeCounter = mFX->GetVariableByName("gEnvCubeMapCount")->AsScalar();
	m_WorldCenterPosition = mFX->GetVariableByName("gWorldCenterPosition")->AsVector();

	
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	m_InvDepthViewProj = mFX->GetVariableByName("gInvDepthViewProj")->AsMatrix();

	ReflectionActorTech = mFX->GetTechniqueByName("ReflectionActorTech");
	SkyBoxTech = mFX->GetTechniqueByName("SkyBoxTech");
	//DrawObjTech = mFX->GetTechniqueByName("DrawObjTech");

	CompositionCubeMapTech = mFX->GetTechniqueByName("CompositionCubeMapTech");

}

ReflectionActorEffect::~ReflectionActorEffect()
{
	ReleaseCOM(ReflectionActorTech);
	ReleaseCOM(CompositionCubeMapTech);
	ReleaseCOM(SkyBoxTech);
}

void ReflectionActorEffect::SetDDNullTextures()
{
	SetTexture(NULL, m_DiffuseMap);
	SetTexture(NULL, m_SkyBoxCubeMap);
	
}

void ReflectionActorEffect::SetNulltoTextures()
{
	SetTexture(NULL, m_DiffuseMap);
	SetTexture(NULL, m_SkyBoxCubeMap);

	//SetTexture(NULL, m_DepthInfoMap);
	
	SetTexture(NULL, m_NormalMap);
	SetTexture(NULL, m_SpecularMap);
	
	SetTexture(NULL, m_EnvCubeMap01);
	SetTexture(NULL, m_EnvCubeMap02);
	SetTexture(NULL, m_EnvCubeMap03);
	SetTexture(NULL, m_EnvCubeMap04);
	SetTexture(NULL, m_EnvCubeMap05);
	SetTexture(NULL, m_EnvCubeMap06);
}

void ReflectionActorEffect::SetColorVariables(CXMMATRIX paraWVP, ID3D11ShaderResourceView* paraDiffuseTexture)
{
	SetMatrix(paraWVP, mWorldViewProj);
	SetTexture(paraDiffuseTexture, m_DiffuseMap);	
}

void ReflectionActorEffect::SetAllEnvTextures(ID3D11ShaderResourceView* EnvCubeMap, UINT EnvCubeCounter)
{
	if (EnvCubeCounter == 0)
		SetTexture(EnvCubeMap, m_EnvCubeMap01);
	if (EnvCubeCounter == 1)
		SetTexture(EnvCubeMap, m_EnvCubeMap02);
	if (EnvCubeCounter == 2)
		SetTexture(EnvCubeMap, m_EnvCubeMap03);
	if (EnvCubeCounter == 3)
		SetTexture(EnvCubeMap, m_EnvCubeMap04);
	if (EnvCubeCounter == 4)
		SetTexture(EnvCubeMap, m_EnvCubeMap05);
	if (EnvCubeCounter == 5)
		SetTexture(EnvCubeMap, m_EnvCubeMap06);
}

void ReflectionActorEffect::SetCubeVariables(CXMMATRIX paraWorldMat, CXMMATRIX paraWVP, CXMMATRIX paraInvVP,
	ID3D11ShaderResourceView* paraWorldNormalMap, ID3D11ShaderResourceView* paraSpecualarMap,
	XMFLOAT3 paraEyePosW, float paraEnvCubeCounter, XMFLOAT3 paraObjWorldPosition)
{
	SetMatrix(paraWorldMat, mWorld);
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraInvVP, m_InvDepthViewProj);

	SetTexture(paraWorldNormalMap, m_NormalMap);
	SetTexture(paraSpecualarMap, m_SpecularMap);
	
	SetVector(paraEyePosW, mEyePosW);
	SetScalar(paraEnvCubeCounter, m_iEnvCubeCounter);
	SetVector(paraObjWorldPosition, m_WorldCenterPosition);
	

	
}

//void ReflectionActorEffect::SetSkyBoxVariables(CXMMATRIX paraWVP, CXMMATRIX paraWorldMat, ID3D11ShaderResourceView* paraSkyBoxTexture, XMFLOAT3 paraWorldCenterPosition,
//	XMFLOAT3 gEyePos, ID3D11ShaderResourceView* WPSRV, ID3D11ShaderResourceView* WNSRV, CXMMATRIX paraInvDepthViewProj)

void ReflectionActorEffect::SetSkyBoxVariables(CXMMATRIX paraWVP, CXMMATRIX paraWorldMat, ID3D11ShaderResourceView* paraSkyBoxTexture)
{
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraWorldMat, mWorld);
	SetTexture(paraSkyBoxTexture, m_SkyBoxCubeMap);

	//SetTexture(WPSRV, m_WPSRV);
	//SetTexture(WNSRV, m_WNSRV);

	//SetVector(gEyePos, mEyePosW);

	//SetMatrix(paraInvDepthViewProj, m_InvDepthViewProj);

	//SetVector(paraWorldCenterPosition, m_WorldCenterPosition);
}

#pragma endregion

#pragma region SkyBoxEffect
SkyBoxEffect::SkyBoxEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;
	
	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_EnvCubeMap = mFX->GetVariableByName("EnvCubeMap")->AsShaderResource();
	
	SkyBoxTech = mFX->GetTechniqueByName("SkyBoxTech");
}

SkyBoxEffect::~SkyBoxEffect()
{
	ReleaseCOM(SkyBoxTech);
}

void SkyBoxEffect::SetNulltoTextures()
{

	SetTexture(NULL, m_EnvCubeMap);
}

void SkyBoxEffect::SetAllVariables(CXMMATRIX paraWVP, ID3D11ShaderResourceView* paraEnvCubeMap)
{
	SetMatrix(paraWVP, mWorldViewProj);
	SetTexture(paraEnvCubeMap, m_EnvCubeMap);
}
#pragma endregion


#pragma region BloomEffect

BloomEffect::BloomEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;

	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();	
	m_SceneMap = mFX->GetVariableByName("SceneMap")->AsShaderResource();
	m_BasicColorMap = mFX->GetVariableByName("BasicColorMap")->AsShaderResource();

	m_factor01 = mFX->GetVariableByName("factor01")->AsScalar();
	m_factor02 = mFX->GetVariableByName("factor02")->AsScalar();
	m_factor03 = mFX->GetVariableByName("factor03")->AsScalar();
	m_factor04 = mFX->GetVariableByName("factor04")->AsScalar();
	m_factor05 = mFX->GetVariableByName("factor05")->AsScalar();
	m_factor06 = mFX->GetVariableByName("factor06")->AsScalar();
	m_factor07 = mFX->GetVariableByName("factor07")->AsScalar();




	BloomTech = mFX->GetTechniqueByName("BloomTech");

	GaussianVerticalTech = mFX->GetTechniqueByName("GaussianVerticalTech");
	GaussianHeightTech = mFX->GetTechniqueByName("GaussianHeightTech");
}

BloomEffect::~BloomEffect()
{
	ReleaseCOM(BloomTech);
	ReleaseCOM(GaussianVerticalTech);
	ReleaseCOM(GaussianHeightTech);
}

void BloomEffect::SetNulltoTextures()
{
	SetTexture(NULL, m_SceneMap);
	SetTexture(NULL, m_BasicColorMap);
}

void BloomEffect::SetAllVariables(CXMMATRIX paraViewProj,  ID3D11ShaderResourceView* paraSceneMap, ID3D11ShaderResourceView* paraBasicColorMap, float* pFactor)
{	
	SetMatrix(paraViewProj, mViewProj);
	SetTexture(paraSceneMap, m_SceneMap);
	SetTexture(paraBasicColorMap, m_BasicColorMap);

	SetScalar(pFactor[0], m_factor01);
	SetScalar(pFactor[1], m_factor02);
	SetScalar(pFactor[2], m_factor03);
	SetScalar(pFactor[3], m_factor04);
	SetScalar(pFactor[4], m_factor05);
	SetScalar(pFactor[5], m_factor06);
	SetScalar(pFactor[6], m_factor07);


}


#pragma endregion

#pragma region LensFlareEffect

LensFlareEffect::LensFlareEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;

	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();

	mView = mFX->GetVariableByName("gView")->AsMatrix();

	m_SceneMap = mFX->GetVariableByName("SceneMap")->AsShaderResource();
	m_BasicColorMap = mFX->GetVariableByName("HighLightMap")->AsShaderResource();



	m_DirtMask = mFX->GetVariableByName("DirtMaskMap")->AsShaderResource();
	m_StarBurst = mFX->GetVariableByName("StarBurstMap")->AsShaderResource();

	m_factor01 = mFX->GetVariableByName("factor01")->AsScalar();
	m_factor02 = mFX->GetVariableByName("factor02")->AsScalar();
	m_factor03 = mFX->GetVariableByName("factor03")->AsScalar();
	m_factor04 = mFX->GetVariableByName("factor04")->AsScalar();
	m_factor05 = mFX->GetVariableByName("factor05")->AsScalar();
	m_factor06 = mFX->GetVariableByName("factor06")->AsScalar();
	m_factor07 = mFX->GetVariableByName("factor07")->AsScalar();




	LensFlareTech = mFX->GetTechniqueByName("LensFlareTech");

	GaussianVerticalTech = mFX->GetTechniqueByName("GaussianVerticalTech");
	GaussianHeightTech = mFX->GetTechniqueByName("GaussianHeightTech");
}

LensFlareEffect::~LensFlareEffect()
{
	ReleaseCOM(LensFlareTech);
	ReleaseCOM(GaussianVerticalTech);
	ReleaseCOM(GaussianHeightTech);
}

void LensFlareEffect::SetNulltoTextures()
{
	SetTexture(NULL, m_SceneMap);
	SetTexture(NULL, m_BasicColorMap);
	SetTexture(NULL, m_DirtMask);
	SetTexture(NULL, m_StarBurst);
}

void LensFlareEffect::SetAllVariables(CXMMATRIX paraViewProj, CXMMATRIX paraView, ID3D11ShaderResourceView* paraSceneMap, ID3D11ShaderResourceView* paraBasicColorMap
	, ID3D11ShaderResourceView* paraDirtMask
	, ID3D11ShaderResourceView* paraStarBurst
	
	, float* pFactor)
{
	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraView, mView);

	SetTexture(paraSceneMap, m_SceneMap);
	SetTexture(paraBasicColorMap, m_BasicColorMap);

	SetTexture(paraDirtMask, m_DirtMask);
	SetTexture(paraStarBurst, m_StarBurst);

	SetScalar(pFactor[0], m_factor01);
	SetScalar(pFactor[1], m_factor02);
	SetScalar(pFactor[2], m_factor03);
	SetScalar(pFactor[3], m_factor04);
	SetScalar(pFactor[4], m_factor05);
	SetScalar(pFactor[5], m_factor06);
	SetScalar(pFactor[6], m_factor07);


}


#pragma endregion

#pragma region DepthOfFieldEffect

DepthOfFieldEffect::DepthOfFieldEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;

	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	
	mInvProj = mFX->GetVariableByName("gInvProj")->AsMatrix();
	

	m_SceneMap = mFX->GetVariableByName("SceneMap")->AsShaderResource();
	m_BlurMap = mFX->GetVariableByName("BlurMap")->AsShaderResource();
	m_DepthMap = mFX->GetVariableByName("DepthMap")->AsShaderResource();
	
	
	
	DOFTech = mFX->GetTechniqueByName("DOFTech");
	HBlurTech = mFX->GetTechniqueByName("HBlurTech");
	VBlurTech = mFX->GetTechniqueByName("VBlurTech");

	m_factor01 = mFX->GetVariableByName("factor01")->AsScalar();
	m_factor02 = mFX->GetVariableByName("factor02")->AsScalar();
	m_factor03 = mFX->GetVariableByName("factor03")->AsScalar();
	m_factor04 = mFX->GetVariableByName("factor04")->AsScalar();
	m_factor05 = mFX->GetVariableByName("factor05")->AsScalar();
	m_factor06 = mFX->GetVariableByName("factor06")->AsScalar();
	m_factor07 = mFX->GetVariableByName("factor07")->AsScalar();

	vDofParams = mFX->GetVariableByName("vDofParams")->AsVector();

}

DepthOfFieldEffect::~DepthOfFieldEffect()
{
	ReleaseCOM(DOFTech);
	ReleaseCOM(HBlurTech);
	ReleaseCOM(VBlurTech);
}

void DepthOfFieldEffect::SetNulltoTextures()
{
	SetTexture(NULL, m_SceneMap);
	SetTexture(NULL, m_DepthMap);
	SetTexture(NULL, m_BlurMap);
}

void DepthOfFieldEffect::SetAllVariables(CXMMATRIX paraWVP, CXMMATRIX paraViewProj, CXMMATRIX paraInvProj, ID3D11ShaderResourceView* paraSceneMap,
	ID3D11ShaderResourceView* paraDepthMap, ID3D11ShaderResourceView* paraBlurMap, float* pFactor, XMFLOAT4 paraDof)
{
	SetMatrix(paraViewProj, mViewProj);
	SetMatrix(paraInvProj, mInvProj);

	SetTexture(paraSceneMap, m_SceneMap);
	SetTexture(paraDepthMap, m_DepthMap);
	SetTexture(paraBlurMap, m_BlurMap);

	SetScalar(pFactor[0], m_factor01);
	SetScalar(pFactor[1], m_factor02);
	SetScalar(pFactor[2], m_factor03);
	SetScalar(pFactor[3], m_factor04);
	SetScalar(pFactor[4], m_factor05);
	SetScalar(pFactor[5], m_factor06);
	SetScalar(pFactor[6], m_factor07);

	SetVector4(paraDof, vDofParams);
	
}

#pragma endregion

#pragma region SceneEffect
SceneEffect::SceneEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;

	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	m_SceneMap = mFX->GetVariableByName("SceneMap")->AsShaderResource();
	m_ColorMap = mFX->GetVariableByName("ColorMap")->AsShaderResource();
	SceneTech = mFX->GetTechniqueByName("SceneTech");
	MulTech = mFX->GetTechniqueByName("MulTech");
	CompTech = mFX->GetTechniqueByName("CompTech");
}

SceneEffect::~SceneEffect()
{
	ReleaseCOM(SceneTech);
	ReleaseCOM(MulTech);
	ReleaseCOM(CompTech);
}

void SceneEffect::SetNulltoTextures()
{
	SetTexture(NULL, m_SceneMap);
	SetTexture(NULL, m_ColorMap);
}

void SceneEffect::SetAllVariables(CXMMATRIX paraWVP, CXMMATRIX paraViewProj, ID3D11ShaderResourceView* paraSceneMap, ID3D11ShaderResourceView* paraColorMap)
{
	SetMatrix(paraViewProj, mViewProj);
	SetTexture(paraSceneMap, m_SceneMap);
	SetTexture(paraColorMap, m_ColorMap);
}


#pragma endregion



#pragma region ShadowMapEffect

ShadowMapEffect::ShadowMapEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;


	mWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	//m_ShadowMap = mFX->GetVariableByName("ShadowMap")->AsShaderResource();

	mTech = mFX->GetTechniqueByName("ShadowMapTech");
}

ShadowMapEffect::~ShadowMapEffect()
{

}

void ShadowMapEffect::SetNulltoTextures()
{
	//SetTexture(NULL, m_ShadowMap);
}

void ShadowMapEffect::SetAllVariables(CXMMATRIX paraWVP, CXMMATRIX paraViewProj, ID3D11ShaderResourceView* paraSceneMap)
{
	SetMatrix(paraWVP, mWorldViewProj);
	SetMatrix(paraViewProj, mViewProj);
}

#pragma region Effects


#pragma region HBAOEffect
HBAOEffect::HBAOEffect(ID3D11Device* device, const LPCWSTR filename) : Effect(device, filename)
{
	m_TextureParameterCount = 1;

	mView = mFX->GetVariableByName("gView")->AsMatrix();
	mProj = mFX->GetVariableByName("gProj")->AsMatrix();
	mViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	m_pixelSize = mFX->GetVariableByName("pixelSize")->AsVector();
	m_DepthMap = mFX->GetVariableByName("DepthMap")->AsShaderResource();
	m_NormalMap = mFX->GetVariableByName("NormalMap")->AsShaderResource();
	m_RandomNoiseMap = mFX->GetVariableByName("RandomNoiseMap")->AsShaderResource();
	m_PositionMap = mFX->GetVariableByName("PositionMap")->AsShaderResource();

	mInvProj = mFX->GetVariableByName("gInvProj")->AsMatrix();
	mInvViewProj = mFX->GetVariableByName("gInvViewProj")->AsMatrix();

	m_factor01 = mFX->GetVariableByName("factor01")->AsScalar();
	m_factor02 = mFX->GetVariableByName("factor02")->AsScalar();
	m_factor03 = mFX->GetVariableByName("factor03")->AsScalar();
	m_factor04 = mFX->GetVariableByName("factor04")->AsScalar();
	m_factor05 = mFX->GetVariableByName("factor05")->AsScalar();
	m_factor06 = mFX->GetVariableByName("factor06")->AsScalar();
	m_factor07 = mFX->GetVariableByName("factor07")->AsScalar();

	HBAOTech = mFX->GetTechniqueByName("HBAOTech");

	GaussianVerticalTech = mFX->GetTechniqueByName("GaussianVerticalTech");
	GaussianHeightTech = mFX->GetTechniqueByName("GaussianHeightTech");
}

HBAOEffect::~HBAOEffect()
{
	ReleaseCOM(HBAOTech);
	ReleaseCOM(GaussianVerticalTech);
	ReleaseCOM(GaussianHeightTech);
}

void HBAOEffect::SetNulltoTextures()
{
	SetTexture(NULL, m_DepthMap);
	SetTexture(NULL, m_NormalMap);
	SetTexture(NULL, m_RandomNoiseMap);
	SetTexture(NULL, m_PositionMap);
}

void HBAOEffect::SetAllVariables(CXMMATRIX paraView, CXMMATRIX paraProj, CXMMATRIX paraViewProj, CXMMATRIX paraInvViewProj, CXMMATRIX paraInvProj, ID3D11ShaderResourceView* paraDepthMap, ID3D11ShaderResourceView* paraNormalMap, ID3D11ShaderResourceView* paraRandomNoiselMap,
	ID3D11ShaderResourceView* paraPositionMap, float* pFactor, XMFLOAT2 pPixelSize)
{
	SetMatrix(paraView, mView);
	SetMatrix(paraProj, mProj);
	SetMatrix(paraViewProj, mViewProj);	
	SetMatrix(paraInvProj, mInvProj);

	SetMatrix(paraInvViewProj, mInvViewProj);

	SetTexture(paraDepthMap, m_DepthMap);
	SetTexture(paraNormalMap, m_NormalMap);
	SetTexture(paraRandomNoiselMap, m_RandomNoiseMap);
	SetTexture(paraPositionMap, m_PositionMap);

	SetScalar(pFactor[0], m_factor01);
	SetScalar(pFactor[1], m_factor02);
	SetScalar(pFactor[2], m_factor03);
	SetScalar(pFactor[3], m_factor04);
	SetScalar(pFactor[4], m_factor05);
	SetScalar(pFactor[5], m_factor06);
	SetScalar(pFactor[6], m_factor07);

	SetVector2(pPixelSize, m_pixelSize);

}
#pragma endregion

void Effects::InitAll(ID3D11Device* device)
{
	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	WCHAR DirPath[MAX_PATH];
	WCHAR FilePath[MAX_PATH];
	const WCHAR* pDirPath = NULL;
	DWORD DW = MAX_PATH;
	
	GetModuleFileName(NULL, FilePath, DW);
	wcscpy_s(DirPath, (ExtractDirectoryWchar(FilePath)).c_str());

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\BasicMaterial.fx");
	BasicFX = new BasicMaterialEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\Lighting.fx");
	BRDFLightingFX = new BRDFLightingEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\BoundingBox.fx");
	LineDrawFx = new LineDrawEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\OutLine.fx");
	OutLineDrawFx = new OutLineDrawEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\DebugWindow.fx");
	DebugWindowFx = new DebugWindowEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\Gizmo.fx");
	GizmoDrawFx = new GizmoDrawEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\UISprite.fx");
	UISpriteFx = new UISpriteEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\ReflectionActor.fx");
	ReflectionActorFx = new ReflectionActorEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\SkyBox.fx");
	SkyBoxFX = new SkyBoxEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\Bloom.fx");
	BloomFX = new BloomEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\LensFlare.fx");
	LensFlareFX = new LensFlareEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\Scene.fx");
	SceneFX = new SceneEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\ShadowMap.fx");
	ShadowMapFx = new ShadowMapEffect(device, FilePath);

	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\HBAO.fx");
	HBAOFX = new HBAOEffect(device, FilePath);


	wcscpy_s(FilePath, DirPath);
	wcscat_s(FilePath, L"\\Engine\\Shaders\\DepthOfField.fx");
	DOFFX = new DepthOfFieldEffect(device, FilePath);

	
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(BRDFLightingFX);
	SafeDelete(LineDrawFx);
	SafeDelete(OutLineDrawFx);
	SafeDelete(DebugWindowFx);
	SafeDelete(GizmoDrawFx);
	SafeDelete(UISpriteFx);
	SafeDelete(ReflectionActorFx);

	SafeDelete(SkyBoxFX);
	SafeDelete(BloomFX);

	SafeDelete(LensFlareFX);

	SafeDelete(SceneFX);

	SafeDelete(ShadowMapFx);
	SafeDelete(HBAOFX);

	SafeDelete(DOFFX);
}

void Effects::ReleaseTextureResource(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTech, Effect* pEffect)
{
	D3DX11_TECHNIQUE_DESC techDesc;
	pTech->GetDesc(&techDesc);

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		if (pEffect == NULL)
		{
			continue;
		}
		
		BasicMaterialEffect* pBasicMaterialEffect = dynamic_cast<BasicMaterialEffect*>(pEffect);
		if (pBasicMaterialEffect)
		{
			pBasicMaterialEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		BRDFLightingEffect* pBRDFLightingEffect = dynamic_cast<BRDFLightingEffect*>(pEffect);
		if (pBRDFLightingEffect)
		{
			pBRDFLightingEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		LineDrawEffect* pLineDrawEffect = dynamic_cast<LineDrawEffect*>(pEffect);
		if (pLineDrawEffect)
		{
			pLineDrawEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		OutLineDrawEffect* pOutLineDrawEffect = dynamic_cast<OutLineDrawEffect*>(pEffect);
		if (pOutLineDrawEffect)
		{
			pOutLineDrawEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		DebugWindowEffect* pDebugWindowEffect = dynamic_cast<DebugWindowEffect*>(pEffect);
		if (pDebugWindowEffect)
		{
			pDebugWindowEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		GizmoDrawEffect* pGizmoDrawEffect = dynamic_cast<GizmoDrawEffect*>(pEffect);
		if (pGizmoDrawEffect)
		{
			pGizmoDrawEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		UISpriteEffect* pUISpriteEffect = dynamic_cast<UISpriteEffect*>(pEffect);
		if (pUISpriteEffect)
		{
			pUISpriteEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		ReflectionActorEffect* pReflectionActorEffect = dynamic_cast<ReflectionActorEffect*>(pEffect);
		if (pReflectionActorEffect)
		{
			pReflectionActorEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}


		SkyBoxEffect* pSkyBoxEffect = dynamic_cast<SkyBoxEffect*>(pEffect);
		if (pSkyBoxEffect)
		{
			pSkyBoxEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		BloomEffect* pBloomEffect = dynamic_cast<BloomEffect*>(pEffect);
		if (pBloomEffect)
		{
			pBloomEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		LensFlareEffect* pLensFlareEffect = dynamic_cast<LensFlareEffect*>(pEffect);
		if (pLensFlareEffect)
		{
			pLensFlareEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}


		SceneEffect* pSceneEffect = dynamic_cast<SceneEffect*>(pEffect);
		if (pSceneEffect)
		{
			pSceneEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		ShadowMapEffect* pShadowMapEffect = dynamic_cast<ShadowMapEffect*>(pEffect);
		if (pShadowMapEffect)
		{
			pShadowMapEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		HBAOEffect* pHBAOEffect = dynamic_cast<HBAOEffect*>(pEffect);
		if (pHBAOEffect)
		{
			pHBAOEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		DepthOfFieldEffect* pPPCEffect = dynamic_cast<DepthOfFieldEffect*>(pEffect);
		if (pPPCEffect)
		{
			pPPCEffect->SetNulltoTextures();
			pTech->GetPassByIndex(p)->Apply(0, deviceContext);
			continue;
		}

		pEffect->SetNulltoTextures();
		pTech->GetPassByIndex(p)->Apply(0, deviceContext);
	}
}
#pragma endregion




