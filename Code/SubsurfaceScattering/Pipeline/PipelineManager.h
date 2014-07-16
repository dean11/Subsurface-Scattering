#ifndef BETCHELOR_PIPELINEMANAGER_H
#define BETCHELOR_PIPELINEMANAGER_H

#include <vld.h>
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Pass\ShaderPass.h"
#include "Shader.h"
#include "ShadowMap.h"
#include "Pass\GeometryPass.h"
#include "Pass\FinalPass.h"
#include "Pass\LightPass.h"
#include "Pass\BlurPass.h"
#include "Pass\SSSPass.h"
#include "Pass\PostEffectPass.h"
#include <D3DTK\SpriteBatch.h>

namespace Pipeline
{
	class PipelineManager
	{
	public:
		static PipelineManager& Instance();

	public:
		void Release();
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

		void ApplyGeometryPass(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, BasicLightData::ShadowMapLight*const* shadowData, int shadowCount);
		void ApplyPostEffectPass(const LightPass::LightData& data );
		void ApplyFrontSSS();
		void ApplyUIPass(ID3D11ShaderResourceView*const* srv = 0, int total = 0);
		void Present();

		void SetMeshBuffer(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& worldInversTranspose, DirectX::XMFLOAT4 const* layers, int layerCount);

	private:
		PipelineManager();
		virtual~PipelineManager();
		bool PipelineManager::CreateSwapChain(int width, int height);
		bool PipelineManager::CreateRTV();
		void CreateViewport(int width, int height);
		bool CreateConstantBuffers();
		bool CreateTranslucentBuffer(int width);

		D3D11_VIEWPORT viewPort;

		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		IDXGISwapChain* d3dSwapchain;
		ID3D11RenderTargetView* renderTarget;
		ID3D11ShaderResourceView *backBufferSRV;

		ID3D11Buffer *objectTranslucentBuffer;
		ID3D11Buffer *objectMatrixBuffer;
		ID3D11Buffer *sceneMatrixBuffer;
		ID3D11Buffer *depthPointLightBuffer;

		ID3D11ShaderResourceView *objectTranslucentSrv;

		GeometryPass geometryPass;
		PostEffectPass postPass;
		BlurPass blurPass;
		
		ShaderPass* prevPass;

		DirectX::SpriteBatch *spriteBatch;
		bool debugRTV;
		int width, height;
	};

}

#endif // !BETCHELOR_PipelineManager_H
