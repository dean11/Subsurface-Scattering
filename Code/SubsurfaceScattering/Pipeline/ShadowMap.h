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
	void SetSize(int width, int height);
	void SetSize(int width, int height, int depth);
	bool Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Release();
	
	void Begin(const SimpleMath::Matrix view[], const SimpleMath::Matrix projection[], int size, int offset);
	void Draw( Model& model );
	void End();

public:
	operator ID3D11DepthStencilView* ()			{ return this->depthSurface; }
	operator ID3D11ShaderResourceView* ()		{ return this->depthSurface.GetDepthStencilSRV(); }
	operator const ID3D11ShaderResourceView* ()	{ return this->depthSurface.GetDepthStencilSRV(); }
	operator RenderSurface ()					{ return this->depthSurface; }
	operator const RenderSurface& ()			{ return this->depthSurface; }
	operator RenderSurface* ()					{ return &this->depthSurface; }

private:
	int index;
	static UINT width;
	static UINT height;
	static UINT depth;
	static D3D11_VIEWPORT viewPort;
	static bool isCreated;
	static RenderSurface depthSurface;
};


class ShadowMapUAV
{
public:
	ShadowMapUAV();
	virtual~ShadowMapUAV();
	void SetSize(int width, int height);
	void SetSize(int width, int height, int depth);
	bool Create(ID3D11Device* device);
	void Release();

	void Begin(const SimpleMath::Matrix view[], const SimpleMath::Matrix projection[], int size, int offset);
	void Draw( Model& model );
	void End();

private:
	
	int index;
	static ID3D11Buffer* frameBuffer;
	static int ref;
	static D3D11_VIEWPORT viewPort;
	static ID3D11ShaderResourceView* surface;
	static DirectX::SimpleMath::Vector3 size;
	static ID3D11Device* device;
};

#endif // !INCLUDEGUARD_DEPTHSURFACE_H
