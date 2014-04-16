#ifndef BETCHELOR_DEFERRED_RENDERER_H
#define BETCHELOR_DEFERRED_RENDERER_H

#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "ShaderPass.h"

namespace Pipeline
{

	enum GBuffer_RTV_Layout
	{
		GBuffer_RTV_Layout_NORMAL		= 0,
		GBuffer_RTV_Layout_COLOR		= 1,
		GBuffer_RTV_Layout_POSITION		= 2,
		GBuffer_RTV_Layout_COUNT
	};
	//enum GBuffer_SRV_Layout
	//{
	//	GBuffer_SRV_Layout_COLOR		= 0,
	//	GBuffer_SRV_Layout_NORMAL		= 1,
	//	GBuffer_SRV_Layout_DEPTH		= 3,
	//	GBuffer_SRV_Layout_LIGHT		= 4,
	//
	//	GBuffer_SRV_COUNT
	//};

	class GeometryPass :public ShaderPass
	{
	public:
		GeometryPass();
		virtual~GeometryPass();

		void Release() override;
		void Apply() override;
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile) override;

	private:
		bool CreateInputLayout();
		bool CreateDepthStencil(int width, int height);
		void CreateViewport(int width, int height);

		Shader vertex;
		Shader pixel;
		ID3D11InputLayout* inputLayout;
		ID3D11RenderTargetView* GBufferRTV[GBuffer_RTV_Layout_COUNT];
		ID3D11ShaderResourceView* GBufferSRV[GBuffer_RTV_Layout_COUNT];
		ID3D11DepthStencilView* depthStencilView;

		ID3D11ShaderResourceView* depthStencilUAV;
		D3D11_VIEWPORT viewPort;
	};

}

#endif // !BETCHELOR_DEFERREDPIPELINE_H
