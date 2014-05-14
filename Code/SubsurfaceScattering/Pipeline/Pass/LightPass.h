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

			BasicLightData::ShadowMapLight* shadowData;
			int shadowCount;

			DirectX::XMFLOAT4X4 invProj;
			DirectX::XMFLOAT4X4 view;
			DirectX::XMFLOAT3 ambientLight;
		};

	public:
		LightPass();
		virtual~LightPass();

		void Release() override;
		void Clear() override;

		void Apply(const LightData& lights, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* positionMap, ID3D11ShaderResourceView* thickness);

		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);
		ID3D11ShaderResourceView* GetLightMapSRV();

		void ReloadShader();

	private:
		void RenderPointLight(BasicLightData::PointLight *dest, const BasicLightData::PointLight* data, int count, const DirectX::XMFLOAT4X4& view);
		void RenderSpotLight(BasicLightData::Spotlight *dest, const BasicLightData::Spotlight* data, int count, const DirectX::XMFLOAT4X4& view);
		void RenderDirectionalLight(BasicLightData::Directional *dest, const BasicLightData::Directional* data, int count, const DirectX::XMFLOAT4X4& view);

		bool CreateSRVAndBuffer(int width, int height);

		unsigned int width;
		unsigned int height;

		Shader lightShader;
		
		ID3D11UnorderedAccessView* lightMapUAV;
		ID3D11ShaderResourceView* lightMapSRV;

		ID3D11ShaderResourceView* pointLightBufferSRV;
		ID3D11ShaderResourceView* spotLightBufferSRV;
		ID3D11ShaderResourceView* dirLightBufferSRV;
		ID3D11ShaderResourceView* shadowBufferSRV;

		ID3D11Buffer* constLightBuffer;
		ID3D11Buffer* shadowBuffer;

		ID3D11Buffer* lightBuffer;

		int firstPointLight;
		int firstDirLight;
		int firstSpotLight;

		int maxPointLight;
		int maxDirLight;
		int maxSpotLight;

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
	};
}


#endif