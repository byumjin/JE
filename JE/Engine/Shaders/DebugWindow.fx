#include "common.fx"

cbuffer cbPerFrame
{
	float4x4 gWorld;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gInvViewProj;
	float4x4 gInvProj;
	float4x4 gInvDepthViewProj;
	float3 gEyePosW;
	float gTime;	
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

Texture2D BasicTexture;
Texture2D SpecularTexture;
Texture2D NormalTexture;
Texture2D DepthTexture;
Texture2D SelectedTexture;
Texture2D UVTexture;
Texture2D EnvTexture;
Texture2D BloomTexture;
Texture2D ShadowTexture;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gViewProj);
	
	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;
    
    return vout;
}

float4 Basic(VertexOut pin) : SV_Target
{
	return  BasicTexture.Sample(TextureSampler, pin.TexCoord);
}

float4 Spec(VertexOut pin) : SV_Target
{
	return  SpecularTexture.Sample(TextureSampler, pin.TexCoord);
}

float4 Normal(VertexOut pin) : SV_Target
{
	return  NormalTexture.Sample(TextureSampler, pin.TexCoord);
}

float4 Roughness(VertexOut pin) : SV_Target
{
	float r = SpecularTexture.Sample(TextureSampler, pin.TexCoord).a;
	return float4(r, r, r, 1.0f);
}

float4 Metallic(VertexOut pin) : SV_Target
{
	float m = NormalTexture.Sample(TextureSampler, pin.TexCoord).a;
	return float4(m, m, m, 1.0f);
}

float4 WorldPosition(VertexOut pin) : SV_Target
{
	float4 DepthInfo = DepthTexture.Sample(TextureSampler, pin.TexCoord);
	float4 PixelWorldPos = mul(float4(ScreenToView(pin.TexCoord), DepthInfo.z / DepthInfo.w, 1.0f)*DepthInfo.w, gInvDepthViewProj);
	//PixelWorldPos = PixelWorldPos / PixelWorldPos.w;
	return PixelWorldPos;
	
	//return GetDepth(DepthTexture, pin.TexCoord, gInvViewProj);
}

float4 Depth(VertexOut pin) : SV_Target
{
	float d = DepthTexture.Sample(TextureSampler, pin.TexCoord).z / DepthTexture.Sample(TextureSampler, pin.TexCoord).w;
	float f = 1000000.0f;
	float n = 0.1;
	float z = (2 * n) / (f + n - d * (f - n));
	z = z*1000.0f;
	//d = pow(abs(d), 200.0f);

	return float4(z, z, z, 1.0f);
}

float4 Selected(VertexOut pin) : SV_Target
{
	return  SelectedTexture.Sample(TextureSampler, pin.TexCoord);	
}

float4 UV(VertexOut pin) : SV_Target
{
	return  UVTexture.Sample(TextureSampler, pin.TexCoord);	
}

float4 Enviroment(VertexOut pin) : SV_Target
{
	float4 SpecularColor = SpecularTexture.Sample(TextureSampler, pin.TexCoord);
	//float level = 7 * SpecularColor.a;
	return  EnvTexture.SampleLevel(TextureMipSampler, pin.TexCoord, 0);
}

float4 Bloom(VertexOut pin) : SV_Target
{
	return  BloomTexture.Sample(TextureSampler, pin.TexCoord);
}


float Shadow(VertexOut pin) : SV_Target
{
	return  ShadowTexture.Sample(TextureSampler, pin.TexCoord);
}


technique11 BasicTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );		
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_5_0, Basic()));
    }
}

technique11 SpecTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Spec()));
	}
}


technique11 NormalTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Normal()));
	}
}

technique11 RoughnessTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Roughness()));
	}
}

technique11 MetallicTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Metallic()));
	}
}

technique11 WorldPositionTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, WorldPosition()));
	}
}

technique11 DepthTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Depth()));
	}
}

technique11 SelectedTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Selected()));
	}
}

technique11 OpacityTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, UV()));
	}
}

technique11 AOTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Shadow()));
	}
}
technique11 EnvTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Enviroment()));
	}
}

technique11 BloomTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Bloom()));
	}
}

technique11 Custom01Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, UV()));
	}
}
/*
technique11 Custom02Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, Basic()));
	}
}
*/
