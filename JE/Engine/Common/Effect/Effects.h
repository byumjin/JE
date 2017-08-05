//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include "../d3dUtil.h"
#include "../../Textures/Texture.h"

enum EffectKind
{
	BASICMATERIALEFFECT,
	BRDFLIGHTNGEFFECT,
	LINEDRAWEFFECT,
	OUTINEDRAWEFFECT,
	DEBUGWINDOWEFFECT,
	GIZMODRAWEFFECT,
	UISPRITEEFFECT,
	REFLECTIONACTOREFFECT,
	SKYBOXERRECT,
	BLOOMEFFECT,
	LENSFLAREEFFECT,
	SCENEEFFECT,
	SHADOWMAPEFFECT,
	HBAOEFFECT
};

static std::string ExtractDirectory(const std::string& path)
{
	return path.substr(0, path.find_last_of('\\') + 1);
}

static std::wstring ExtractDirectoryWchar(const WCHAR* path)
{
	std::wstring source(path);
	std::wstring ProjectFolder;

	size_t lastposition;
	UINT i = 0;
	while (i < 2)
	{
		lastposition = source.rfind(L"\\", source.length());
		source = source.substr(0, lastposition);
		i++;
	}

	size_t startposition = source.rfind(L"\\", source.length());
	ProjectFolder = source.substr(startposition, lastposition);

	source.append(ProjectFolder);

	return source;
}

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const LPCTSTR /*const std::wstring&*/ filename);
	virtual ~Effect();

	void SetMatrix(CXMMATRIX M, ID3DX11EffectMatrixVariable* pMat)			{ pMat->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetVector4(const XMFLOAT4& v, ID3DX11EffectVectorVariable* pVector) { pVector->SetRawValue(&v, 0, sizeof(XMFLOAT4)); }
	void SetVector(const XMFLOAT3& v, ID3DX11EffectVectorVariable* pVector)	{ pVector->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetVector2(const XMFLOAT2& v, ID3DX11EffectVectorVariable* pVector)	{ pVector->SetRawValue(&v, 0, sizeof(XMFLOAT2)); }
	void SetScalar(const float s, ID3DX11EffectVariable* pScalar)			{ pScalar->SetRawValue(&s, 0, sizeof(float)); }
	void SetBool(const bool b, ID3DX11EffectVariable* pBool)			{ pBool->SetRawValue(&b, 0, sizeof(bool)); }

	//Must release ShaderResourceView later
	void SetTexture(ID3D11ShaderResourceView* Texture, ID3DX11EffectShaderResourceVariable* pTexture)
	{
		if(Texture)
		pTexture->SetResource(Texture);
	}

	


	virtual void SetNulltoTextures()
	{

	}

	virtual void SetAllVariables();

	ID3DX11EffectTechnique* mTech; 
	UINT m_TextureParameterCount;

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
	//CEffect* mFX;

	//common variables
	//Matrix
	ID3DX11EffectMatrixVariable* mWorld;
	ID3DX11EffectMatrixVariable* mViewProj;
	ID3DX11EffectMatrixVariable* mWorldViewProj;
	ID3DX11EffectMatrixVariable* mWorldInvTranspose;
	ID3DX11EffectMatrixVariable* mInvViewProj;
	ID3DX11EffectMatrixVariable* mInvProj;
	
	//Vector
	ID3DX11EffectVectorVariable* mEyePosW;
	
	//Scalar
	ID3DX11EffectVariable* mTime;

	ID3D11ClassLinkage* m_ClassLinkage;
};

class MaterialInstance : public Asset
{
public:
	MaterialInstance()
	{

	}
	virtual ~MaterialInstance()
	{

	}

private:

};



#pragma endregion

#pragma region LineDrawEffect
class LineDrawEffect : public Effect
{
public:
	LineDrawEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~LineDrawEffect();	

	void SetAllVariables(CXMMATRIX paraWVP, bool paraSelected);

	ID3DX11EffectVariable* mbSelected;
	
	
};
#pragma endregion

#pragma region OutLineDrawEffect
class OutLineDrawEffect : public Effect
{
public:
	OutLineDrawEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~OutLineDrawEffect();

	virtual void SetNulltoTextures()
	{		
		SetTexture(NULL, mStencilTexture);
	}

	void SetAllVariables(CXMMATRIX paraViewProj, CXMMATRIX paraWVP, ID3D11ShaderResourceView* paraStencilTexture, XMFLOAT2 paraTextureSize);

	ID3DX11EffectShaderResourceVariable* mStencilTexture;

	ID3DX11EffectVectorVariable* mTextureSize;	

	ID3DX11EffectTechnique* FillSilhouetteTech;
	ID3DX11EffectTechnique* OutLineTech;

};
#pragma endregion

#pragma region GizmoDrawEffect
class GizmoDrawEffect : public Effect
{
public:
	GizmoDrawEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~GizmoDrawEffect();

	void SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraWVP, CXMMATRIX paraOriginTranslation, float paraLength);

	void SetAxisTechVariables(CXMMATRIX paraWorld, CXMMATRIX paraWVP, CXMMATRIX paraOriginTranslation, float paraLength);
	void SetTransTechVariables(CXMMATRIX paraWorld, CXMMATRIX paraWVP, CXMMATRIX paraOriginTranslation, float paraLength, int SelectedAxis);

	ID3DX11EffectMatrixVariable* mOriginTranslation;

	ID3DX11EffectTechnique* TransTech;

	//Scalar
	ID3DX11EffectVariable* mLength;
	ID3DX11EffectVariable* mSelectedAxis;
};
#pragma endregion

