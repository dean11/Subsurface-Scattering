#ifndef SPHEREMAP_H
#define SPHEREMAP_H
#include "Model.h"
#include "..\Pipeline\PipelineManager.h"
#include <D3DTK\DDSTextureLoader.h>
#include "..\Pipeline\Shader.h"

class SphereMap
{
public:
	SphereMap();
	virtual~SphereMap();
	bool CreateSkyBox(ID3D11Device* device, ID3D11DeviceContext* dc);
	//DirectX::XMFLOAT4X4 CenterAroundCamPos(DirectX::XMFLOAT3& camPos);
	void Render(float delta, DirectX::XMFLOAT3 camPos);

private:
	Model sphereMap;
	//ID3D11ShaderResourceView* cubeSRV;
	ID3D11Device* device;
	ID3D11DeviceContext* dc;
	Pipeline::Shader pix;
	UINT off = 0;
	DirectX::XMFLOAT4X4 world4x4;
};

#endif

