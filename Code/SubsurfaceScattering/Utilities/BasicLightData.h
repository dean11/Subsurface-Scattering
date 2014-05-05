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
<<<<<<< HEAD
		DirectX::XMFLOAT4 lightColour;
		DirectX::XMFLOAT4 direction;		//w is just used for padding atm
	
=======
		DirectX::XMFLOAT3 color;
		DirectX::XMFLOAT3 direction;
		float pad[2 + 4 + 4];
>>>>>>> 1dff155c5f61c13ed99df508c39256fef11d8e68
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
};





#endif 