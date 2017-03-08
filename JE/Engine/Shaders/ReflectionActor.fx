#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gInvDepthViewProj;
	
	float3 gWorldCenterPosition;
	float3 gEyePosW;

	float gEnvCubeMapCount;
};

struct VertexIn
{
	float3 LocalPosition  : POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 Normal : NORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);
	vout.PosW = mul(float4(vin.LocalPosition, 1.0f), gWorld).xyz;

	vout.Color = vin.Color;
	vout.TexCoord = vin.TexCoord;
	vout.Normal = vin.Normal;
	return vout;	
}


Texture2D DiffuseMap;
TextureCube SkyBoxCubeMap;

Texture2D NormalMap;
Texture2D SpecularMap;


TextureCube EnvCubeMap01;
TextureCube EnvCubeMap02;
TextureCube EnvCubeMap03;
TextureCube EnvCubeMap04;
TextureCube EnvCubeMap05;
TextureCube EnvCubeMap06;


float4 PS(VertexOut pin) : SV_Target
{
	//float3 uvw = float3(pin.Tex, pin.PrimID % 4);
	return float4( DiffuseMap.Sample(ObjTextureSampler, pin.TexCoord));
}

VertexOut SkyBoxVS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);
	vout.PosW = mul(float4(vin.LocalPosition, 1.0f), gWorld).xyz;

	vout.Color = vin.Color;
	vout.TexCoord = vin.TexCoord;
	vout.Normal = vin.Normal;
	return vout;
}


float4 SkyBoxPS(VertexOut pin) : SV_Target
{
	return  float4(SkyBoxCubeMap.SampleLevel(samAnisotropic, normalize(pin.PosW), 0).xyz, 0.0f);

/*
float4 DepthInfo = WPMap.Sample(TextureSampler, pin.TexCoord);
float4 PixelWorldPos = mul(float4(ScreenToView(pin.TexCoord), DepthInfo.z / DepthInfo.w, 1.0f)*DepthInfo.w, gInvDepthViewProj);

float3 ViewVec = normalize(gEyePosW.xyz - PixelWorldPos.xyz);

float3 WM = WNMap.Sample(TextureSampler, pin.TexCoord).xyz;

return float4(WM, 1.0f);

float3 WorldNormal = WM*2.0f-1.0f;



float3 reflectVec = -ViewVec + WorldNormal*2.0f*saturate(dot(ViewVec, WorldNormal));

return  float4(SkyBoxCubeMap.SampleLevel(samAnisotropic, normalize(reflectVec), 0).xyz, 0.0f);
*/
}

struct VertexInforCompositionCubeMapPS
{
	float3 LocalPosition  : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOutforCompositionCubeMapPS
{
	float4 PosH    : SV_POSITION;
	float3 PosW : POSITION;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
	float3 NormalW : NORMAL;
	float2 TexCoord : TEXCOORD;
	float4 VertexColor : COLOR;
	float4 depthPosition : TEXTURE0;
};

VertexOutforCompositionCubeMapPS CompositionCubeMapVS(VertexInforCompositionCubeMapPS vin)
{
	VertexOutforCompositionCubeMapPS vout;

	vout.PosW = mul(float4(vin.LocalPosition, 1.0f), gWorld).xyz;

	vout.BinormalW = mul(vin.BiNormal, (float3x3)gWorld);
	vout.TangentW = mul(vin.Tangent, (float3x3)gWorld);
	vout.NormalW = mul(vin.Normal, (float3x3)gWorld);

	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);

	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;

	vout.depthPosition = vout.PosH;

	return vout;
}

