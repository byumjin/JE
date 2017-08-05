#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gViewProj;
	float4x4 gView;
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
	
	float4 VertexColor : COLOR;
	float2 TexCoord : TEXCOORD;
};

Texture2D SceneMap;
Texture2D HighLightMap;

Texture2D DirtMaskMap;
Texture2D StarBurstMap;


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
	return gl_FragColor;
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



		

		//float3 camx = cam.getViewMatrix().col(0); // camera x (left) vector

		float3 camx = mul(float3(1.0f, 0.0f, 0.0f), (float3x3)gView);

		//float3 camz = cam.getViewMatrix().col(1); // camera z (forward) vector
		
		float3 camz = mul(float3(0.0f, 1.0f, 0.0f), (float3x3)gView);
		
		float camrot = dot(camx, float3(0, 0, 1)) + dot(camz, float3(0, 1, 0));
		camrot *= -3.0f;
		
		float3x3 scaleBias1 = {
			2.0f, 0.0f, -1.0f,
			0.0f, 2.0f, -1.0f,
			0.0f, 0.0f, 1.0f
		};
	
		float3x3 scaleBias2 = {
			0.5f, 0.0f, 0.5f,
			0.0f, 0.5f, 0.5f,
			0.0f, 0.0f, 1.0f
		};

		float3x3 rotation = {
		 cos(camrot), -sin(camrot), 0.0f,
		 sin(camrot), cos(camrot), 0.0f ,
		 0.0f, 0.0f, 1.0f
		};

		float3 st1 = float3(pin.TexCoord, 1.0f) * 2.0  - float3(1.0f, 1.0f, 1.0f);
		st1.z = 1.0f;
		float3 st2 = mul(st1, rotation);
		st2.z = 1.0f;

		float3 st3 = st2 * 0.5 + float3(0.5f, 0.5f, 0.5f);
		
		float2 lensStarTexcoord = st3.xy;// mul(float3(pin.TexCoord, 1.0f), mul(scaleBias2, mul(rotation, scaleBias1))).xy;
		
		
		//return float4(lensStarTexcoord, 0.0f, 1.0f);
		return gl_FragColor*5.0f * (DirtMaskMap.Sample(TextureMipSampler, pin.TexCoord) * 0.9f + 0.1f) * (StarBurstMap.Sample(TextureMipSampler, lensStarTexcoord));
		//return gl_FragColor* (DirtMaskMap.Sample(TextureMipSampler, pin.TexCoord) * 0.9f + 0.1f) * (StarBurstMap.Sample(TextureMipSampler, pin.TexCoord) * 0.8f + 0.2f);
		//return float4(pin.TexCoord, 0.0f, 0.0f);
	}

	
	float4 textureDistorted(Texture2D tex,
							float2 texcoord,
							float2 direction, // direction of distortion
							float3 distortion)
	{
		
		
		return float4
			(
			tex.Sample(TextureMipSampler, texcoord + direction * distortion.r).r,
			tex.Sample(TextureMipSampler, texcoord + direction * distortion.g).g,
			tex.Sample(TextureMipSampler, texcoord + direction * distortion.b).b,
				0.0f
			);
			
		
	}
	

float4 PS(VertexOut pin) : SV_Target
{

	

	//float4 rgba = SceneMap.Sample(TextureSampler, pin.TexCoord);
	//float4 ori = HighLightMap.Sample(TextureSampler, pin.TexCoord);

	
	

	int uGhosts = 4; // number of ghost samples
	float uGhostDispersal = 0.4; // dispersion factor

	float uDistortion = 5.0f;
	

	float2 texcoord = -pin.TexCoord + float2(1.0, 1.0);

	float2 texelSize = float2(factor01, factor01);
	float3 distortion = float3(-texelSize.x * uDistortion, 0.0, texelSize.x * uDistortion);

	


	// ghost vector to image centre:
	float2 ghostVec = (float2(0.5, 0.5) - texcoord) * uGhostDispersal;
	
	float3 direction = normalize(float3(ghostVec, 0.0));

	// sample ghosts:  
	float4 result = float4(0,0,0,0);
	float4 ghost = float4(0, 0, 0, 0);
	
	for (int i = 0; i < uGhosts; ++i)
	{
		float2 offset = frac(texcoord + ghostVec * float(i));

		//float weight = length(float2(0.5, 0.5) - offset) / length(float2(0.5, 0.5));
		//weight = pow(1.0 - saturate(weight), 5.0);

		//result += (textureDistorted(HighLightMap, offset, direction.xy, distortion)* weight * 0.3) / (float)((i+1)*(i+1));
		//result += HighLightMap.Sample(TextureMipSampler, offset) * weight * 0.3;//   texture(uInputTex, offset);
		//result += textureDistorted(HighLightMap, offset, direction.xy, distortion) * weight / (float)((i + 1)*(i + 1));
		ghost += textureDistorted(HighLightMap, offset, direction.xy, distortion);
	}

	float weightLens = length(float2(0.5, 0.5) - texcoord) / length(float2(0.5, 0.5));
	weightLens = pow( saturate(1.0 - saturate(weightLens)), 3.0);
	weightLens *= 2.0f;
	//return float4(weightLens, weightLens, weightLens, 1.0f);

	ghost *= weightLens;

	//return result;
	

	// sample halo:

	float uHaloWidth = 0.4;

	float2 haloVec = normalize(ghostVec) * uHaloWidth;
	float weight = length(float2(0.5, 0.5) - frac(texcoord + haloVec)) / length(float2(0.5, 0.5));
	
	weight = pow(1.0f - saturate(weight), 50.0f);
	
	//weight = 1.0f - saturate(weight);

	//return float4(weight, weight, weight, 1.0f);

	//result += textureDistorted(SceneMap, texcoord + haloVec, direction.xy, distortion) * textureDistorted(HighLightMap, texcoord + haloVec, direction.xy, distortion) * pow(weight, 3) * 0.6f;
	result = textureDistorted(SceneMap, texcoord + haloVec, direction.xy, distortion) * textureDistorted(HighLightMap, texcoord + haloVec, direction.xy, distortion) *weight;
	result += ghost;
	//result += SceneMap.Sample(TextureMipSampler, texcoord + haloVec) * 0.1 * pow(weight,6) * 0.3;
	

	//result *= DirtMaskMap.Sample(TextureMipSampler, pin.TexCoord).r;

	return result;
}

technique11 LensFlareTech
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
