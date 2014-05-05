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
		enum SrvMap
		{
			SrvMap_Diffuse = 0,		//:register(0)
			SrvMap_Normal = 1,		//:register(1)
			SrvMap_Light = 2,		//:register(2)

			SrvMap_COUNT
		};

	public:
		FinalPass();
		virtual~FinalPass();

		void Release() override;
		void Clear() override;

		void Apply();
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);

	private:
		
		ID3D11Buffer* quadVBuffer;
		ID3D11Buffer* quadIBuffer;
		Shader vertex;
		Shader pixel;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
	};
}

#endif // !BATCHELOR_FINALPASS_H
