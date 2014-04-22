#ifndef BACHELOR_APP_H
#define BACHELOR_APP_H

#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <DirectXMath.h>

#include "Scene\Scene.h"
#include "Utilities\Camera.h"

class App
{
	public:
		App();
		virtual~App();
		bool Initiate();
		void Run();
		void Release();

	private:
		bool Init3D_DeviceAndContext();
		bool Init3D_Pipeline();
		bool InitWindow();
	
		static LRESULT CALLBACK WindowCallback(HWND h, UINT m, WPARAM w, LPARAM l);

	private:
		DirectX::XMINT2 winDimension;
		bool isInitiated;
		bool isRunning;

		ID3D11Device* d3dDevice;
		ID3D11DeviceContext* d3dDeviceContext;
		
		Camera camera;

		Scene *subsurface;
		Scene *volumeLight;

		struct Keyboard
		{
			bool W;
			bool A;
			bool S;
			bool D;
		} moveKeys;
};

#endif // !BACHELOR_APP_H
