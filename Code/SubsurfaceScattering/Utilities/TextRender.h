#ifndef TEXTRENDERING__H
#define TEXTRENDERING__H

#include "..\Pipeline\PipelineManager.h"
#include <D3DTK\SpriteFont.h>

class TextRender
{
public:
	static void Release();

	static void Initiate(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* filename);

	static void Write(const wchar_t* text, float x, float y);

private:
	friend class Pipeline::PipelineManager;
	static void Present(DirectX::SpriteBatch* s);

};

#endif // !TEXTRENDERING__H
