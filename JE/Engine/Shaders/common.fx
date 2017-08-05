#ifndef COMMON_FX
#define COMMON_FX

#define PI 3.14159265359
#define DegreeToRadian 0.01745329252

struct DirectionalLight
{
	float4 Color;
	float3 Direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct PointLight
{
	// Packed into 4D vector: (Position, Range)
	float3 Position;
	float Range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	float3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct SpotLight
{
	// Packed into 4D vector: (Position, Range)
	float3 Position;
	float Range;

	// Packed into 4D vector: (Direction, Spot)
	float3 Direction;
	float Spot;

	// Packed into 4D vector: (Att, Pad)
	float3 Att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

SamplerState TextureSampler
{
	//Filter = MIN_MAG_MIP_LINEAR;
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);	
	
};

SamplerState ShadowTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

};

SamplerState TextureMipSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

};

SamplerState LUTSampler
{
	//Filter = MIN_MAG_MIP_LINEAR;
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	//BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

};

SamplerState ObjTextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};

SamplerState RandomNoiseTextureSampler
{
	Filter = MIN_MAG_MIP_POINT;	
	AddressU = WRAP;
	AddressV = WRAP;
	AddressW = WRAP;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};

SamplerState g_samLinearClamp
{
	Filter = MIN_MAG_MIP_LINEAR;
	//Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 8;

	AddressU = WRAP;
	AddressV = WRAP;
};

float3 TangentSpaceToWorldSpace(float3 unitNormalW, float3 tangentW, float3 biW, float3 localNormal)
{
	//Tangnet to Local to World
	float3x3 TBN = float3x3(tangentW, biW, unitNormalW);

	float3 NT = localNormal * 2.0f - 1.0f; 

	//NT.z = NT.z * 0.5f;
	//NT = normalize(NT);

	float3 result = mul(NT, TBN);
	return result;
}

float2 ScreenToView(float2 uv)
{
	return float2((uv.x*2.0f - 1.0f), ((1.0f - uv.y)*2.0f - 1.0f));
}

float2 ViewToScreen(float2 ViewPos)
{
	return float2((ViewPos.x + 1.0f) *0.5f, 1.0f - (ViewPos.y + 1.0f)*0.5f);
}

float4 GetDepth(Texture2D DepthTexture, float2 TexCoord, float4x4 InvViewProj)
{
	//return mul(   float4(ScreenToView(TexCoord), DepthTexture.Sample(TextureSampler, TexCoord).z / DepthTexture.Sample(TextureSampler, TexCoord).w, 1.0f)   *DepthTexture.Sample(TextureSampler, TexCoord).w, InvViewProj);
	
	float4 result = mul(float4(ScreenToView(TexCoord), DepthTexture.Sample(TextureSampler, TexCoord).z / DepthTexture.Sample(TextureSampler, TexCoord).w, 1.0f), InvViewProj);
	return result / result.w;

}

#endif