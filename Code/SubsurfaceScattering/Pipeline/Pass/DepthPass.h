#ifndef DEPTHPASS_H
#define DEPTHPASS_H
#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "ShaderPass.h"
#include "..\..\Utilities\Camera.h"
#include "LightPass.h"

class DepthPass : public ShaderPass
{
	public:
		DepthPass();
		virtual~DepthPass();

		void Release();
		void Clear();

		void Apply(Camera& camera, std::vector<ID3D11Buffer> vertexBuffers, std::vector<LightPass::LightData>);

		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);
		ID3D11ShaderResourceView* GetDepthMapSRV();
};


#endif