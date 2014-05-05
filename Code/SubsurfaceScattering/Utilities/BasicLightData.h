#ifndef BASICLIGHTDATA_H
#define BASICLIGHTDATA_H
#include <DirectXMath.h>

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
		DirectX::XMFLOAT4 lightColour;
		DirectX::XMFLOAT4 direction;		//w is just used for padding atm
	
	};

	struct PointLight
	{
		DirectX::XMFLOAT4 lightColour;
		DirectX::XMFLOAT4 positionRange;	//xyz = Position, w = range, these are only used to calculate the light's world matrix
		DirectX::XMFLOAT4 attenuation;		//w is just used for padding atm
	};

	struct Spotlight
	{
		DirectX::XMFLOAT4 lightColour;
		DirectX::XMFLOAT4 positionRange;	//xyz = position, w = range
		DirectX::XMFLOAT4 directionSpot;	//xyz = position, w = spot, exponent used to calc the light cone
		DirectX::XMFLOAT4 attenuation;		//w is just used for padding atm
	};
};





#endif 