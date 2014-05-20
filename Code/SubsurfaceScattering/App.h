#ifndef BACHELOR_APP_H
#define BACHELOR_APP_H

#pragma comment(lib, "d3d11.lib")
#if defined(_DEBUG) || defined(DEBUG)
#pragma comment(lib, "DirectXTK_debug.lib")
#else
#pragma comment(lib, "DirectXTK_release.lib")
#endif
#include <vld.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Scene\Renderer.h"
#include "Utilities\Camera.h"
#include "Utilities\DynamicArray.h"

class App
{
	public:
		App();
		virtual~App();
		bool Initiate();
		void Run();
		void Release();

	private:
		bool InitWindow();
	
		static LRESULT CALLBACK WindowCallback(HWND h, UINT m, WPARAM w, LPARAM l);

	private:
		DirectX::XMINT2 winDimension;
		bool isInitiated;
		bool isRunning;
		bool pause;

		Camera camera;

		Renderer *renderer;
};

#endif // !BACHELOR_APP_H