#pragma region BasicMaterialEffect
class BasicMaterialEffect : public Effect
{
public:
	BasicMaterialEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~BasicMaterialEffect();
	
	virtual void SetNulltoTextures()
	{
		SetTexture(NULL, mDiffuseMap);
		SetTexture(NULL, mSpecularMap);
		SetTexture(NULL, mNormalMap);

		SetTexture(NULL, m_SkyBoxCubeMap);		
	}

	void SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraWorldView, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraIV, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
		XMFLOAT3 paraEyePosW, float paraTime, bool parabSelected, ID3D11ShaderResourceView* DiffuseTexture, ID3D11ShaderResourceView* SpecularTexture,
		ID3D11ShaderResourceView* NormalTexture);
	
	void SetSkyBoxVariables(CXMMATRIX paraWorld, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
		XMFLOAT3 paraEyePosW, float paraTime, ID3D11ShaderResourceView* paraSkyBoxCubeMap);

	

	ID3DX11EffectShaderResourceVariable* mDiffuseMap;
	ID3DX11EffectShaderResourceVariable* mSpecularMap;
	ID3DX11EffectShaderResourceVariable* mNormalMap;

	ID3DX11EffectShaderResourceVariable* m_SkyBoxCubeMap;

	Texture *m_DiffuseMap;
	Texture *m_SpecularMap;
	Texture *m_NormalMap;	

	ID3DX11EffectMatrixVariable* mWorldView;
	ID3DX11EffectMatrixVariable* mInvView;

	ID3DX11EffectVariable* mbSelected;

	

	ID3DX11EffectTechnique* SkyBoxTech;

	
};

class BasicMaterialEffect_Instance : public MaterialInstance
{
public:

	BasicMaterialEffect_Instance()
	{

	};

	void SetTextures(Texture *DiffuseMap, Texture *SpecularMap, Texture *NormalMap)
	{
		m_DiffuseMap = DiffuseMap;
		m_SpecularMap = SpecularMap;
		m_NormalMap = NormalMap;
	}

	void LoadFromFilename(ID3D11Device * pd3dDevice, LPCWSTR path)
	{

	}

	Texture *m_DiffuseMap;
	Texture *m_SpecularMap;
	Texture *m_NormalMap;

	WCHAR m_strPath_DiffuseMap[MAX_PATH];
	WCHAR m_strPath_SpecularMap[MAX_PATH];
	WCHAR m_strPath_NormalMap[MAX_PATH];

	WCHAR m_strTitle[MAX_PATH];

