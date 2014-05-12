#ifndef SHADOWVOLPASS_H
#define SHADOWVOLPASS_H
#include <d3d11.h>
#include <vector>
#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "ShaderPass.h"

namespace Pipeline
{
	enum ShadowVol_RTV_Layout
	{
		ShadowVol_RTV_Layout_VOLUMEFRONT = 0,
		ShadowVol_RTV_Layout_VOLUMEBACK = 1,
		ShadowVol_RTV_Layout_FRONT = 2,
		ShadowVol_RTV_Layout_BACK = 3,

		ShadowVol_RTV_Layout_COUNT,
	};

	class ShadowVolPass :public ShaderPass
	{
	public:
		ShadowVolPass();
		virtual~ShadowVolPass();

		void Apply();
		void Release();
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);
		ID3D11ShaderResourceView* GetShaderResource(ShadowVol_RTV_Layout srv);

		ID3D11RenderTargetView* GetRenderTarget(ShadowVol_RTV_Layout rtv);

	private:
		bool CreateDepthStencilAndRenderTargets(int width, int height);

		Shader capsVertex;
		Shader capsGeometry;
		Shader capsPixel;

		Shader volVertex;
		Shader volGeometry;
		Shader volPixel;

		RenderSurface ShadVolRTVs[ShadowVol_RTV_Layout_COUNT];
		RenderSurface depthStencil;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
	};

}
#endif