#include "App.h"
#include <stdio.h>
#include <cstdio>
#include "Utilities\clock.h"
#include "Pipeline\PipelineManager.h"
#include "Utilities\WindowShell.h"
#include "Scene\SubsurfaceScatteringScene.h"
#include "Scene\LightScatteringScene.h"
#include <sys\stat.h>
#include <d3d11_2.h>
#include <windowsx.h>

#define SUBSURFACESCATTERING
//#define LIGHTSCATTERING
Scene *currentScene = 0;
App* app = 0;
int oldX = 0;
int oldY = 0;
bool LBUTTONDOWN = false;

App::App()
	:	isInitiated(false)
	,	isRunning (false)
	,	winDimension(800, 600)
	,	d3dDevice(0)
	,	d3dDeviceContext(0)
{
	app = this;
	memset(&this->moveKeys, 0, sizeof(Keyboard));
}
App::~App()
{}
bool App::Initiate()
{
	if(!this->InitWindow())					return false;
	if(!this->Init3D_DeviceAndContext())	return false;
	if(!this->Init3D_Pipeline())			return false;

	this->camera.SetPosition(0.0f, 0.0f, -50.0f);
	this->camera.SetProjectionMatrix(((DirectX::XM_PI / 180.0f) * 45.0f), ((float)this->winDimension.x / (float)this->winDimension.x), 0.1f, 10000.0f);
	this->camera.Render();


	//Initiate your scene..
#ifdef SUBSURFACESCATTERING
	SubsurfaceScatteringScene::SSSInitDesc desc;
	desc.height = this->winDimension.y;
	desc.width = this->winDimension.x;
	currentScene = new SubsurfaceScatteringScene(desc);
	currentScene->Initiate(this->d3dDevice, this->d3dDeviceContext);
	((SubsurfaceScatteringScene*)currentScene)->SetCamera(&this->camera);
#elif defined LIGHTSCATTERING
	LightScatteringScene::LSSInitDesc desc;
	desc.height = this->winDimension.y;
	desc.width = this->winDimension.x;
	currentScene = new LightScatteringScene(desc);
	currentScene->Initiate(this->d3dDevice, this->d3dDeviceContext);
	((LightScatteringScene*)currentScene)->SetMainCamera(&this->camera);
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

		this->camera.Render();
		
		if (this->moveKeys.W)
			this->camera.RelativeForward(0.5f);
		if (this->moveKeys.S)
			this->camera.RelativeForward(-0.5f);
		if (this->moveKeys.A)
			this->camera.RelativeRight(-0.5f);
		if (this->moveKeys.D)
			this->camera.RelativeRight(0.5f);

		if(this->moveKeys.LEFT)
			app->camera.RelativeYaw(-0.5f);
		if (app->moveKeys.RIGHT)
			app->camera.RelativeYaw(0.5f);
		if (app->moveKeys.UP)
			app->camera.RelativePitch(-0.5f);
		if (app->moveKeys.DOWN)
			app->camera.RelativePitch(0.5f);
		
		if (app->moveKeys.CTRL)
			app->camera.RelativeUp(-0.5f);
		if (app->moveKeys.SPACE)
			app->camera.RelativeUp(0.5f);
			

		if (currentScene) currentScene->Frame(dt);
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
	desc.windowName = L"I draw";
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
	{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
	D3D_FEATURE_LEVEL initiatedFeatureLevel;
	HRESULT hr = S_OK;
	if( FAILED( hr = ::D3D11CreateDevice(	NULL, // default adapter
											driverType,
											NULL, // no software device
											createDeviceFlags,
											featureLevelsToTry, 2,
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

		case WM_KEYUP:
		{
			switch (w)
			{
			case 0x53: //S
				app->moveKeys.S = false;
				break;
			case 0x57: //W
				app->moveKeys.W = false;
				break;
			case 0x41: //A
				app->moveKeys.A = false;
				break;
			case 0x44: //D
				app->moveKeys.D = false;
				break;
			case VK_LEFT:
				app->moveKeys.LEFT = false;
				break;
			case VK_RIGHT:
				app->moveKeys.RIGHT = false;
				break;
			case VK_UP:
				app->moveKeys.UP = false;
				break;
			case VK_DOWN:
				app->moveKeys.DOWN = false;
				break;
			case VK_CONTROL:
				app->moveKeys.CTRL = false;
				break;
			case VK_SPACE:
				app->moveKeys.SPACE = false;
				break;
			}
		} break;
		case WM_KEYDOWN:
			switch(w)
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
				break;
				case 0x53: //S
					app->moveKeys.S = true;
					break;
				case 0x57: //W
					app->moveKeys.W = true;
					break;
				case 0x41: //A
					app->moveKeys.A = true;
					break;
				case 0x44: //D
					app->moveKeys.D = true;
					break;
				case VK_LEFT:
					app->moveKeys.LEFT = true;
					app->camera.RelativeYaw(-2.0f);
					break;
				case VK_RIGHT:
					app->moveKeys.RIGHT = true;
					app->camera.RelativeYaw(2.0f);
					break;
				case VK_UP:
					app->moveKeys.UP = true;
					app->camera.RelativePitch(-2.0f);
					break;
				case VK_DOWN:
					app->moveKeys.DOWN = true;
					app->camera.RelativePitch(2.0f);
					break;
				case VK_CONTROL:
					app->moveKeys.CTRL = true;
					break;
				case VK_SPACE:
					app->moveKeys.SPACE = true;
					break;
			}
		break;

		case WM_LBUTTONDOWN:
			LBUTTONDOWN = true;
			break;
		case WM_LBUTTONUP:
			LBUTTONDOWN = false;
			break;
		case WM_MOUSEMOVE:
		{
			int xPos = GET_X_LPARAM(l);
			int yPos = GET_Y_LPARAM(l);
			if (LBUTTONDOWN)
			{
				 app->camera.RelativeYaw(((float)(xPos - oldX)) * 0.1f);
				 app->camera.RelativePitch(((float)(yPos - oldY))* 0.1f);
			}
			oldX = xPos;
			oldY = yPos;
		}
		break;
		case WM_ACTIVATEAPP:
		case WM_NCACTIVATE:
		case WM_ACTIVATE:
		{
			oldX = GET_X_LPARAM(l);
			oldY = GET_Y_LPARAM(l);
		}
			break;
	}

	return DefWindowProc(h, m, w, l);
}





