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
	float2 TexCoord : TEXCOORD;
	float4 VertexColor : COLOR;
	float4 depth : TEXTURE0;
};

//Texture2D ShadowMap;


VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);	
	vout.depth = float4(vin.LocalPosition, 1.0f);

	return vout;
}

float2 PS(VertexOut pin) : SV_Target
{	
	float dep;	
	dep = mul(pin.depth, gWorldViewProj).z;
	return dep;
	/*
	float dx = ddx(dep);
	float dy = ddy(dep);

	return float2(dep, pow(dep, 2.0f) + 0.25f*(dx*dx + dy*dy));
	*/
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
