#include "common.fx"

cbuffer cbPerFrame
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gOriginTranslation;
	float gLength;
	float gSelectedAxis;
	
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
};

struct VertexIn2
{
	float3 LocalPosition  : POSITION;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VertexOut2
{
	float4 PosH    : SV_POSITION;
	float4 VertexColor : COLOR;
	float3 Normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.LocalPosition*gLength, 1.0f), gWorldViewProj);
	vout.PosH = mul(vout.PosH, gOriginTranslation);
	
	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;
    
    return vout;
}

float4 Axis(VertexOut pin) : SV_Target
{
	return pin.VertexColor;
}

VertexOut2 TransVS(VertexIn2 vin)
{
	VertexOut2 vout;

	vout.PosH = mul(float4(vin.LocalPosition*gLength, 1.0f), gWorldViewProj);
	vout.PosH = mul(vout.PosH, gOriginTranslation);

	vout.VertexColor = vin.Color;

	//[flatten]
	if (gSelectedAxis == 1.0f)
	{
		//[flatten]
		if (vout.VertexColor.r == 1.0f)
		{
			vout.VertexColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (vout.VertexColor.r == 0.75f)
		{
			vout.VertexColor = float4(0.75f, 0.75f, 0.0f, 1.0f);
		}
		else
		{
			vout.VertexColor = vin.Color;
		}
	}		
	else if (gSelectedAxis == 2.0f)
	{
		//[flatten]
		if (vout.VertexColor.g == 1.0f)
		{
			vout.VertexColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (vout.VertexColor.g == 0.75f)
		{
			vout.VertexColor = float4(0.75f, 0.75f, 0.0f, 1.0f);
		}
		else
		{
			vout.VertexColor = vin.Color;
		}
	}		
	else if (gSelectedAxis == 3.0f)
	{
		//[flatten]
		if (vout.VertexColor.b == 1.0f)
		{
			vout.VertexColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else if (vout.VertexColor.b == 0.75f)
		{
			vout.VertexColor = float4(0.75f, 0.75f, 0.0f, 1.0f);
		}
		else
		{
			vout.VertexColor = vin.Color;
		}
	}
	else if (gSelectedAxis == 4.0f)
	{
		//[flatten]
		if (vout.VertexColor.r == 0.75f)
		{
			vout.VertexColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		}
		else
		{
			vout.VertexColor = vin.Color;
		}
	}

	vout.Normal = vin.Normal;
	
	return vout;
}

float4 TransPS(VertexOut2 pin) : SV_Target
{
	//float3 LightVec = normalize(float3(1.0f, 0.5f, -1.0f));

	//float diffuse = saturate(dot(pin.Normal, LightVec));

	return pin.VertexColor;
}

technique11 AxisTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );		
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_5_0, Axis()));
    }
}

technique11 TransTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, TransVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, TransPS()));
	}
};