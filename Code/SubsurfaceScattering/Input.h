#ifndef INPUT_1_H
#define INPUT_1_H
#include <Windows.h>

#define		VK_S		0x53
#define		VK_W		0x57
#define		VK_A		0x41
#define		VK_D		0x44
#define		VK_L		0x4C
#define		VK_P		0x50
#define		VK_Q		0x51
#define		VK_R		0x52
#define		VK_0		96
#define		VK_1		97
#define		VK_2		98
#define		VK_3		99
#define		VK_4		100
#define		VK_5		101
#define		VK_6		102
#define		VK_7		103
#define		VK_8		104
#define		VK_9		105
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
