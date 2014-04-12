#include "App.h"
#include <stdio.h>
#include <cstdio>
#include "clock.h"
#include "Pipeline\DeferredPipeline.h"
#include "WindowShell.h"
#include <sys\stat.h>

Pipeline::DeferredPipeline &renderer = Pipeline::DeferredPipeline::Instance();

App::App()
	:	isInitiated(false)
	,	isRunning (false)
	,	winDimension(800, 600)
	,	d3dDevice(0)
	,	d3dDeviceContext(0)
	,	d3dSwapchain(0)
	,	depthStencilView(0)
	,	depthStencilUAV(0)
	,	backBufferRTV(0)
	,	backBufferUAV(0)
{

}
App::~App()
{}
bool App::Initiate()
{
	if(!this->InitWindow())					return false;
	if(!this->Init3D_DeviceAndContext())	return false;
	if(!this->Init3D_SwapChain())			return false;
	if(!this->Init3D_DepthStencil())		return false;
	if(!this->Init3D_RTV())					return false;
	if(!this->Init3D_Viewport())			return false;
	if(!this->Init3D_Pipeline())			return false;

	return (this->isInitiated = true);
}
void App::Run()
{
	this->isRunning = true;
	Utility::WinTimer clock;
	clock.reset();

	while (WindowShell::Frame() && this->isRunning)
	{
		float dt = (float)clock.getElapsedSeconds();
		clock.reset();
		Render(dt);
	}
}
void App::Release()
{
	if(isInitiated)
	{
		renderer.Release();

		this->backBufferRTV->Release();
		this->backBufferUAV->Release();
		this->d3dDevice->Release();
		this->d3dDeviceContext->Release();
		this->d3dSwapchain->Release();
		this->depthStencilUAV->Release();
		this->depthStencilView->Release();
		this->isInitiated = false;
		this->isRunning = false;
	}
}

void App::BeginScene()
{
	this->d3dDeviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Pipeline::DeferredPipeline::Instance().Render();
}
void App::EndScene()
{
	this->d3dSwapchain->Present(0, 0);
}
void App::Render(float dt)
{
	this->BeginScene();
	{
		this->d3dDeviceContext->Draw(0, 0);
	}
	this->EndScene();
}

