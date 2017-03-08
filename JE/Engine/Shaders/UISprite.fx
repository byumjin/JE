#include "common.fx"

cbuffer cbPerFrame
{	
	float3 gEyeWorldPos;
	float4x4 gWorld;	
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	bool bSelected;
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
	float4 Color : COLOR;
	float2 TexCoord : TEXCOORD;
};

/*
struct GeoOut
{
	float4 PosH    : SV_POSITION;
	float2 Tex     : TEXCOORD;
	float3 PosW    : POSITION;		
};
*/
Texture2D UISpriteMap;


VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.WorldPosition, 1.0f), gWorldViewProj);
	//vout.PosH = float4(vin.WorldPosition, 1.0f);
	vout.Color = vin.Color;
	vout.TexCoord = vin.TexCoord;
	return vout;	
}
/*
// We expand each point into a quad (4 vertices), so the maximum number of vertices
// we output per geometry shader invocation is 4.
[maxvertexcount(4)]
void GS(point VertexOut gin[1],	inout TriangleStream<GeoOut> triStream)
{
	//
	// Compute the local coordinate system of the sprite relative to the world
	// space such that the billboard is aligned with the y-axis and faces the eye.
	//

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyeWorldPos - gin[0].WorldPosition;
	//look.y = 0.0f; // y-axis aligned, so project to xz-plane
	look = normalize(look);
	float3 right = cross(up, look);
	up = cross(look, right);
	//
	// Compute triangle strip vertices (quad) in world space.
	//
	float halfWidth = 0.5f * 4.0f; // gin[0].SizeW.x;
	float halfHeight = 0.5f * 4.0f; // gin[0].SizeW.y;

	float4 v[4];
	v[0] = float4(gin[0].WorldPosition - halfWidth*right + halfHeight*up, 1.0f);
	v[1] = float4(gin[0].WorldPosition - halfWidth*right - halfHeight*up, 1.0f);
	v[2] = float4(gin[0].WorldPosition + halfWidth*right + halfHeight*up, 1.0f);
	v[3] = float4(gin[0].WorldPosition + halfWidth*right - halfHeight*up, 1.0f);

	//
	// Transform quad vertices to world space and output 
	// them as a triangle strip.
	//
	GeoOut gout;
	
	gout.PosH = mul(v[0], gViewProj);
	gout.PosW = v[0].xyz;
	gout.Tex = float2(0.0f, 0.0f);
	triStream.Append(gout);

	gout.PosH = mul(v[1], gViewProj);
	gout.PosW = v[1].xyz;
	gout.Tex = float2(0.0f, 1.0f);
	triStream.Append(gout);

	gout.PosH = mul(v[2], gViewProj);
	gout.PosW = v[2].xyz;
	gout.Tex = float2(1.0f, 0.0f);
	triStream.Append(gout);

	gout.PosH = mul(v[3], gViewProj);
	gout.PosW = v[3].xyz;
	gout.Tex = float2(1.0f, 1.0f);
	triStream.Append(gout);
	
}
*/
float4 PS(VertexOut pin) : SV_Target
{
	//float3 uvw = float3(pin.Tex, pin.PrimID % 4);
	float4 result = UISpriteMap.Sample(ObjTextureSampler, pin.TexCoord);
	//[flatten]
	if(bSelected)
		return  lerp(result, float4(0.0f, 0.0f, 1.0f, 1.0f) ,0.5f);
	else
		return  result;
	//return  float4(1.0f,0.0f,0.0f,1.0f);
}

technique11 UISpriteTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		/*SetGeometryShader(CompileShader(gs_5_0, GS()));*/
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}	
}
