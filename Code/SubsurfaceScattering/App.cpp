#include "App.h"
#include <stdio.h>
#include <cstdio>
#include "Utilities\clock.h"
#include "Pipeline\PipelineManager.h"
#include "Utilities\WindowShell.h"
#include "Scene\Renderer.h"
#include "Scene\Renderer.h"
#include <sys\stat.h>
#include <d3d11_2.h>
#include <windowsx.h>
#include "Input.h"
#include <D3DTK\SpriteFont.h>


App* app = 0;
int oldX = 0;
int oldY = 0;
bool LBUTTONDOWN = false;

App::App()
	:	isInitiated(false)
	,	isRunning (false)
	,	pause(false)
	,	winDimension(1280, 720)
{
	app = this;
}
App::~App()
{}
bool App::Initiate()
{
	WindowShell::WINDOW_INIT_DESC wdesc;
	wdesc.windowName = L"I draw";
	wdesc.windowSize.x = this->winDimension.x;
	wdesc.windowSize.y = this->winDimension.y;
	wdesc.windowProcCallback = App::WindowCallback;
	
	
	if(!WindowShell::CreateWin(wdesc))
		return false;

	WindowShell::CreateConsoleWindow();

	this->camera.SetPosition(0.0f, 0.0f, -5.0f);
	this->camera.SetProjectionMatrix(((DirectX::XM_PI / 180.0f) * 45.0f), ((float)this->winDimension.x / (float)this->winDimension.x), 0.1f, 10000.0f);
	this->camera.Render();

	Renderer::RendererInitDesc desc;
	desc.height = this->winDimension.y;
	desc.width = this->winDimension.x;
	this->renderer = new Renderer();
	this->renderer->SetMainCamera(&this->camera);

	if(!this->renderer->Initiate(desc)) 
		return false;
	

	return (this->isInitiated = true);
}
void App::Run()
{
	this->isRunning = true;
	Utility::WinTimer clock;
	clock.reset();
	
	while (WindowShell::Frame() && this->isRunning)
	{
		if(pause) 
		{
			Sleep(100);
		}
		else
		{
			this->camera.Render();
		
			if (Input::IsKeyDown(VK_W))		this->camera.RelativeForward(0.5f);
			if (Input::IsKeyDown(VK_S))		this->camera.RelativeForward(-0.5f);
			if (Input::IsKeyDown(VK_A))		this->camera.RelativeRight(-0.5f);
			if (Input::IsKeyDown(VK_D))		this->camera.RelativeRight(0.5f);
			if (Input::IsKeyDown(VK_CONTROL))	app->camera.RelativeUp(-0.5f);
			if (Input::IsKeyDown(VK_SPACE))		app->camera.RelativeUp(0.5f);
			

			float dt = (float)clock.getElapsedSeconds();
			clock.reset();

			this->renderer->Frame(dt);
		}
	}
}
void App::Release()
{
	if(isInitiated)
	{
		Pipeline::PipelineManager::Instance().Release();

		this->isInitiated = false;
		this->isRunning = false;

		
		this->renderer->Release();
		delete this->renderer;
		this->renderer = 0;
		
	}
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

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			Input::SetKeyState(w, false);
		} break;
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:

			if (w == VK_ESCAPE)		PostQuitMessage(0);
			else					Input::SetKeyState(w, true);
		break;

		case WM_LBUTTONDOWN:
			oldX = GET_X_LPARAM(l);
			oldY = GET_Y_LPARAM(l);
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
			bool act = (LOWORD(w) != WA_INACTIVE) && (HIWORD(w) == 0);
			if(act)
				app->pause = false;
			else
				app->pause = true;

			oldX = GET_X_LPARAM(l);
			oldY = GET_Y_LPARAM(l);
		}
			break;
	}

	return DefWindowProc(h, m, w, l);
}





