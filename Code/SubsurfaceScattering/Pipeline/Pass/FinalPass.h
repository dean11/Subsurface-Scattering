#ifndef BATCHELOR_FINALPASS_H
#define BATCHELOR_FINALPASS_H

#include "..\Shader.h"

namespace Pipeline
{
	class FinalPass
	{
	public:
		FinalPass();
		virtual~FinalPass();

		void Release();
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
