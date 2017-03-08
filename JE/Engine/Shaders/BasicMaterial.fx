#include "common.fx"

cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	float4x4 gWorld;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gInvViewProj;
	float4x4 gInvProj;
	float3 gEyePosW;
	float gTime;
	
	

	bool bSelected;

};

struct VertexIn
{
	float3 LocalPosition  : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 BiNormal : BINORMAL;
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

struct VertexOut
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

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;


TextureCube SkyBoxCubeMap;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosW = mul(float4(vin.LocalPosition, 1.0f), gWorld).xyz;

	/*vout.BinormalW =  mul(vin.BiNormal, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(vin.Tangent, (float3x3)gWorldInvTranspose);
	vout.NormalW = mul(vin.Normal, (float3x3)gWorldInvTranspose);*/

	vout.BinormalW =  vin.BiNormal;
	vout.TangentW = vin.Tangent;
	vout.NormalW = vin.Normal;

	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);

	vout.TexCoord = vin.TexCoord;
	vout.VertexColor = vin.Color;

	vout.depthPosition = vout.PosH;

	return vout;
}

struct POut
{
	float4 Color    : SV_Target0;
	float4 Specular : SV_Target1;
	float4 Normal   : SV_Target2;
	float4 Depth    : SV_Target3;
	float4 UV		: SV_Target4;
	//float4 Reflection : SV_Target5;
};

POut PSforSkybox(VertexOut pin)
{
	POut output;	
	
	output.Color = SkyBoxCubeMap.SampleLevel(samAnisotropic, normalize(pin.PosW), 0);
	output.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	output.Normal = float4((pin.NormalW + 1.0f)*0.5f, -2.0f);
	output.UV = float4(pin.TexCoord, 0.0f, 0.0f);
	output.Depth = float4(1.0f, 1.0f, 1.0f, 1.0f);// pin.depthPosition;
	//output.Reflection = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return output;
}


POut PS(VertexOut pin)
{
	POut output;

	// ** NORMAL ** //
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);
	pin.TangentW = normalize(pin.TangentW);
	pin.BinormalW = normalize(pin.BinormalW);

	float3 WorldNormal = TangentSpaceToWorldSpace(pin.NormalW, pin.TangentW, pin.BinormalW, NormalMap.Sample(samAnisotropic, pin.TexCoord).xyz);

	WorldNormal = normalize(mul(WorldNormal, (float3x3)gWorldInvTranspose));


	//rgb is for normal, a is for metallic


	//float Metallic = min( NormalMap.Sample(samAnisotropic, pin.TexCoord).a , 0.0f);

	output.Normal = float4((WorldNormal + 1.0f)*0.5f, NormalMap.Sample(samAnisotropic, pin.TexCoord).a);
	//output.Normal = float4((pin.NormalW + 1.0f)*0.5f, NormalMap.Sample(samAnisotropic, pin.TexCoord).a);

	// ** COLOR ** //
	//rgb is for color
	output.Color = DiffuseMap.Sample(samAnisotropic, pin.TexCoord);
	
	// ** SPECULAR ** //
	//, a is for roughness
	output.Specular = SpecularMap.Sample(samAnisotropic, pin.TexCoord);
	
	
	//output.Specular.a = 0.5f;
	//output.Specular.a = gTime*0.3f - (int)(gTime*0.3f);//  abs(sin(gTime));
	//output.Specular.a = (sin(gTime) + 1.0f) * 0.5f;
		// ** UV ** //
	//[flatten]
	if(pin.TexCoord.x < 0.0f || pin.TexCoord.x > 1.0f || pin.TexCoord.y < 0.0f || pin.TexCoord.y > 1.0f)
		output.UV = float4(pin.TexCoord, 1.0f, 1.0f);
	else
		output.UV = float4(pin.TexCoord, 0.0f, 1.0f);

	
	
	// ** Depth ** //
	// r,g is for depth info
	//output.Depth.x = pin.depthPosition.z;	
	//output.Depth.y = pin.depthPosition.w;
	//output.Depth.z = bSelected;
	//output.Depth.w = 0.0f;
	
	output.Depth = pin.depthPosition;
	output.Depth.x = bSelected;
	return output;
}

technique11 BasicMaterialTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}

technique11 SkyBoxTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PSforSkybox()));
	}
}
