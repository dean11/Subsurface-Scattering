#ifndef INCLUDEGUARD_DEPTHSURFACE_H
#define INCLUDEGUARD_DEPTHSURFACE_H

#include "Shader.h"
#include "RenderSurface.h"
#include <D3DTK\SimpleMath.h>

class Model;
using namespace DirectX;
using namespace Pipeline;

class ShadowMap
{
public:
	ShadowMap();
	virtual~ShadowMap();
	bool Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
	void Release();
	
	void Begin(const SimpleMath::Matrix& view, const SimpleMath::Matrix& projection);
	void Draw( Model& model );
	void End();

public:
	inline ID3D11ShaderResourceView* GetSrv() { return this->depth.GetDepthStencilSRV(); }

	operator ID3D11DepthStencilView* ()			{ return this->depth; }
	operator ID3D11ShaderResourceView* ()		{ return this->depth.GetDepthStencilSRV(); }
	operator const ID3D11ShaderResourceView* ()	{ return this->depth.GetDepthStencilSRV(); }
	operator RenderSurface ()					{ return this->depth; }
	operator const RenderSurface& ()			{ return this->depth; }
	operator RenderSurface* ()					{ return &this->depth; }

private:
	RenderSurface depth;
	D3D11_VIEWPORT viewPort;
	D3D11_VIEWPORT prevViewport;
};


class ShadowMapArray
{
public:
	ShadowMapArray();
	virtual~ShadowMapArray();
	bool Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, int depth);
	void Release();
	void Clear(ID3D11DeviceContext* dc );
	
	void Begin(const SimpleMath::Matrix& view, const SimpleMath::Matrix& projection, int shadowIndex);
	void End();

	void SetWorld(const SimpleMath::Matrix& world);

//public:
	//operator ID3D11DepthStencilView* ()		{ return this->depth; }
	//operator ID3D11ShaderResourceView* ()	{ return this->depth.GetDepthStencilSRV(); }
	//operator RenderSurface ()				{ return this->depth; }
	//operator const RenderSurface& ()		{ return this->depth; }
	//operator RenderSurface* ()				{ return &this->depth; }

private:
	SimpleMath::Matrix view;
	SimpleMath::Matrix projection;

	UINT _width;
	UINT _height;

	RenderSurface depth;
	D3D11_VIEWPORT viewPort;
	D3D11_VIEWPORT prevViewport;
};

#endif // !INCLUDEGUARD_DEPTHSURFACE_H
