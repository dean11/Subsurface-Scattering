#include "TextRender.h"
#include <vector>
#include <string>
using namespace DirectX;

static SpriteFont* sfont = 0;
struct TBUFF
{
	TBUFF(){};
	TBUFF(const wchar_t* _t, float _x, float _y)
	{
		t = _t;
		x = _x;
		y = _y;
	}
	std::wstring t;
	float x, y;
};
std::vector<TBUFF> textbuff;

void TextRender::Release()
{
	delete sfont;
}
void TextRender::Initiate(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* filename)
{
	if (sfont)
	{
		delete sfont;
	}

	sfont = new SpriteFont(device, filename);
}


void TextRender::Write(const wchar_t* text, float x, float y)
{
	if (lstrlenW(text) == 0) return;

	textbuff.push_back(TBUFF(text, x, y));
}

void TextRender::Present(SpriteBatch* s)
{
	for (size_t i = 0; i < textbuff.size(); i++)
	{
		sfont->DrawString(s, textbuff[i].t.c_str(), XMFLOAT2(textbuff[i].x, textbuff[i].y), DirectX::Colors::White, 0, DirectX::XMFLOAT2(0.0f, 0.0f) , DirectX::XMFLOAT2(0.6f, 0.6f));
	}
	textbuff.resize(0);
}
	