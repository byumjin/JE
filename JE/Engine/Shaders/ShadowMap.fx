#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gWorldViewProj;
	float4x4 gViewProj;
};

struct VertexIn
{
	float3 LocalPosition  : POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	//float3 depth : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 VertexColor : COLOR;
	

	float3 depth : POSITION;
};

//Texture2D ShadowMap;


VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	//vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gViewProj);
	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);

	
	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;

	//float4 dep = float4(vout.PosH.x / vout.PosH.w, vout.PosH.y / vout.PosH.w, vout.PosH.z / vout.PosH.w, 1.0f);	
	//vout.depth = float3(dep.x, dep.y, dep.z);
	vout.depth = float3(vout.PosH.x, vout.PosH.y, vout.PosH.z);
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{	
	float dep;
	dep = pin.depth.z;
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	return float4(dep, dep, dep, 1.0f);
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
}


technique11 ShadowMapTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));

	}	
}
