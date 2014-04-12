#ifndef BACHELOR_APP_H
#define BACHELOR_APP_H

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <DirectXMath.h>
#include "Pipeline\DeferredPipeline.h"

class App
{
	public:
		App();
		virtual~App();
		bool Initiate();
		void Run();
		void Release();

	private:
		void BeginScene();
		void EndScene();
		void Render(float dt);
		bool Init3D_DeviceAndContext();
		bool Init3D_SwapChain();
		bool Init3D_DepthStencil();
		bool Init3D_RTV();
		bool Init3D_Viewport();
		bool Init3D_Pipeline();
		bool InitWindow();
	
		static LRESULT CALLBACK WindowCallback(HWND h, UINT m, WPARAM w, LPARAM l);

	private:
		DirectX::XMINT2 winDimension;
		bool isInitiated;
		bool isRunning;

		ID3D11Device* d3dDevice;
		ID3D11DeviceContext* d3dDeviceContext;
		IDXGISwapChain* d3dSwapchain;
		ID3D11DepthStencilView* depthStencilView;
		ID3D11ShaderResourceView* depthStencilUAV;
		D3D11_VIEWPORT viewPort;
		ID3D11RenderTargetView* backBufferRTV;
		ID3D11UnorderedAccessView* backBufferUAV;
};

#endif // !BACHELOR_APP_H