float4 CompositionCubeMapPS(VertexOutforCompositionCubeMapPS pin) : SV_Target
{
	// ** SPECULAR ** //
	//, a is for roughness
	float4 Specular = SpecularMap.Sample(ObjTextureSampler, pin.TexCoord);
	//Specular.a = 0.10f;

	//float level = 7 * pow(Specular.a,10);
	float level = 7 * pow(Specular.a, 0.25f);
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//float level = 0;

	// ** NORMAL ** //
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);
	pin.TangentW = normalize(pin.TangentW);
	pin.BinormalW = normalize(pin.BinormalW);

	float3 WorldNormal = TangentSpaceToWorldSpace(pin.NormalW, pin.TangentW, pin.BinormalW, NormalMap.Sample(ObjTextureSampler, pin.TexCoord).xyz);
	WorldNormal = normalize(WorldNormal);
	//float4 PixelWorldPos = mul(float4(pin.depthPosition.xy, pin.depthPosition.z / pin.depthPosition.w, 1.0f)*pin.depthPosition.w, gInvDepthViewProj);
	float4 PixelWorldPos = mul(pin.depthPosition, gInvDepthViewProj);
	float3 ViewVec = normalize(gEyePosW.xyz - PixelWorldPos.xyz);




	

	float3 reflectVec = -ViewVec + (2.0f * saturate(dot(ViewVec, WorldNormal)) * WorldNormal);
	
	//return float4(reflectVec, 1.0f);

	reflectVec = normalize(reflectVec);

	//return  EnvCubeMap01.SampleLevel(samAnisotropic, reflectVec, level);// float4(reflectVec, 1.0f);

	// ** Reflection ** //
	float4 rE = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 rr = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	if (gEnvCubeMapCount >= 1.0f)
	{
		if (gEnvCubeMapCount >= 2.0f)
		{
			if (gEnvCubeMapCount >= 3.0f)
			{
				if (gEnvCubeMapCount >= 4.0f)
				{
					if (gEnvCubeMapCount >= 5.0f)
					{
						if (gEnvCubeMapCount >= 6.0f)
						{
							rr = EnvCubeMap06.SampleLevel(samAnisotropic, reflectVec, level);
							//rr = EnvCubeMap06.Sample(samAnisotropic, reflectVec);
							//[flatten]
							if (gEnvCubeMapCount <= 6.0f)
								rE = rr;
							else if (rr.a > 0.0f)
								rE = rr;
						}

						rr = EnvCubeMap05.SampleLevel(samAnisotropic, reflectVec, level);
						//rr = EnvCubeMap05.Sample(samAnisotropic, reflectVec);
						//[flatten]
						if (gEnvCubeMapCount <= 5.0f)
							rE = rr;
						else if (rr.a > 0.0f)
							rE = rr;
					}

					rr = EnvCubeMap04.SampleLevel(samAnisotropic, reflectVec, level);
					//rr = EnvCubeMap04.Sample(samAnisotropic, reflectVec);
					//[flatten]
					if (gEnvCubeMapCount <= 4.0f)
						rE = rr;
					else if (rr.a > 0.0f)
						rE = rr;
				}

				rr = EnvCubeMap03.SampleLevel(samAnisotropic, reflectVec, level);
				//rr = EnvCubeMap03.Sample(samAnisotropic, reflectVec);

				//[flatten]
				if (gEnvCubeMapCount <= 3.0f)
					rE = rr;
				else if (rr.a > 0.0f)
					rE = rr;
			}

			rr = EnvCubeMap02.SampleLevel(samAnisotropic, reflectVec, level);
			//rr = EnvCubeMap02.Sample(samAnisotropic, reflectVec);
			//[flatten]
			if (gEnvCubeMapCount <= 2.0f)
				rE = rr;
			else if (rr.a > 0.0f)
				rE = rr;
		}

		rr = EnvCubeMap01.SampleLevel(samAnisotropic, reflectVec, level);
		//rr = EnvCubeMap01.Sample(samAnisotropic, reflectVec);
		//[flatten]
		if (gEnvCubeMapCount <= 1.0f)
			rE = rr;
		else if (rr.a > 0.0f)
			rE = rr;
	}


	return rE;
}

technique11 ReflectionActorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		/*SetGeometryShader(CompileShader(gs_5_0, GS()));*/
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}

technique11 SkyBoxTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, SkyBoxVS()));
		/*SetGeometryShader(CompileShader(gs_5_0, GS()));*/
		SetPixelShader(CompileShader(ps_5_0, SkyBoxPS()));
	}
}

technique11 CompositionCubeMapTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, CompositionCubeMapVS()));
		/*SetGeometryShader(CompileShader(gs_5_0, GS()));*/
		SetPixelShader(CompileShader(ps_5_0, CompositionCubeMapPS()));
	}
}



