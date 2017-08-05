#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gViewProj;
	float4x4 gInvProj;
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

Texture2D SceneMap;
Texture2D DepthMap;
Texture2D BlurMap;

float4 vDofParams;

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

float4 GaussianHorizontal(VertexOut pin) : SV_Target
{
	int level = 1;

	float4 gl_FragColor = float4(0.0f,0.0f,0.0f,0.0f);

	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor07, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor07, 0.0f)).r *0.0044299121055113265;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor06, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor06, 0.0f)).r *0.00895781211794;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor05, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor05, 0.0f)).r *0.0215963866053;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor04, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor04, 0.0f)).r *0.0443683338718;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor03, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor03, 0.0f)).r *0.0776744219933;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor02, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor02, 0.0f)).r *0.115876621105;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor01, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(-factor01, 0.0f)).r *0.147308056121;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord, level) * BlurMap.Sample(TextureSampler, pin.TexCoord) *0.159576912161;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor01, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor01, 0.0f)).r *0.147308056121;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor02, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor02, 0.0f)).r *0.115876621105;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor03, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor03, 0.0f)).r *0.0776744219933;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor04, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor04, 0.0f)).r *0.0443683338718;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor05, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor05, 0.0f)).r *0.0215963866053;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor06, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor06, 0.0f)).r *0.00895781211794;
	gl_FragColor += SceneMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor07, 0.0f), level) * BlurMap.Sample(TextureSampler, pin.TexCoord + float2(factor07, 0.0f)).r *0.0044299121055113265;

	return gl_FragColor;
}

float4 GaussianVertical(VertexOut pin) : SV_Target
{
	int level = 1;

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

	return gl_FragColor;
}


float ComputeDepthBlur(float depth)
{
	float f;

	if (depth < vDofParams.y)
	{
		f = (vDofParams.y - depth) / (vDofParams.y - vDofParams.x);
	}
	else
	{
		f = (depth - vDofParams.y) / (vDofParams.z - vDofParams.y);
		f *= vDofParams.w;
	}

	f *= f;

	return saturate(f);
}


float4 PS(VertexOut pin) : SV_Target
{

	float4 DepthInfo = DepthMap.Sample(TextureSampler, pin.TexCoord);

	float4 posInCamera = mul(float4(ScreenToView(pin.TexCoord), DepthInfo.z / DepthInfo.w, 1.0f), gInvProj);
	posInCamera = posInCamera / posInCamera.w;

	float d = ComputeDepthBlur(posInCamera.z);	

	return float4(BlurMap.Sample(TextureSampler, pin.TexCoord).xyz, d);
}

technique11 DOFTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}

technique11 HBlurTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, GaussianHorizontal()));
	}
}

technique11 VBlurTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, GaussianVertical()));
	}
}