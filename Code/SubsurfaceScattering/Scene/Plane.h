#ifndef PLANE_H
#define PLANE_H

#include "Model.h"

class Plane	:public Model
{
public:
	Plane() {}
	virtual~Plane() {}

	bool CreatePlane(ID3D11Device* device, DirectX::XMFLOAT3 position, const wchar_t texture[], const SimpleMath::Vector4 materialLayer[], int layerCount, float width = 1.0f, float depth = 1.0f, float scale = 1.0f);
	bool CreateModel(const char path[], ID3D11Device* device, const SimpleMath::Vector4 layers[], int layerCount)  override { return false; }
};

#endif // !_1_H
