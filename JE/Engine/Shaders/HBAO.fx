#include "common.fx"

cbuffer cbPerFrame
{	
	float4x4 gViewProj;
	float4x4 gProj;
	float4x4 gView;

	float4x4 gInvProj;
	float4x4 gInvViewProj;

	float2 pixelSize;
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

Texture2D DepthMap;
Texture2D PositionMap;
Texture2D NormalMap;
Texture2D RandomNoiseMap;

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
	int level = 0;

	float4 gl_FragColor = float4(0.0f,0.0f,0.0f,0.0f);	

	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor04, 0.0f), level)*0.00390625;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor03, 0.0f), level)*0.03125;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor02, 0.0f), level)*0.109375;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(-factor01, 0.0f), level)*0.21875;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord, level)*0.2734375;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor01, 0.0f), level)*0.21875;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor02, 0.0f), level)*0.109375;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor03, 0.0f), level)*0.03125;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(factor04, 0.0f), level)*0.00390625;

	return gl_FragColor;
}

float4 GaussianHeight(VertexOut pin) : SV_Target
{
	int level = 0;

	float4 gl_FragColor = float4(0.0f,0.0f,0.0f,0.0f);
	
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor04), level)*0.00390625;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor03), level)*0.03125;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor02), level)*0.109375;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, -factor01), level)*0.21875;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord, level)*0.2734375;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor01), level)*0.21875;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor02), level)*0.109375;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor03), level)*0.03125;
	gl_FragColor += DepthMap.SampleLevel(TextureMipSampler, pin.TexCoord + float2(0.0f, factor04), level)*0.00390625;
	return gl_FragColor;
}

float3 CosineSampleHemisphere(float u1, float u2)
{
	float r = sqrt(u1);
	float theta = 2.0f * PI * u2;

	float x = r * cos(theta);
	float y = r * sin(theta);

	return normalize(float3(x, y, -sqrt(max(0.0f, 1.0f - u1))));
}

