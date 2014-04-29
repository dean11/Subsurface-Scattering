#ifndef LIGHTPASS_H
#define LIGHTPASS_H
#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "..\..\Utilities\BasicLightData.h"
#include "ShaderPass.h"

namespace Pipeline
{
	//enum BPL_RTV_Layout
	//{
	//	/*BPL_RTV_Layout_AMBIENT	= 0,
	//	BPL_RTV_Layout_DIFFUSE	= 1,
	//	BPL_RTV_Layout_SPECULAR = 2,*/
	//	BPL_RTV_Layout_LIGHTMAP = 0,
	//	BPL_RTV_Layout_COUNT
	//};

	class LightPass :public ShaderPass
	{
	public:
		struct LightData
		{
			const BasicLightData::PointLight* pointData;
			int pointCount;
			const BasicLightData::Spotlight* spotData;
			int spotCount;
			const BasicLightData::Directional* dirData;
			int dirCount;

			DirectX::XMFLOAT4X4 invProj;
		};

	public:
		LightPass();
		virtual~LightPass();

		void Release() override;
		void Clear() override;

		void Apply(const LightData& lights, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* normalMap);

		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);
		ID3D11ShaderResourceView* GetLightMapSRV();

	private:
		void RenderPointLight(const BasicLightData::PointLight* data, int count);
		void RenderSpotLight(const BasicLightData::Spotlight* data, int count);
		void RenderDirectionalLight(const BasicLightData::Directional* data, int count);

		bool CreateSRVAndBuffer(int width, int height);

		unsigned int width;
		unsigned int height;

		Shader lightShader;
		
		ID3D11UnorderedAccessView* lightMapUAV;
		ID3D11ShaderResourceView* lightMapSRV;

		ID3D11ShaderResourceView* pointLightBufferSRV;
		ID3D11ShaderResourceView* spotLightBufferSRV;
		ID3D11ShaderResourceView* dirLightBufferSRV;
		ID3D11Buffer* lightBuffer;

		ID3D11Buffer* constLightBuffer;

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
	};
}


#endif