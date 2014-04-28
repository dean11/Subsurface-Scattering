#ifndef POINTLIGHT_H
#define POINTLIGHT_H
#include "..\Utilities\BasicLightData.h"
#include "Model.h"
#include "..\Pipeline\Shader.h"

class PointLight
{
	struct Light
	{
		BasicLightData::PointLight light;
		Model sphere;
	};
public:
	PointLight();
	virtual~PointLight();
	void Release();

	bool CreatePointLight(const char path[], BasicLightData::PointLight data, ID3D11Device* device);
	Light& GetPointLight();

private:
	Light pointLight;
};

#endif