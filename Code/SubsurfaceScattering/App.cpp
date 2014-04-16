#include "App.h"
#include <stdio.h>
#include <cstdio>
#include "Utilities\clock.h"
#include "Pipeline\PipelineManager.h"
#include "Utilities\WindowShell.h"
#include "Scene\SubsurfaceScatteringScene.h"
#include <sys\stat.h>
#include <d3d11_2.h>

#define SUBSURFACESCATTERING
Scene *currentScene = 0;


App::App()
	:	isInitiated(false)
	,	isRunning (false)
	,	winDimension(800, 600)
	,	d3dDevice(0)
	,	d3dDeviceContext(0)
{
	
}
App::~App()
{}
bool App::Initiate()
{
	if(!this->InitWindow())					return false;
	if(!this->Init3D_DeviceAndContext())	return false;
	if(!this->Init3D_Pipeline())			return false;

	//Initiate your scene..
#ifdef SUBSURFACESCATTERING
	currentScene = new SubsurfaceScatteringScene();
	currentScene->Initiate(this->d3dDevice, this->d3dDeviceContext);
#elif defined VOLUMELIGHT
	currentScene = new VolumetricLightingScene();
	currentScene->Initiate();
#endif

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

		currentScene->Frame(dt);
	}
}
void App::Release()
{
	if(isInitiated)
	{
		Pipeline::PipelineManager::Instance().Release();

		this->d3dDevice->Release();
		this->d3dDeviceContext->Release();
		this->isInitiated = false;
		this->isRunning = false;
	}
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

bool App::Init3D_Pipeline()
{
	if (!Pipeline::PipelineManager::Instance().Initiate(this->d3dDevice, this->d3dDeviceContext, (int)this->winDimension.x, (int)this->winDimension.y))
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





