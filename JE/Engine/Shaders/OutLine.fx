#include "common.fx"

cbuffer cbPerFrame
{
	
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float2 gTextureSize;
	
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

Texture2D StencilTexture;

VertexOut VSforFillSilhouette(VertexIn vin)
{
	VertexOut vout;

	//Not WorldPosition, It is Localposition
	vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gWorldViewProj);

	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;

	return vout;
}

float FillSilhouette(VertexOut pin) : SV_Target
{
	return 1.0f;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gViewProj);

	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;

	return vout;
}

bool CheckOutline(float2 UV)
{
	//[flatten]
	if (UV.x < 0.0f || UV.y < 0.0f)
		return false;
	else
	{
		return (StencilTexture.Sample(TextureSampler, UV).x >= 1.0f);
			     
	}
}

float4 DrawOutline(VertexOut pin) : SV_Target
{	 	
	float padx = 1.0f / gTextureSize.x;
	float pady = 1.0f / gTextureSize.y;

	float padx2 = padx*2.0f;
	float pady2 = pady*2.0f;

	//[flatten]
	if ((StencilTexture.Sample(TextureSampler, pin.TexCoord)).x >= 1.0f)
	{
		return float4(0.0f, 0.5f, 1.0f, 0.2f);
	}
	else if ((
		CheckOutline(pin.TexCoord + float2(-padx2, -pady2)) ||
		CheckOutline(pin.TexCoord + float2(-padx2, 0.0f)) ||
		CheckOutline(pin.TexCoord + float2(-padx2, pady2)) ||
		CheckOutline(pin.TexCoord + float2(0.0f, -pady2)) ||
		CheckOutline(pin.TexCoord + float2(0.0f, pady2)) ||
		CheckOutline(pin.TexCoord + float2(padx2, -pady2)) ||
		CheckOutline(pin.TexCoord + float2(padx2, 0.0f)) ||
		CheckOutline(pin.TexCoord + float2(padx2, pady2)) ||

		CheckOutline(pin.TexCoord + float2(-padx, -pady2)) ||
		CheckOutline(pin.TexCoord + float2(padx, -pady2)) ||
		CheckOutline(pin.TexCoord + float2(-padx2, -pady)) ||
		CheckOutline(pin.TexCoord + float2(-padx2, pady)) ||
		CheckOutline(pin.TexCoord + float2(padx2, -pady)) ||
		CheckOutline(pin.TexCoord + float2(padx2, pady)) ||
		CheckOutline(pin.TexCoord + float2(-padx, pady2)) ||
		CheckOutline(pin.TexCoord + float2(padx, pady2)) ||
		
		CheckOutline(pin.TexCoord + float2(-padx, -pady)) ||
		CheckOutline(pin.TexCoord + float2(-padx, 0.0f)) ||
		CheckOutline(pin.TexCoord + float2(-padx, pady)) ||
		CheckOutline(pin.TexCoord + float2(0.0f, -pady)) ||
		CheckOutline(pin.TexCoord + float2(0.0f, pady)) ||
		CheckOutline(pin.TexCoord + float2(padx, -pady)) ||
		CheckOutline(pin.TexCoord + float2(padx, 0.0f)) ||
		CheckOutline(pin.TexCoord + float2(padx, pady))
		))
	{
		return float4(1.0f, 0.5f, 0.0f, 1.0f);
	}
	else
		return float4(0.0f, 0.0f, 0.0f, 0.0f);	
}

technique11 FillSilhouetteTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VSforFillSilhouette()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, FillSilhouette()));
	}
}

technique11 OutLineTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, DrawOutline()));
	}
}