float4 PS2(VertexOut pin) : SV_Target
{
	// Bias of AO angle
	float bias = 0.2f;

	// Intensity of AO
	float intensity = 2.0f;

	// Size of local step
	float localStepSizeGap = 4.0f;

	// Maxium distance of AO 
	float lenCap = 0.5f;
	
	// Ambient Occulusion value
	float AO = 0.0f;

	// Number of sample directions
	int numOfDirections = 4;

	// Number of sample step 
	int numOfStep = 6;
	float stepSize = 2.0f;
	float gapAngle = 360.0f / (float)numOfDirections;
	gapAngle *= DegreeToRadian;

	//float4 DepthInfo = DepthMap.Sample(TextureSampler, pin.TexCoord);
	
	
	//float4 posInCamera = mul(float4(ScreenToView(pin.TexCoord), DepthInfo.z / DepthInfo.w, 1.0f), gInvProj);
	//posInCamera = posInCamera / posInCamera.w;
	
	float4 posInCamera = mul(PositionMap.Sample(TextureSampler, pin.TexCoord), gView);

	if (posInCamera.z < -10000.0f)
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		//Reconstruct Normal

		//float2 InvScreenSize = 1.0f / pixelSize;

		float4 posInCameraUp = mul(PositionMap.Sample(TextureSampler, pin.TexCoord - float2(0.0f, pixelSize.y)), gView);
		float4 posInCameraDown = mul(PositionMap.Sample(TextureSampler, pin.TexCoord + float2(0.0f, pixelSize.y)), gView);
		float4 posInCameraLeft = mul(PositionMap.Sample(TextureSampler, pin.TexCoord - float2(pixelSize.x, 0.0f)), gView);
		float4 posInCameraRight = mul(PositionMap.Sample(TextureSampler, pin.TexCoord + float2(pixelSize.x, 0.0f)), gView);

		float3 UC = posInCamera.xyz - posInCameraUp.xyz;
		float3 DC = posInCameraDown.xyz - posInCamera.xyz;

		float3 LC = posInCamera.xyz - posInCameraLeft.xyz;
		float3 RC = posInCameraRight.xyz - posInCamera.xyz;

		float3 DX;
		float3 DY;

		if (length(UC) < length(DC))
		{
			DY = UC;
		}
		else
		{
			DY = DC;
		}

		if (length(LC) < length(RC))
		{
			DX = LC;
		}
		else
		{
			DX = RC;
		}


		//float3 d1 = ddx(posInCamera);
		//float3 d2 = ddy(posInCamera);
		//float3 normalInCamera = normalize(cross(d1, d2)); // this normal is dp/du X dp/dv

		float3 normalInCamera = normalize(cross(DX, DY));


		//return float4(normalInCamera, 1.0f);
		//normal = normalize(mul(normal, (float3x3)gInvView));




		//posInCamera = mul(posInCamera, gView);

		//posInCamera.z = DepthInfo.w;

		//return posInCamera;

		/*
		float3 d1 = ddx(posInCamera);
		float3 d2 = ddy(posInCamera);
		float3 normalInCamera = normalize(cross(d1, d2)); // this normal is dp/du X dp/dv
		*/

		/*
		float3 normalInCamera = NormalMap.Sample(TextureSampler, pin.TexCoord).xyz * 2.0f - 1.0f;
		float4 normalInCamera4 = mul(float4(normalInCamera, 0.0f), gView);
		normalInCamera = normalize(normalInCamera4.xyz);
		*/

		//float t = acos(dot(normalInCamera, float3(0, 0, -1.0f)));

		//first Direction
		float2 sampleDirection = float2(1.0f, 0.0f);
		float2 noiseMapSize = float2(256.0f, 256.0f);

		float2 noiseScale = float2(1.0f / (pixelSize.x * noiseMapSize.x), 1.0f / (pixelSize.y * noiseMapSize.y));

		float randomVal = saturate(RandomNoiseMap.Sample(RandomNoiseTextureSampler, float2(pin.TexCoord.x * noiseScale.x, pin.TexCoord.y * noiseScale.y)).x);
		//float randomVal = 0.0f;


		for (int i = 0; i < numOfDirections; i++)
		{
			//Rotate
			float newgapAngle = gapAngle * ((float)i + randomVal);
			float cosVal = cos(newgapAngle);
			float sinVal = sin(newgapAngle);

			float2 rotatedSampleDirection = float2(cosVal * sampleDirection.x - sinVal * sampleDirection.y, sinVal * sampleDirection.x + cosVal * sampleDirection.y);

			//return 

			float localAO = 0.0f;

			float localStepSize = stepSize;

			for (int j = 0; j < numOfStep; j++)
			{
				float2 rotatedSampleDirection2 = float2(rotatedSampleDirection.x  * localStepSize  * pixelSize.x, rotatedSampleDirection.y  * localStepSize  * pixelSize.y);

				float2 newUV = rotatedSampleDirection2 + pin.TexCoord;

				if (newUV.x > 1.0f || newUV.y > 1.0f || newUV.x < 0.0f || newUV.y < 0.0f)
					break;


				//float4 stepDepthInfo = DepthMap.Sample(TextureSampler, newUV);
				//float4 stepPosInCamera = mul(float4(ScreenToView(newUV), stepDepthInfo.z / stepDepthInfo.w, 1.0f), gInvProj);
				//stepPosInCamera /= stepPosInCamera.w;


				float4 stepPosInCamera = mul(PositionMap.Sample(TextureSampler, newUV), gView);

				float3 diffVec = stepPosInCamera.xyz - posInCamera.xyz;

				float len = length(diffVec);

				//return float4(diffVec, 0.0f);

				if (len <= lenCap)
				{
					float dotVal = saturate(dot(normalInCamera, diffVec / len));

					float weight = len / lenCap;
					weight = 1.0f - weight*weight;
					/*
					float h = atan(stepPosInCamera.z / length(stepPosInCamera.xy));
					float dotVal = sin(h) - sin(t);
					*/

					if (dotVal < bias)
						dotVal = 0.0f;

					localAO = max(localAO, dotVal * weight);
					localStepSize += stepSize;
				}
				else
					break;

			}

			AO += localAO;

			//break;
		}

		AO /= (float)numOfDirections;



		AO = 1.0f - saturate(AO);
		AO = pow(AO, intensity);

		return float4(AO, AO, AO, AO);
	}
}


