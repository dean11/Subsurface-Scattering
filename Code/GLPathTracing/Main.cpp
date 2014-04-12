#ifdef _WIN32
#include <Windows.h>
void SetDllPath()
{
#if defined(_WIN64)
	SetDllDirectory(L"..\\DLL\\x64");
#elif defined(_WIN32)
	SetDllDirectory(L"..\\DLL\\x86");
#endif
}
#else
void SetDllPath(){}
#endif

#include <vld.h>
#include "App.h"

int WINAPI WinMain( HINSTANCE hinst, HINSTANCE prevInst, PSTR cmdLine, int cmdShow)
{
	

	App *app = new App();

	if(app->Initiate())
	{
		app->Run();

		app->Release();
	}
	
	delete app;
}
