#ifndef BETCHELOR_PIPELINEMANAGER_H
#define BETCHELOR_PIPELINEMANAGER_H

#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>

#include "Shader.h"
#include "Pass\GeometryPass.h"
#include "Pass\FinalPass.h"

namespace Pipeline
{
	class PipelineManager
	{
	public:
		static PipelineManager& Instance();

	public:
		void Release();
		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);

		void ApplyGeometryPass(bool clearPrevious);
		void ApplyLightPass(bool clearPrevious){}
		void ApplySSSPass(bool clearPrevious){}

		void Present();

	private:
		PipelineManager();
		virtual~PipelineManager();
		bool PipelineManager::CreateSwapChain(int width, int height);
		bool PipelineManager::CreateRTV();
		void CreateViewport(int width, int height);

		D3D11_VIEWPORT viewPort;
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
		IDXGISwapChain* d3dSwapchain;
		ID3D11RenderTargetView* renderTarget;

		FinalPass finalPass;
		GeometryPass geometryPass;
	};

}

#endif // !BETCHELOR_PipelineManager_H
