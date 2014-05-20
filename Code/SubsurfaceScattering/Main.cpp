
#include <vld.h>

#ifdef _WIN32
#include <Windows.h>
#include <ctime>
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

#include "App.h"



int WINAPI WinMain( HINSTANCE hinst, HINSTANCE prevInst, PSTR cmdLine, int cmdShow)
{
	srand (time(NULL));
	App *app = new App();
	
	if(app->Initiate())
	{
		app->Run();
	
		app->Release();
	}
	
	delete app;
}
