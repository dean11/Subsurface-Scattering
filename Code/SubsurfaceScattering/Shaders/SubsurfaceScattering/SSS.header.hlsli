

cbuffer FrameData :register(b1)
{
	float4x4 LightView;
	float4x4 LightProjection;
	float3 LightDirection;

	float pad[1];
};


Texture2D gSRVColor				:register(t0);
Texture2D gSRVNormal			:register(t1);
Texture2D gLightMap				:register(t2);
Texture2D gPositionMap			:register(t4);
Texture2D gThicknessMap			:register(t5);

Texture2D gShadowMap			:register(t6);



//Buffer for lightmatrix for shadow casting
//StructuredBuffer<LightData> LightMatrix	:register(t5);	

RWTexture2D<float4> FinalImage	:register(u0);

