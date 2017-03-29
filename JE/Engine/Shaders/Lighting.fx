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

	//[flatten]
	if (WorldNormal.a < -1.0f)
		return float4(BaseColor);
	

	//BaseColor = lerp(BaseColor, float3(0.25f,0.25f,0.25f), Metallic);

	float3 LightVec = -normalize(gDirLight.Direction);
	float diffuseFactor = saturate(dot(LightVec, WorldNormal.xyz));		
	
	float4 DepthInfo = DepthInfoTexture.Sample(TextureSampler, pin.TexCoord);
	float DepthVal = DepthInfo.z / DepthInfo.w;

	float4 PixelWorldPos = mul(float4(ScreenToView(pin.TexCoord), DepthVal, 1.0f)*DepthInfo.w, gInvDepthViewProj);	

	float3 ViewVec = normalize(gEyePosW.xyz - PixelWorldPos.xyz);
	float3 HalfVec = normalize(ViewVec + LightVec);

	float4 SpecularColor = SpecularTexture.Sample(TextureSampler, pin.TexCoord);

	float Roughness = min( max(SpecularColor.w, 0.05f), 0.95f);

	float com = sin(Roughness*3.141592f);

	float Roughness_sqrt = sqrt(Roughness);
	Roughness_sqrt = sqrt(Roughness_sqrt);

	Roughness = lerp(Roughness, Roughness_sqrt, com);

	float4 ShadowCoord =  mul(PixelWorldPos, gShadowVP);
	
	ShadowCoord.x = (ShadowCoord.x + 1.0f) * 0.5f;
	ShadowCoord.y = 1.0f - ((ShadowCoord.y + 1.0f) * 0.5f);

	

	float LoH = saturate(dot(LightVec, HalfVec));
	float NoV = saturate(dot(WorldNormal.xyz, ViewVec));

	//float energyConservationSpec = 1.0f - Roughness;
	float energyConservation = 1.0f - Roughness*0.8f;

	float energyConservation2 = 1.0f - Roughness*0.2f;

	float3 spec = GGX_Spec(WorldNormal.xyz, HalfVec, Roughness, 0.0f, BaseColor.xyz, SpecularColor.xyz, BRDFLUTTexture.Sample(LUTSampler, float2(LoH, Roughness)).zw) * energyConservation;


	float2 EnvBRDF = BRDFLUTTexture.Sample(LUTSampler, float2(NoV, Roughness)).xy;

	//float level = 10 * SpecularColor.a;

	Metallic = sqrt(Metallic);

	float4 Reflection = ReflectionTexture.SampleLevel(TextureMipSampler, pin.TexCoord, 0) * (float4(lerp(SpecularColor.rgb, BaseColor.rgb, Metallic), 1.0f)
		* EnvBRDF.x + EnvBRDF.y);// *lerp(energyConservation, energyConservation2, Metallic);

	float4 result = (float4(lerp(BaseColor.xyz, BaseColor.xyz*0.05f, Metallic), 1.0f) + float4(spec, 1.0f))* diffuseFactor + Reflection*lerp(0.15f, 1.0f, Metallic);

	//float Bias = 100000 / pow(2, 32) + 1.0*0.0001f;
	
	result.a = Opacity;

	if (ShadowCoord.z > ShadowTexture.Sample(ShadowTextureSampler, ShadowCoord.xy).x + 0.00001f * DepthInfo.z)
		return lerp(float4(0, 0, 0, Opacity), result, 0.5f);
	else	
		return  result;
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
