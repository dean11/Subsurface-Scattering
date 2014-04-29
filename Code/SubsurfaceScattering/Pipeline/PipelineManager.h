#ifndef BETCHELOR_PIPELINEMANAGER_H
#define BETCHELOR_PIPELINEMANAGER_H

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Pass\ShaderPass.h"
#include "Shader.h"
#include "Pass\GeometryPass.h"
#include "Pass\FinalPass.h"
#include "Pass\LightPass.h"

namespace Pipeline
{
	class PipelineManager
	{
	public:
		static PipelineManager& Instance();

	public:
		void Release();
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

		void ApplyGeometryPass();
		void ApplyLightPass(const LightPass::LightData& data);
		void ApplySSSPass(){}

		void Present();

		void SetObjectMatrixBuffers(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& worldInversTranspose);
		void SetSceneMatrixBuffers(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

	private:
		PipelineManager();
		virtual~PipelineManager();
		bool PipelineManager::CreateSwapChain(int width, int height);
		bool PipelineManager::CreateRTV();
		void CreateViewport(int width, int height);
		bool CreateConstantBuffers();

		D3D11_VIEWPORT viewPort;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		IDXGISwapChain* d3dSwapchain;
		ID3D11RenderTargetView* renderTarget;

		ID3D11Buffer *objectMatrixBuffer;
		ID3D11Buffer *sceneMatrixBuffer;

		FinalPass finalPass;
		GeometryPass geometryPass;
		LightPass lightPass;

		ShaderPass* prevPass;
	};

}

#endif // !BETCHELOR_PipelineManager_H
