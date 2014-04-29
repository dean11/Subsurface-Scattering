#ifndef LIGHTPASS_H
#define LIGHTPASS_H
#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "..\..\Utilities\BasicLightData.h"

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

	class LightPass
	{
	public:
		LightPass();
		virtual~LightPass();

		void Release();
		void Apply( ID3D11ShaderResourceView* diffuseMap, ID3D11ShaderResourceView* normalMap );

		void RenderPointLight(const BasicLightData::PointLight* data, int count);
		void RenderSpotLight(const BasicLightData::Spotlight* data, int count);
		void RenderDirectionalLight(const BasicLightData::Directional* data, int count);
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);

	private:
		bool CreateSRVAndBuffer(int width, int height);

		Shader pointLight;
		Shader spotLight;
		Shader dirLight;

		ID3D11ShaderResourceView* lightMapSRV;

		ID3D11ShaderResourceView* lightBufferSRV;
		ID3D11Buffer* lightBuffer;

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
	};
}


#endif