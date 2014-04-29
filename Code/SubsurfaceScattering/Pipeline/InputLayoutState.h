#ifndef BATCHELOR_INPUTLAYOUTMANAGER_H
#define BATCHELOR_INPUTLAYOUTMANAGER_H

#include <d3d11_2.h>

class InputLayoutManager
{
public:
	static void MicrosoftFailedWithDirectX(ID3D11Device* device, void *v, UINT BYTECODELENGTH);
	static void Release();
	static ID3D11InputLayout* GetLayout_V_VN_VT();
	static ID3D11InputLayout* GetLayout_V_VT();
};

#endif // !BATCHELOR_INPUTLAYOUTMANAGER_H
