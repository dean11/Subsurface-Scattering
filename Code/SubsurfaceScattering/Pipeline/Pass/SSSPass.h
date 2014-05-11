#ifndef SSSPASS_H
#define SSSPASS_H

#include <d3d11.h>
#include "..\Shader.h"
#include "ShaderPass.h"
#include "..\ShadowMap.h"
#include "..\..\Utilities\BasicLightData.h"
#include <D3DTK\SimpleMath.h>


namespace Pipeline
{
	using namespace DirectX;
	class SSSPass
	{
	public:
		SSSPass();
		virtual~SSSPass();

		void Release();
		void Clear();

		void Apply(ID3D11ShaderResourceView* srvWorldPositionMap, ID3D11ShaderResourceView* srvNomalMap, const ShadowMap* depths, unsigned int totalDepths);

		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

	private:
		unsigned int width;
		unsigned int height;

		Shader sssShader;

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		ID3D11Buffer* frameBuffer;

		ID3D11UnorderedAccessView* sssUav;

	};
}

#endif // !SSSPASS_1_H