	BasicMaterialEffect *pParentMaterial;
};


#pragma endregion

#pragma region BRDFLightingEffect
class BRDFLightingEffect : public Effect
{
public:
	BRDFLightingEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~BRDFLightingEffect();
		
	void SetDirLights(const DirectionalLight& lights)   { DirLights->SetRawValue(&lights, 0, sizeof(DirectionalLight)); }
	
	void SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
		XMFLOAT3 paraEyePosW, float paraTime, CXMMATRIX paraShadowVP, CXMMATRIX paraDepthIVP,
		ID3D11ShaderResourceView* paraBasicColorTexture,
		ID3D11ShaderResourceView* param_SpecularTexture,
		ID3D11ShaderResourceView* paraNormalTexture,	
		ID3D11ShaderResourceView* paraDepthInfoTexture,
		ID3D11ShaderResourceView* paraReflectionTexture,
		ID3D11ShaderResourceView* paraLUTTexture,
		ID3D11ShaderResourceView* paraShadowTexture
		);
	

	virtual void SetNulltoTextures()
	{
		SetTexture(NULL, mBasicColorTexture);
		SetTexture(NULL, mNormalTexture);
		SetTexture(NULL, m_SpecularTexture);
		SetTexture(NULL, m_ReflectionTexture);
		
		SetTexture(NULL, mDepthInfoTexture);

		SetTexture(NULL, m_BRDFLUTTexture);
		SetTexture(NULL, m_ShadowTexture);
		
	}
	
	ID3DX11EffectMatrixVariable* m_ShadowVP;
	ID3DX11EffectMatrixVariable* m_DepthInvProj;
	ID3DX11EffectVariable* DirLights;

	ID3DX11EffectShaderResourceVariable* mBasicColorTexture;
	ID3DX11EffectShaderResourceVariable* m_SpecularTexture;
	ID3DX11EffectShaderResourceVariable* mNormalTexture;
	ID3DX11EffectShaderResourceVariable* m_ReflectionTexture;

	ID3DX11EffectShaderResourceVariable* mDepthInfoTexture;
	ID3DX11EffectShaderResourceVariable* m_BRDFLUTTexture;

	ID3DX11EffectShaderResourceVariable* m_ShadowTexture;
	
};
#pragma endregion

#pragma region DebugWindowEffect
class DebugWindowEffect : public Effect
{
public:
	DebugWindowEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~DebugWindowEffect();

	void SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraViewProj, CXMMATRIX paraWVP, CXMMATRIX paraWIT, CXMMATRIX paraIVP, CXMMATRIX paraIP,
		XMFLOAT3 paraEyePosW, float paraTime, ID3D11ShaderResourceView* paraBasicTexture, ID3D11ShaderResourceView* paraSpecularTexture, ID3D11ShaderResourceView* paraNormalTexture, ID3D11ShaderResourceView* paraDepthTexture,
		ID3D11ShaderResourceView* paraAOTexture, ID3D11ShaderResourceView* paraUVTexture, ID3D11ShaderResourceView* paraEnvTexture, ID3D11ShaderResourceView* paraBloomTexture, ID3D11ShaderResourceView* paraLensFlareTexture,
		ID3D11ShaderResourceView* paraShadowTexture,
		CXMMATRIX paraInvDepthViewProj);

	virtual void SetNulltoTextures()
	{
		SetTexture(NULL, mBasicTexture);
		SetTexture(NULL, m_SpecularTexture);
		SetTexture(NULL, mNormalTexture);
		SetTexture(NULL, mDepthTexture);
		SetTexture(NULL, mAOTexture);
		SetTexture(NULL, m_UVTexture);
		SetTexture(NULL, m_EnvTexture);
		SetTexture(NULL, m_BloomTexture);
		SetTexture(NULL, m_ShadowTexture);
		SetTexture(NULL, m_LensFlareTexture);
	}

	ID3DX11EffectMatrixVariable* m_InvDepthViewProj;

	ID3DX11EffectShaderResourceVariable* mBasicTexture;
	ID3DX11EffectShaderResourceVariable* m_SpecularTexture;
	ID3DX11EffectShaderResourceVariable* mNormalTexture;
	ID3DX11EffectShaderResourceVariable* mDepthTexture;
	ID3DX11EffectShaderResourceVariable* mAOTexture;

	ID3DX11EffectShaderResourceVariable* m_LensFlareTexture;
	ID3DX11EffectShaderResourceVariable* m_UVTexture;
	ID3DX11EffectShaderResourceVariable* m_EnvTexture;
	ID3DX11EffectShaderResourceVariable* m_BloomTexture;
	ID3DX11EffectShaderResourceVariable* m_ShadowTexture;

	ID3DX11EffectTechnique* BasicTech;
	ID3DX11EffectTechnique* SpecTech;
	ID3DX11EffectTechnique* NormalTech;
	ID3DX11EffectTechnique* RoughnessTech;
	ID3DX11EffectTechnique* MetallicTech;
	ID3DX11EffectTechnique* WorldPositionTech;
	ID3DX11EffectTechnique* DepthTech;
	ID3DX11EffectTechnique* SelectedTech;
	ID3DX11EffectTechnique* OpacityTech;

	ID3DX11EffectTechnique* LensFlareTech;

	ID3DX11EffectTechnique* AOTech;
	ID3DX11EffectTechnique* EnvTech;
	ID3DX11EffectTechnique* BloomTech;

	ID3DX11EffectTechnique* Custom01Tech;
	//ID3DX11EffectTechnique* Custom02Tech;
	


};
#pragma endregion

