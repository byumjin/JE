#include "common.fx"

cbuffer cbPerFrame
{		
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 LocalPosition  : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 Color : COLOR;
};

TextureCube EnvCubeMap;


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);
	vout.WorldNormal = vin.Normal;
	vout.TexCoord = vin.TexCoord;
	vout.Color = vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{	
	return pin.Color * 0.0f;// 0.1f; // 454545f;// EnvCubeMap01.Sample(samAnisotropic, pin.WorldNormal.xyz);
}

technique11 SkyBoxTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}