bool App::InitWindow()
{
	WindowShell::WINDOW_INIT_DESC desc;
	desc.windowName = L"Subsurface Scattering in screen-space";
	desc.windowSize.x = this->winDimension.x;
	desc.windowSize.y = this->winDimension.y;
	desc.windowProcCallback = App::WindowCallback;
	
	if(!WindowShell::CreateWin(desc))
		return false;

	return true;
}
bool App::Init3D_DeviceAndContext()
{
	::D3D_DRIVER_TYPE driverType = ::D3D_DRIVER_TYPE_HARDWARE;
		
	UINT createDeviceFlags = ::D3D11_CREATE_DEVICE_SINGLETHREADED;
	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevelsToTry[] = 
	{D3D_FEATURE_LEVEL_11_0,};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	if( FAILED( ::D3D11CreateDevice(	NULL, // default adapter
										driverType,
										NULL, // no software device
										createDeviceFlags,
										featureLevelsToTry, 1,
										D3D11_SDK_VERSION,
										&this->d3dDevice,
										&initiatedFeatureLevel,
										&this->d3dDeviceContext ) ) )
	{ // if failed
		if( this->d3dDeviceContext ) 
		{ 
			this->d3dDeviceContext->Release(); this->d3dDeviceContext = NULL; 
		}
		if( this->d3dDevice ) 
		{ 
			this->d3dDevice->Release(); this->d3dDevice = NULL; 
		}
		return false;
	}

	return true;
}
bool App::Init3D_SwapChain()
{
	//generate static Swapchain Desc
	DXGI_SWAP_CHAIN_DESC desc;
	desc.OutputWindow						= WindowShell::GetHWND();
	desc.BufferCount						= 1;
	desc.Windowed							= true;
	desc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	desc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags								= 0;
	desc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling					= DXGI_MODE_SCALING_CENTERED;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.RefreshRate.Numerator	= 60;
	desc.BufferDesc.Height					= (UINT)this->winDimension.y;
	desc.BufferDesc.Width					= (UINT)this->winDimension.x;
	desc.SampleDesc.Count					= 1;
	desc.SampleDesc.Quality					= 0;
	

	//Get Device Factory
	::IDXGIDevice *dxgiDevice = NULL;
	if( FAILED( this->d3dDevice->QueryInterface( __uuidof( IDXGIDevice ), (void**)&dxgiDevice ) ) )
	{
		printf("Failed to Query for the GPU's dxgiDevice.\nFailed to create swapChain for the GPU.\n");
		return false;
	}

	::IDXGIAdapter *dxgiAdapter = NULL;
	if( FAILED( dxgiDevice->GetParent( __uuidof( IDXGIAdapter ), (void**)&dxgiAdapter ) ) )
	{
		dxgiDevice->Release();
		printf("Failed to get GPU's parent dxgiAdapter.\nFailed to create swapChain for the GPU.\n");
		return false;
	}
	dxgiDevice->Release();

	::IDXGIFactory *dxgiFactory = NULL;
	if( FAILED( dxgiAdapter->GetParent( __uuidof( IDXGIFactory ), (void**)&dxgiFactory ) ) )
	{
		dxgiAdapter->Release();
		printf( "Failed to get GPU's parent dxgiFactory.\nFailed to create swapChain for the GPU.\n");
		return false;
	}
	dxgiAdapter->Release();
	
	//Create SwapChain
	if( FAILED( dxgiFactory->CreateSwapChain( this->d3dDevice, &desc, &this->d3dSwapchain) ) )
	{
		dxgiFactory->Release();
		printf( "Failed to create swapChain for the GPU.\n");
		return false;
	}

	dxgiFactory->Release();

	return true;
}
bool App::Init3D_DepthStencil()
{
	D3D11_TEXTURE2D_DESC desc;
	desc.MipLevels		= 1;
	desc.ArraySize		= 1;
	desc.Format			= DXGI_FORMAT_R32_TYPELESS;
	desc.Usage			= D3D11_USAGE_DEFAULT;
	desc.BindFlags		= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags	= 0;
	desc.MiscFlags		= 0;
	desc.Height			= (UINT)this->winDimension.y;
	desc.Width			= (UINT)this->winDimension.x;
	desc.SampleDesc.Count=1;
	desc.SampleDesc.Quality=0;

	ID3D11Texture2D* depthstencil;

	if(FAILED(this->d3dDevice->CreateTexture2D(&desc,0,&depthstencil)))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format						= DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension				= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags						= 0;
	dsvDesc.Texture2D.MipSlice			= 0;
	if(this->d3dDevice->CreateDepthStencilView( depthstencil, &dsvDesc, &this->depthStencilView) != S_OK)
	{
		depthstencil->Release();
		return false;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format						= DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels			= 1;
	srvDesc.Texture2D.MostDetailedMip	= 0;
	if(FAILED( this->d3dDevice->CreateShaderResourceView( depthstencil, &srvDesc, &this->depthStencilUAV)))
	{
		depthstencil->Release();
		return false;
	}
	depthstencil->Release();
	return true;
}
bool App::Init3D_RTV()
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory( &descView, sizeof(descView) );
	descView.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descView.Texture2D.MipSlice=0;

	ID3D11Texture2D* backBuffer;
	if(FAILED(this->d3dSwapchain->GetBuffer(0,__uuidof(ID3D11Texture2D),reinterpret_cast<void**>(&backBuffer))))
	{
		printf( "Failed to get BackBuffer from Swapchain" );
		return false;
	}

	if(FAILED(this->d3dDevice->CreateRenderTargetView(backBuffer,0,&this->backBufferRTV)))
	{
		printf( "Failed to create RTV for BackBuffer" );
		backBuffer->Release();
		return false;
	}

	if(FAILED(this->d3dDevice->CreateUnorderedAccessView(backBuffer,0,&this->backBufferUAV)))
	{
		printf( "Failed to create UAV for BackBuffer" );
		backBuffer->Release();
		return false;
	}

	backBuffer->Release();

	return true;
}
bool App::Init3D_Viewport()
{
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;
	this->viewPort.Width = (FLOAT)this->winDimension.x;
	this->viewPort.Height = (FLOAT)this->winDimension.y;
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;

	return true;
}
bool App::Init3D_Pipeline()
{
	if(!renderer.Initiate(this->d3dDevice, this->d3dDeviceContext, (int)this->viewPort.Width, (int)this->viewPort.Height))
		return false;

	return true;
}

LRESULT CALLBACK App::WindowCallback(HWND h, UINT m, WPARAM w, LPARAM l)
{
	PAINTSTRUCT ps;
		HDC hdc;

		switch (m) 
		{
			case WM_PAINT:
				hdc = BeginPaint(h, &ps);
				EndPaint(h, &ps);
			break;

			case WM_DESTROY:
				PostQuitMessage(0);
			break;

			case WM_KEYDOWN:
				switch(w)
				{
					case VK_ESCAPE:
						PostQuitMessage(0);
					break;
				}
			break;
		}

	return DefWindowProc(h, m, w, l);
}





