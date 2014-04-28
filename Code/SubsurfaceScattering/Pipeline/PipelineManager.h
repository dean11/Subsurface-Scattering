#ifndef BETCHELOR_PIPELINEMANAGER_H
#define BETCHELOR_PIPELINEMANAGER_H

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Shader.h"
#include "Pass\GeometryPass.h"
#include "Pass\FinalPass.h"
#include "Pass\LightPass.h"

namespace Pipeline
{
	class PipelineManager
	{
	public:
		struct LightData
		{
			const BasicLightData::PointLight* pointData;
			int pointCount;
			const BasicLightData::Spotlight* spotData;
			int spotCount;
			const BasicLightData::Directional* dirData;
			int dirCount;
		};
	public:
		static PipelineManager& Instance();

	public:
		void Release();
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

		void ApplyGeometryPass(bool clearPrevious);
		void ApplyLightPass(const LightData& data);
		void ApplySSSPass(bool clearPrevious){}

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
	};

}

#endif // !BETCHELOR_PipelineManager_H
