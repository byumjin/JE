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
Texture2D BasicColorMap;

float factor01;
float factor02;
float factor03;
float factor04;
float factor05;
float factor06;
float factor07;



VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gViewProj);
	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;
	return vout;
}

float4 GaussianVertical(VertexOut pin) : SV_Target
{
	int level = 2;

	float4 gl_FragColor = float4(0.0f,0.0f,0.0f,0.0f);
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor07, 0.0f), level)*0.0044299121055113265;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor06, 0.0f), level)*0.00895781211794;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor05, 0.0f), level)*0.0215963866053;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor04, 0.0f), level)*0.0443683338718;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor03, 0.0f), level)*0.0776744219933;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor02, 0.0f), level)*0.115876621105;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor01, 0.0f), level)*0.147308056121;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord, level)*0.159576912161;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor01, 0.0f), level)*0.147308056121;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor02, 0.0f), level)*0.115876621105;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor03, 0.0f), level)*0.0776744219933;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor04, 0.0f), level)*0.0443683338718;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor05, 0.0f), level)*0.0215963866053;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor06, 0.0f), level)*0.00895781211794;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor07, 0.0f), level)*0.0044299121055113265;
	return gl_FragColor*5.0f;
}

float4 GaussianHeight(VertexOut pin) : SV_Target
	{
		int level = 2;

		float4 gl_FragColor = float4(0.0f,0.0f,0.0f,0.0f);
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor07), level)*0.0044299121055113265;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor06), level)*0.00895781211794;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor05), level)*0.0215963866053;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor04), level)*0.0443683338718;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor03), level)*0.0776744219933;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor02), level)*0.115876621105;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor01), level)*0.147308056121;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord, level)*0.159576912161;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor01), level)*0.147308056121;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor02), level)*0.115876621105;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor03), level)*0.0776744219933;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor04), level)*0.0443683338718;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor05), level)*0.0215963866053;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor06), level)*0.00895781211794;
		gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor07), level)*0.0044299121055113265;
		return gl_FragColor*5.0f;

		//return float4(pin.TexCoord, 0.0f, 0.0f);
	}

float4 PS(VertexOut pin) : SV_Target
{
	float4 rgba = SceneMap.Sample(TextureSampler, pin.TexCoord);
	float4 ori = BasicColorMap.Sample(TextureSampler, pin.TexCoord);


	//[flatten]
	if (rgba.r > 1.0f || rgba.g > 1.0f || rgba.b > 1.0f)
	{
		
		rgba = max((rgba - 1.0f), 0.0f)*0.2f;
		
		
		

		//rgba = rgba * 0.01f;

		return float4(rgba.xyz, 1.0f) * ori;
	}
	
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

technique11 BloomTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}

technique11 GaussianVerticalTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, GaussianVertical()));
	}
}

technique11 GaussianHeightTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, GaussianHeight()));
	}
}
