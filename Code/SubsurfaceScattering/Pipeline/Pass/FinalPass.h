#ifndef BATCHELOR_FINALPASS_H
#define BATCHELOR_FINALPASS_H

#include "..\Shader.h"
#include "ShaderPass.h"
#include "..\RenderSurface.h"


namespace Pipeline
{
	class FinalPass :public ShaderPass
	{
	public:
		struct DispatchTextureData
		{
			ID3D11ShaderResourceView* DiffuseMap;
			ID3D11ShaderResourceView* NormalMap;
			ID3D11ShaderResourceView* LightMap;
			ID3D11ShaderResourceView* ThicknessMap;
			ID3D11ShaderResourceView* ShadowMap;
			DispatchTextureData()
			{
				DiffuseMap		= 0;
				NormalMap		= 0;
				LightMap		= 0;
				ThicknessMap	= 0;
				ShadowMap		= 0;
			}
		};
		enum SrvMap
		{
			SrvMap_Diffuse		= 0,		//:register(0)
			SrvMap_Normal		= 1,		//:register(1)
			SrvMap_Light		= 2,		//:register(2)
			SrvMap_Thickness	= 3,		//:register(3)
			SrvMap_shadowMap	= 4,		//:register(4)

			SrvMap_COUNT
		};

	public:
		FinalPass();
		virtual~FinalPass();

		void Release() override;
		void Clear() override;

		void Apply(DispatchTextureData& srvData);
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile, IDXGISwapChain* swapChain);

	private:
		int width;
		int height;
		bool isCleared;

		Shader compute;

		ID3D11DeviceContext* deviceContext;

		ID3D11UnorderedAccessView* backBufferUAV;
	};
}

#endif // !BATCHELOR_FINALPASS_H
