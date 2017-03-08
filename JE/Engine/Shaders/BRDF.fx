#ifndef BRDF_FX
#define BRDF_FX

#include "common.fx"

float3 Schlick(float VoN, float metallic, float3 BaseColor, float3 SpecularColor)
{
	float a = 1.0f - cos(VoN);

	
	//n = c/v
	/*
	float n1 = 1.0f;//1.0f / 1.0003f;
	float n2 = 1.0f + ior;

	float refraction = (n1 - n2) / (n1 + n2);

	float3 f0 = float3(refraction, refraction, refraction);
	
	//float refraction = lerp(0.21f, 0.98f, metallic);
	//float3 f0 = float3(refraction, refraction, refraction);
	*/
	//float3 SpecularColorSqrt = sqrt(clamp(float3(0, 0, 0), float3(0.99, 0.99, 0.99), SpecularColor));
	//float3 f0 = (1 + SpecularColorSqrt) / (1 - SpecularColorSqrt);
	
	float3 f0 = float3(0.042f, 0.042f, 0.042f);

	//f0 = f0*f0;
	f0 = lerp(f0, BaseColor.rgb, metallic); // ?

	return   f0 + (float3(1.0f,1.0f,1.0f) - f0)*pow(a, 5.0f);
}

float3 Schlick_UE4(float3 ViewVec, float3 HalfVec, float metallic, float3 BaseColor, float3 SpecularColor)
{
	float VoH = saturate(dot(ViewVec, HalfVec));
	
	//n = c/v
	float n1 = 1.0f;//1.0f / 1.0003f;
	float n2 = 1.0f;// +ior;

	float refraction = (n1 - n2) / (n1 + n2);

	float3 f0 = float3(refraction, refraction, refraction);
		f0 = f0*f0;
	f0 = lerp(f0, BaseColor.rgb, metallic);

	return   f0 + (float3(1.0f, 1.0f, 1.0f) - f0)*pow(2.0f, (-5.55473f * VoH - 6.98316f)*VoH);
}

float Geometric_Attenuation(float NoH, float NoV, float HoV, float NoL, float LoH)
{
	float first = 2.0f * NoH * NoV / HoV;
	float second = 2.0f * NoH * NoL / LoH;
	return min(min(first, second), 1.0f);
}

float Beckmann_Distribution(float NoH, float Roughness)
{
	float a = acos(NoH);

	float expval = (1.0f - cos(a)*cos(a)) / (cos(a)*cos(a)*Roughness*Roughness) * -1.0f;
	return exp(expval) / (Roughness*Roughness * cos(a) * cos(a) * cos(a) * cos(a));
}

float3 CookTorrance_Spec(float3 ViewVec, float3 Normal, float3 LightVec, float3 HalfVec, float Roughness, float metallic, float3 BaseColor, float3 SpecularColor)
{
	float NoH = saturate(dot(Normal, HalfVec));
	float NoV = saturate(dot(Normal, ViewVec));
	float HoV = saturate(dot(HalfVec, ViewVec));
	float NoL = saturate(dot(Normal, LightVec));
	float LoH = saturate(dot(LightVec, HalfVec));

	float D = Beckmann_Distribution(NoH, Roughness);
	
	float3 F = Schlick(NoV, metallic, BaseColor, SpecularColor);
	//float3 F = Schlick_UE4(ViewVec, HalfVec, metallic, BaseColor);
	float G = Geometric_Attenuation(NoH, NoV, HoV, NoL, LoH);

	return D*F*G / (4 * NoV * NoL);
	
}

float Pow4(float x)
{
	return x*x*x*x;
}

float2 LightingFunGGX_FV(float dotLH, float roughness)
{
	float alpha = roughness*roughness;

	//F
	float F_a, F_b;
	float dotLH5 = pow(saturate(1.0f - dotLH), 5.0f);
	F_a = 1.0f;
	F_b = dotLH5;

	//V
	float vis;
	float k = alpha * 0.5f;
	float k2 = k*k;
	float invK2 = 1.0f - k2;
	vis = rcp(dotLH*dotLH*invK2 + k2);

	return float2((F_a - F_b)*vis, F_b*vis);
}

float LightingFuncGGX_D(float dotNH, float roughness)
{
	float alpha = roughness*roughness;
	float alphaSqr = alpha*alpha;
	float denom = dotNH * dotNH * (alphaSqr - 1.0f) + 1.0f;

	return alphaSqr / (PI*denom*denom);
}

float3 GGX_Spec(float3 Normal, float3 HalfVec, float Roughness, float metallic, float3 BaseColor, float3 SpecularColor, float2 paraFV)
{
	//float NoL = saturate(dot(Normal, LightVec));
	//float LoH = saturate(dot(LightVec, HalfVec));
	float NoH = saturate(dot(Normal, HalfVec));

	
	float D = LightingFuncGGX_D(Pow4(NoH), Roughness);
	float2 FV_helper = paraFV;

	float3 F0 = SpecularColor;

	float3 FV = F0*FV_helper.x + float3(FV_helper.y, FV_helper.y, FV_helper.y);
	return D * FV;
	
}

#endif