#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gWorldViewProj;
	bool bSelected;

};


struct VertexIn
{
	float3 LocalPosition  : POSITION;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION; 	
};




VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	vout.PosH = mul(float4(vin.LocalPosition, 1.0f), gWorldViewProj);
	
    return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
	
	if (bSelected)
		return float4(1.0f, 1.0f, 0.0f, 1.0f);
	else
		return float4(0.0f, 1.0f, 0.0f, 1.0f);
	
	
}

technique11 LineTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );		
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