float4 PS(VertexOut pin) : SV_Target
{
	float4 DepthInfo = DepthMap.Sample(TextureSampler, pin.TexCoord);
	float4 posInCamera = mul(float4(ScreenToView(pin.TexCoord), DepthInfo.z / DepthInfo.w, 1.0f)*DepthInfo.w, gInvProj);
	posInCamera = posInCamera / posInCamera.w;

	//float posZInCamera = posInCamera.z;

	/*
	if (posZInCamera > 1000.0f)
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	*/

	//float radius = 1.0f;


	float3 normalInCamera = NormalMap.Sample(TextureSampler, pin.TexCoord).xyz * 2.0f - 1.0f;
	normalInCamera = mul(normalInCamera, (float3x3)gView);

	//float t = acos(dot(normalInCamera, float3(0.0f, 0.0f, -1.0f)));
	

	//numOfDirections
	int numOfDirections = 4;
	int numOfStep = 2;

	float stepSize = 0.25f;
	float gapAngle = 360.0f / (float)numOfDirections;
	gapAngle *= DegreeToRadian;

	
	//first Direction
	float2 sampleDirection = float2(0.2f, 0.0f);
	float2 noiseMapSize = float2(256.0f, 256.0f);

	float AO = 0.0f;
	float2 noiseScale = float2(1.0f / (pixelSize.x * noiseMapSize.x), 1.0f / (pixelSize.y * noiseMapSize.y));

	float random = RandomNoiseMap.Sample(RandomNoiseTextureSampler, float2(pin.TexCoord.x * noiseScale.x, pin.TexCoord.y * noiseScale.y)).x;

	
	for (int i = 0; i < numOfDirections; i++)
	{		
		//Rotate
		float newgapAngle = gapAngle * (float)i + random;
		float cosVal = cos(newgapAngle);
		float sinVal = sin(newgapAngle);

		float2 rotatedSampleDirection = float2(cosVal * sampleDirection.x - sinVal * sampleDirection.y, sinVal * sampleDirection.x + cosVal * sampleDirection.y);
		
		float h = 0.0f;

		//SignOfDirection		
		//float s = dot(normalInCamera, normalize(float3(rotatedSampleDirection.x, rotatedSampleDirection.y, 0.0f)));
		
		for (int j = 1; j <=  numOfStep; j++)
		{
			float2 rotatedSampleDirection2 = float2(rotatedSampleDirection.x  * stepSize * (float)j, rotatedSampleDirection.y  * stepSize * (float)j );
			
			
			float4 NDC = mul(float4(rotatedSampleDirection2 + posInCamera.xy, posInCamera.z ,1.0f), gProj);
			NDC /= NDC.w;

			NDC.xy = ViewToScreen(NDC.xy);
			float2 newUV = NDC.xy;

			float4 stepDepthInfo = DepthMap.Sample(TextureSampler, newUV);
			float4 stepPosInCamera = mul(float4(ScreenToView(newUV), stepDepthInfo.z / stepDepthInfo.w, 1.0f)*stepDepthInfo.w, gInvProj);
			stepPosInCamera /= stepPosInCamera.w;
			float3 diffVec = stepPosInCamera.xyz - posInCamera.xyz;
						
			float len = length(diffVec);
			
			if(len <= stepSize * 2.0f)
			{
				float Weight = 1.0f - saturate(len - stepSize) / stepSize;

				Weight = Weight*Weight;
				h = max(h, saturate(dot(normalInCamera, normalize(diffVec))));
			}
		}

		AO += h;
	}
	
	AO /= (float)numOfDirections;
	
	AO = saturate(1.0f - AO);
	AO = AO*AO;
	return float4(AO, AO, AO, AO);
}

technique11 HBAOTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));		
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS2()));
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
