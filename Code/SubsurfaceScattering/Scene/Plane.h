#ifndef PLANE_H
#define PLANE_H

#include "Model.h"

class Plane	:public Model
{
public:
	Plane();
	virtual~Plane();

	bool CreatePlane(ID3D11Device* device, DirectX::XMFLOAT3 position, const wchar_t texture[], float width = 1.0f, float depth = 1.0f, float scale = 1.0f);
	bool CreateModel(const char path[], ID3D11Device* device)  override;
	void Render(ID3D11DeviceContext* dc);
	void RenderForDepthMap(ID3D11DeviceContext* dc);
};

#endif // !_1_H
