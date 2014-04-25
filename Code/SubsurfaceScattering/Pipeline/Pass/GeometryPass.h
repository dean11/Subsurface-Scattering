#ifndef BETCHELOR_DEFERRED_RENDERER_H
#define BETCHELOR_DEFERRED_RENDERER_H

#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "ShaderPass.h"
#include "..\RenderSurface.h"

namespace Pipeline
{

	enum GBuffer_RTV_Layout
	{
		GBuffer_RTV_Layout_NORMAL		= 0,
		GBuffer_RTV_Layout_COLOR		= 1,
		GBuffer_RTV_Layout_COUNT
	};

	class GeometryPass :public ShaderPass
	{
	public:
		GeometryPass();
		virtual~GeometryPass();

		void Release() override;
		void Apply() override;
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile) override;
		ID3D11ShaderResourceView* GetShaderResource(GBuffer_RTV_Layout srv);
		ID3D11RenderTargetView* GetRenderTarget(GBuffer_RTV_Layout rtv);

	private:
		bool CreateDepthStencilAndRenderTargets(int width, int height);

		Shader vertex;
		Shader pixel;

		RenderSurface GBufferRTVs[GBuffer_RTV_Layout_COUNT];
		RenderSurface depthStencil;
	};

}

#endif // !BETCHELOR_DEFERREDPIPELINE_H
