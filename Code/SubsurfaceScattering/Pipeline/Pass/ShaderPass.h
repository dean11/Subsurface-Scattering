#ifndef BATCHELOR_SHADERPASS_H
#define BATCHELOR_SHADERPASS_H

#include <d3d11_2.h>

class ShaderPass
{
public:
	virtual void Release() = 0;
	virtual void Apply() = 0;
	virtual bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile) = 0;

protected:
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
};

#endif // !BATCHELOR_SHADERPASS_H
