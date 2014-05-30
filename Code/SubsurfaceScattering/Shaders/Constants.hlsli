
#define SSS_ENABLE
#define MAX_SHADOWMAPS	5

//Standarnd Lights
struct DirLight
{
	float3 color;
	float3 direction;
	float intensity;

	float pad[1 + 4 + 4];
};
struct SpotLight
{
	float3 pos;
	float range;
	float3 dir;
	float coneAngle;
	float3 att;
	float3 color;
	int shadowMap;

	float pad[1];
};
struct PointLight
{
	float4 positionRange;
	float3 diffuse;
	int shadowMap;

	float pad[4 + 4];
};

//ShadowMap lights
struct ShadowMapLightData
{
	float4x4 viewProjection;
	float3 position;
	float range;
	float3 direction;
	float spot;
	float3 att;
	float3 color;

	float pad[2];
};
