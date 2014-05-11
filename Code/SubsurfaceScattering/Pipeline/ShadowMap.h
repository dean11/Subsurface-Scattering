#ifndef INCLUDEGUARD_DEPTHSURFACE_H
#define INCLUDEGUARD_DEPTHSURFACE_H

#include "Shader.h"
#include "RenderSurface.h"
#include <D3DTK\SimpleMath.h>

using namespace DirectX;
using namespace Pipeline;

class ShadowMap
{
public:
	ShadowMap();
	virtual~ShadowMap();
	bool Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
	bool Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, const SimpleMath::Matrix& view, const SimpleMath::Matrix& projection );
	void Release();
	void Clear(ID3D11DeviceContext* dc );
	
	void Begin(const SimpleMath::Matrix& view, const SimpleMath::Matrix& projection);
	void Begin();
	void End();

	void SetWorld(const SimpleMath::Matrix& world);
	void SetView(const SimpleMath::Matrix& m);
	void SetProjection(const SimpleMath::Matrix& m);

public:
	operator ID3D11DepthStencilView* ()		{ return this->depth; }
	operator ID3D11ShaderResourceView* ()	{ return this->depth.GetDepthStencilSRV(); }
	operator RenderSurface ()				{ return this->depth; }
	operator const RenderSurface& ()		{ return this->depth; }
	operator RenderSurface* ()				{ return &this->depth; }

private:
	SimpleMath::Matrix view;
	SimpleMath::Matrix projection;

	RenderSurface depth;
	D3D11_VIEWPORT viewPort;
	D3D11_VIEWPORT prevViewport;
	
	static ID3D11Device* device;
	static ID3D11DeviceContext* dc;
	static Shader *depthShader;
	static ID3D11Buffer* sceneBuffer;
};

#endif // !INCLUDEGUARD_DEPTHSURFACE_H