#pragma region UISpriteEffect
class UISpriteEffect : public Effect
{
public:
	UISpriteEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~UISpriteEffect();

	virtual void SetNulltoTextures()
	{
		SetTexture(NULL, m_UISpriteMap);
	}

	void SetAllVariables(CXMMATRIX paraWorld, CXMMATRIX paraPV, CXMMATRIX paraWVP, XMFLOAT3 paraEyeWorldPos, bool paraSelected, ID3D11ShaderResourceView* UISpriteTexture);

	ID3DX11EffectVariable* m_bSelected;
	ID3DX11EffectTechnique* UISpriteTech;
	ID3DX11EffectShaderResourceVariable* m_UISpriteMap;
	

};
#pragma endregion

#pragma region ReflectionActorEffect
class ReflectionActorEffect : public Effect
{
public:
	ReflectionActorEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~ReflectionActorEffect();

	void SetDDNullTextures();
	virtual void SetNulltoTextures();
	
	void SetColorVariables(CXMMATRIX paraWVP, ID3D11ShaderResourceView* paraDiffuseTexture);


	void SetCubeVariables(CXMMATRIX paraWorldMat, CXMMATRIX paraWVP, CXMMATRIX paraInvVP, ID3D11ShaderResourceView* paraWorldNormalMap, ID3D11ShaderResourceView* paraSpecualarMap,
		 XMFLOAT3 paraEyePosW, float EnvCubeCounter, XMFLOAT3 ObjWorldPosition);

	void SetAllEnvTextures(ID3D11ShaderResourceView* EnvCubeMap, UINT EnvCubeCounter);

	void SetSkyBoxVariables(CXMMATRIX paraWVP, CXMMATRIX paraWorldMat, ID3D11ShaderResourceView* paraSkyBoxTexture);


	ID3DX11EffectTechnique* ReflectionActorTech;
	ID3DX11EffectTechnique* CompositionCubeMapTech;

	//ID3DX11EffectTechnique* DrawObjTech;

	ID3DX11EffectTechnique* SkyBoxTech;

	ID3DX11EffectMatrixVariable* m_InvDepthViewProj;

	ID3DX11EffectShaderResourceVariable* m_DiffuseMap;
	ID3DX11EffectShaderResourceVariable* m_SkyBoxCubeMap;

	//ID3DX11EffectShaderResourceVariable* m_DepthInfoMap;
	ID3DX11EffectShaderResourceVariable* m_NormalMap;
	ID3DX11EffectShaderResourceVariable* m_SpecularMap;

