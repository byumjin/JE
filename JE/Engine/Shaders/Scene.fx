#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gViewProj;
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
	//float3 WorldNormal : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 VertexColor : COLOR;
};

Texture2D SceneMap;


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
	return SceneMap.Sample(TextureSampler, pin.TexCoord);	
}

technique11 SceneTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}
