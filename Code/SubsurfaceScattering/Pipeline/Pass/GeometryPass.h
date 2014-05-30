#ifndef BETCHELOR_DEFERRED_RENDERER_H
#define BETCHELOR_DEFERRED_RENDERER_H

#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "ShaderPass.h"
#include "..\..\Utilities\BasicLightData.h"

namespace Pipeline
{

	enum GBuffer_RTV_Layout
	{
		// These values must be in the same order as in the shader.
		GBuffer_RTV_Layout_NORMAL		= 0,
		GBuffer_RTV_Layout_COLOR		= 1,
		GBuffer_RTV_Layout_POSITION		= 2,
		GBuffer_RTV_Layout_THICKNESS	= 3,

		//Everything below this enum value is considered to have no render target. and is not part of the "geometry pipleine"
		GBuffer_RTV_Layout_COUNT,

		GBuffer_RTV_Layout_DepthStencil,
	};

	class GeometryPass :public ShaderPass
	{
	public:
		GeometryPass();
		virtual~GeometryPass();

		void Release() override;
		void Clear() override;

		void Apply(BasicLightData::ShadowMapLight*const* shadowData, int shadowCount);
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile);
		ID3D11ShaderResourceView* GetShaderResource(GBuffer_RTV_Layout srv);
		
		ID3D11RenderTargetView* GetRenderTarget(GBuffer_RTV_Layout rtv);

	private:
		bool CreateDepthStencilAndRenderTargets(int width, int height);

		Shader vertex;
		Shader pixel;

		RenderSurface GBufferRTVs[GBuffer_RTV_Layout_COUNT];
		RenderSurface depthStencil;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;

		ID3D11Buffer*	shadowBuffer;
		ID3D11ShaderResourceView* shadowSRV;
	};

}

#endif // !BETCHELOR_DEFERREDPIPELINE_H
