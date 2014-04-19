#ifndef BATCHELOR_FINALPASS_H
#define BATCHELOR_FINALPASS_H

#include "..\Shader.h"
#include "ShaderPass.h"

namespace Pipeline
{
	class FinalPass :public ShaderPass
	{
	public:
		FinalPass();
		virtual~FinalPass();

		void Release() override;
		void Apply() override;
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile) override;

	private:
		
		ID3D11Buffer* quadVBuffer;
		ID3D11Buffer* quadIBuffer;
		Shader vertex;
		Shader pixel;
		
	};
}

#endif // !BATCHELOR_FINALPASS_H
