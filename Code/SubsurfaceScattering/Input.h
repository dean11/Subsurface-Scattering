#ifndef INPUT_1_H
#define INPUT_1_H
#include <Windows.h>

#define		VK_S		0x53
#define		VK_W		0x57
#define		VK_A		0x41
#define		VK_D		0x44
#define		VK_MINUS	109
#define		VK_PLUS		107

class Input
{
public:
	static bool IsKeyDown( UINT key )
	{
		return GetKeys()[key];
	}
	
	static void SetKeyState(UINT key, bool state)
	{
		GetKeys()[key] = state;
	}

private:
	static bool* GetKeys()
	{
		static bool keys[255] = { 0 };

		return keys;
	}
};

#endif // !INPUT_1_H