	ID3DX11EffectVariable* m_iEnvCubeCounter;
	ID3DX11EffectVectorVariable* m_WorldCenterPosition;

	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap01;
	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap02;
	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap03;
	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap04;
	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap05;
	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap06;


};
#pragma endregion


#pragma region SkyBoxEffect
class SkyBoxEffect : public Effect
{
public:
	SkyBoxEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~SkyBoxEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraWVP, ID3D11ShaderResourceView* paraEnvCubeMap);

	ID3DX11EffectTechnique*SkyBoxTech;
	ID3DX11EffectShaderResourceVariable* m_EnvCubeMap;
};
#pragma endregion


#pragma region BloomEffect
class BloomEffect : public Effect
{
public:
	BloomEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~BloomEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraViewProj, ID3D11ShaderResourceView* paraSceneMap, ID3D11ShaderResourceView* paraBasicColorMap, float* pFactor);

	ID3DX11EffectTechnique* BloomTech;

	ID3DX11EffectTechnique* GaussianVerticalTech;
	ID3DX11EffectTechnique* GaussianHeightTech;

	ID3DX11EffectShaderResourceVariable* m_SceneMap;
	ID3DX11EffectShaderResourceVariable* m_BasicColorMap;

	ID3DX11EffectVariable* m_factor01;
	ID3DX11EffectVariable* m_factor02;
	ID3DX11EffectVariable* m_factor03;
	ID3DX11EffectVariable* m_factor04;
	ID3DX11EffectVariable* m_factor05;
	ID3DX11EffectVariable* m_factor06;
	ID3DX11EffectVariable* m_factor07;
};
#pragma endregion


#pragma region LensFlare
class LensFlareEffect : public Effect
{
public:
	LensFlareEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~LensFlareEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraViewProj, CXMMATRIX paraView, ID3D11ShaderResourceView* paraSceneMap, ID3D11ShaderResourceView* paraBasicColorMap
		, ID3D11ShaderResourceView* paraDirtMask
		, ID3D11ShaderResourceView* paraStarBurst

		, float* pFactor);

	ID3DX11EffectMatrixVariable* mView;

	ID3DX11EffectTechnique* LensFlareTech;

	ID3DX11EffectTechnique* GaussianVerticalTech;
	ID3DX11EffectTechnique* GaussianHeightTech;

	ID3DX11EffectShaderResourceVariable* m_SceneMap;
	ID3DX11EffectShaderResourceVariable* m_BasicColorMap;

	ID3DX11EffectShaderResourceVariable* m_DirtMask;
	ID3DX11EffectShaderResourceVariable* m_StarBurst;

	ID3DX11EffectVariable* m_factor01;
	ID3DX11EffectVariable* m_factor02;
	ID3DX11EffectVariable* m_factor03;
	ID3DX11EffectVariable* m_factor04;
	ID3DX11EffectVariable* m_factor05;
	ID3DX11EffectVariable* m_factor06;
	ID3DX11EffectVariable* m_factor07;
};
#pragma endregion

#pragma region DepthOfFieldEffect
class DepthOfFieldEffect : public Effect
{
public:
	DepthOfFieldEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~DepthOfFieldEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraWVP, CXMMATRIX paraViewProj, CXMMATRIX paraInvProj, ID3D11ShaderResourceView* paraSceneMap,
		ID3D11ShaderResourceView* paraDepthMap, ID3D11ShaderResourceView* paraBlurMap, float* pFactor, XMFLOAT4 paraDof);

	ID3DX11EffectTechnique* DOFTech;
	ID3DX11EffectTechnique* HBlurTech;
	ID3DX11EffectTechnique* VBlurTech;

	ID3DX11EffectShaderResourceVariable* m_SceneMap;
	ID3DX11EffectShaderResourceVariable* m_DepthMap;
	ID3DX11EffectShaderResourceVariable* m_BlurMap;

	ID3DX11EffectVariable* m_factor01;
	ID3DX11EffectVariable* m_factor02;
	ID3DX11EffectVariable* m_factor03;
	ID3DX11EffectVariable* m_factor04;
	ID3DX11EffectVariable* m_factor05;
	ID3DX11EffectVariable* m_factor06;
	ID3DX11EffectVariable* m_factor07;

	ID3DX11EffectVectorVariable* vDofParams;
	

};
#pragma endregion

