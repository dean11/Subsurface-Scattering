#ifndef BETCHELOR_DEFERREDPIPELINE_H
#define BETCHELOR_DEFERREDPIPELINE_H

#include <d3d11.h>
#include <vector>

#include "Shader.h"

namespace Pipeline
{

	enum GBuffer_RTV_Layout
	{
		GBuffer_RTV_Layout_NORMAL		= 0,
		GBuffer_RTV_Layout_COLOR		= 1,
		GBuffer_RTV_Layout_POSITION	= 2,
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

	class DeferredPipeline
	{
	public:
		static DeferredPipeline& Instance();
		static void Release();

		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

		void Render();

	private:
		DeferredPipeline();
		virtual~DeferredPipeline();

		Shader vertex;
		Shader pixel;

		ID3D11RenderTargetView* GBufferRTV[GBuffer_RTV_Layout_COUNT];
		ID3D11ShaderResourceView* GBufferSRV[GBuffer_RTV_Layout_COUNT];
	};

}

#endif // !BETCHELOR_DEFERREDPIPELINE_H
