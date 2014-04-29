#ifndef BATCHELOR_SHADERPASS_H
#define BATCHELOR_SHADERPASS_H

#include <d3d11_2.h>

class ShaderPass
{
public:
	virtual void Release() = 0;
	virtual void Clear() {};
};

#endif // !BATCHELOR_SHADERPASS_H
