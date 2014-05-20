#ifndef BASICLIGHTDATA_H
#define BASICLIGHTDATA_H
#include <D3DTK\SimpleMath.h>

#include "..\Pipeline\ShadowMap.h"
#include "Camera.h"

using namespace DirectX;

class BasicLightData
{
public:
	struct AmbDiffSpec
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT4 specular;			//w = specular power

	};

	struct Directional
	{
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 direction;
		float intensity;
		float pad[1 + 4 + 4];
	};

	struct PointLight
	{
		DirectX::XMFLOAT4 positionRange;	//xyz = Position, w = range
		DirectX::XMFLOAT3 lightColour;
		float pad[1 + 4 + 4];
	};

	struct Spotlight
	{
		DirectX::XMFLOAT3 position;
		float range;
		DirectX::XMFLOAT3 unitDir;
		float coneAngle;
		DirectX::XMFLOAT3 attenuation;
		DirectX::XMFLOAT3 color;
		float pad[2];
	};

	struct ShadowMapLight
	{
		DirectX::XMFLOAT3 attenuation;
		DirectX::XMFLOAT3 color;
		float range;
		float spot;
		Camera camera;
		ShadowMap shadowMap;
		bool isOn;
	};
};





#endif 