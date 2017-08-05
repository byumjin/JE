#include "common.fx"
#include "BRDF.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	
	float4x4 gWorld;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gInvViewProj;
	float4x4 gInvProj;
	float3 gEyePosW;
	float gTime;	

	float4x4 gShadowVP;
	float4x4 gInvDepthViewProj;
};

struct VertexIn
{
	float3 WorldPosition  : POSITION;	
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;	
};

struct VertexOut
{
	float4 PosH    : SV_POSITION; 	
	float2 TexCoord : TEXCOORD;
    float4 VertexColor : COLOR;
};

Texture2D BasicColorTexture;
Texture2D SpecularTexture;
Texture2D NormalTexture;

Texture2D ReflectionTexture;
Texture2D DepthInfoTexture;
Texture2D BRDFLUTTexture;
Texture2D ShadowTexture;


float linstep(float low, float high, float v)
{
	return clamp((v - low) / (high - low), 0.0, 1.0);
}

float VSM(float2 uv, float compare)
{
	float2 moments = ShadowTexture.Sample(ShadowTextureSampler, uv);
	float p = smoothstep(compare - 0.02f, compare, moments.x);
	float variance = max(moments.y - moments.x*moments.x, -0.001f);
	float d = compare - moments.x;
	float p_max = linstep(0.2f, 1.0, variance / (variance + d*d));
	return clamp(max(p, p_max), 0.0f, 1.0f);
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gViewProj);
	
	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 BaseColor = BasicColorTexture.Sample(TextureSampler, pin.TexCoord);

	float Opacity = BaseColor.a;
	
	float4 WorldNormal = NormalTexture.Sample(TextureSampler, pin.TexCoord)*2.0f - 1.0f;

	float Metallic = NormalTexture.Sample(TextureSampler, pin.TexCoord).a;

	if (WorldNormal.a < -1.0f)
		return float4(BaseColor);

	float3 LightVec = -normalize(gDirLight.Direction);
	float diffuseFactor = saturate(dot(LightVec, WorldNormal.xyz));		
	
	float4 DepthInfo = DepthInfoTexture.Sample(TextureSampler, pin.TexCoord);
	float DepthVal = DepthInfo.z / DepthInfo.w;

	float4 PixelWorldPos = mul(float4(ScreenToView(pin.TexCoord), DepthVal, 1.0f), gInvDepthViewProj);	
	PixelWorldPos /= PixelWorldPos.w;

	float3 ViewVec = normalize(gEyePosW.xyz - PixelWorldPos.xyz);
	float3 HalfVec = normalize(ViewVec + LightVec);

	float4 SpecularColor = SpecularTexture.Sample(TextureSampler, pin.TexCoord);

	float Roughness = min( max(SpecularColor.w, 0.05f), 0.95f);

	float com = sin(Roughness*3.141592f);

	float Roughness_sqrt = sqrt(Roughness);
	Roughness_sqrt = sqrt(Roughness_sqrt);

	Roughness = lerp(Roughness, Roughness_sqrt, com);

	float4 ShadowCoord =  mul(PixelWorldPos, gShadowVP);
	
	float ShadowDepth = ShadowCoord.z;

	ShadowCoord / ShadowCoord.w;

	ShadowCoord.x = (ShadowCoord.x + 1.0f) * 0.5f;
	ShadowCoord.y = 1.0f - ((ShadowCoord.y + 1.0f) * 0.5f);

	float LoH = saturate(dot(LightVec, HalfVec));
	float NoV = saturate(dot(WorldNormal.xyz, ViewVec));

	
	float energyConservation = 1.0f - Roughness*0.8f;
	float energyConservation2 = 1.0f - Roughness*0.2f;

	float3 spec = GGX_Spec(WorldNormal.xyz, HalfVec, Roughness, 0.0f, BaseColor.xyz, SpecularColor.xyz, BRDFLUTTexture.Sample(LUTSampler, float2(LoH, Roughness)).zw) * energyConservation;


	float2 EnvBRDF = BRDFLUTTexture.Sample(LUTSampler, float2(NoV, Roughness)).xy;



	Metallic = sqrt(Metallic);

	float4 Reflection = ReflectionTexture.SampleLevel(TextureMipSampler, pin.TexCoord, 0) * (float4(lerp(SpecularColor.rgb, BaseColor.rgb, Metallic), 1.0f)
		* EnvBRDF.x + EnvBRDF.y);

	float4 BasicColor = float4(lerp(BaseColor.xyz, BaseColor.xyz*0.2f, Metallic), 1.0f);
	float4 ReflectionColor = Reflection*lerp(0.15f, 1.0f, Metallic);
	
	float4 result = (BasicColor + float4(spec, 1.0f))* diffuseFactor + ReflectionColor;	
	
	//result.xyz *= gDirLight.Color.xyz;
	//result.xyz *= gDirLight.Color.a;
	
	result.xyz *= float3(0.9f, 0.98f, 1.0f);
	result.xyz *= 1.25f;

	result.xyz = clamp(result.xyz, 0.0f, 2.0f);

	result.a = Opacity;



	float f = 1000000.0f;
	float n = 0.1;
	float z = (2 * n) / (f + n - DepthVal * (f - n));
	float zz = z*1000.0f;

	float VSM_depth = VSM(ShadowCoord.xy, 0.02f);

	if (diffuseFactor > 0.0f && (ShadowDepth > ShadowTexture.Sample(ShadowTextureSampler, ShadowCoord.xy).x + 0.01f * zz /*0.005f*/))
		result = lerp(float4(0, 0, 0, Opacity), saturate(result), 0.7f);
	
	//if (diffuseFactor <= 0.0f || ShadowDepth > VSM_depth + 0.01f * zz)
	//	result = lerp(float4(0, 0, 0, Opacity), saturate(result), 0.5f);

	return lerp(result, result + BaseColor, BaseColor.a);

	//Fog
	/*
	float4 fogColor = float4(0.741f, 0.98f, 1.0f, 1.0f);

	float fd = saturate(1.0f - zz*10.0f);


	return lerp(fogColor, result, fd*fd);
	*/
}


technique11 DirLightsTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );		
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );		
    }
}