#pragma region SceneEffect
class SceneEffect : public Effect
{
public:
	SceneEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~SceneEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraWVP, CXMMATRIX paraViewProj, ID3D11ShaderResourceView* paraSceneMap, ID3D11ShaderResourceView* paraColorMap);

	ID3DX11EffectTechnique* SceneTech;
	ID3DX11EffectTechnique* MulTech;
	ID3DX11EffectTechnique* CompTech;

	ID3DX11EffectShaderResourceVariable* m_SceneMap;
	ID3DX11EffectShaderResourceVariable* m_ColorMap;
};
#pragma endregion

#pragma region ShadowMapEffect
class ShadowMapEffect : public Effect
{
public:
	ShadowMapEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~ShadowMapEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraWVP, CXMMATRIX paraViewProj, ID3D11ShaderResourceView* paraSceneMap);
	
	//ID3DX11EffectShaderResourceVariable* m_ShadowMap;
};
#pragma endregion

#pragma region HBAOEffect
class HBAOEffect : public Effect
{
public:
	HBAOEffect(ID3D11Device* device, const LPCWSTR filename);
	virtual ~HBAOEffect();

	virtual void SetNulltoTextures();

	void SetAllVariables(CXMMATRIX paraView, CXMMATRIX paraProj, CXMMATRIX paraViewProj, CXMMATRIX paraInvProj, CXMMATRIX paraInvViewProj, ID3D11ShaderResourceView* paraDepthMap, ID3D11ShaderResourceView* paraNormalMap,
		ID3D11ShaderResourceView* paraRandomNoiselMap, ID3D11ShaderResourceView* paraPositionMap, float* pFactor, XMFLOAT2 pPixelSize);

	ID3DX11EffectTechnique* HBAOTech;

	ID3DX11EffectTechnique* GaussianVerticalTech;
	ID3DX11EffectTechnique* GaussianHeightTech;

	ID3DX11EffectShaderResourceVariable* m_DepthMap;
	ID3DX11EffectShaderResourceVariable* m_NormalMap;
	ID3DX11EffectShaderResourceVariable* m_RandomNoiseMap;
	ID3DX11EffectShaderResourceVariable* m_PositionMap;

	ID3DX11EffectMatrixVariable* mView;
	ID3DX11EffectMatrixVariable* mProj;

	ID3DX11EffectVectorVariable* m_pixelSize;

	ID3DX11EffectVariable* m_factor01;
	ID3DX11EffectVariable* m_factor02;
	ID3DX11EffectVariable* m_factor03;
	ID3DX11EffectVariable* m_factor04;
	ID3DX11EffectVariable* m_factor05;
	ID3DX11EffectVariable* m_factor06;
	ID3DX11EffectVariable* m_factor07;
};
#pragma endregion

#pragma region Effects

class Effects
{
public:
	void InitAll(ID3D11Device* device);
	void DestroyAll();

	static void ReleaseTextureResource(ID3D11DeviceContext* deviceContext, ID3DX11EffectTechnique* pTech, Effect* pEffect);	

	BasicMaterialEffect* BasicFX;
	BRDFLightingEffect* BRDFLightingFX;
	LineDrawEffect* LineDrawFx;
	OutLineDrawEffect* OutLineDrawFx;
	DebugWindowEffect* DebugWindowFx;
	GizmoDrawEffect* GizmoDrawFx;
	UISpriteEffect* UISpriteFx;
	ReflectionActorEffect* ReflectionActorFx;
	SkyBoxEffect* SkyBoxFX;

	BloomEffect* BloomFX;
	LensFlareEffect* LensFlareFX;

	SceneEffect* SceneFX;

	ShadowMapEffect* ShadowMapFx;

	HBAOEffect* HBAOFX;

	DepthOfFieldEffect* DOFFX;
};

#pragma endregion



#endif // EFFECTS_H