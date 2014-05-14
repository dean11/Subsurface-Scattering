

Texture2D gSRVColor				:register(t0);			 //SrvMap_Normal = 1,			//:register(1)
Texture2D gLightMap				:register(t2);			 //SrvMap_Light = 2,			//:register(2)

RWTexture2D<float4> FinalImage	:register(u0);

[numthreads(32, 32, 1)]
void main(	uint3 dti	: SV_DispatchThreadID, 
			uint3 gti	: SV_GroupThreadID, 
			uint3 gid	: SV_GroupID, 
			uint gi		: SV_GroupIndex )
{
	float4 color		= gSRVColor[dti.xy];
	float4 light		= gLightMap[dti.xy];

	FinalImage[dti.xy] = (saturate(color) * saturate(light));

}
