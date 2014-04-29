#include "InputLayoutState.h"


namespace
{
	static ID3D11InputLayout* inputLayout_V_VN_VT = NULL;
	static ID3D11InputLayout* GetInputLayout_V_VN_VT(ID3D11Device *device, void*v, UINT BYTECODELENGTH)
	{
		if (!inputLayout_V_VN_VT)
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			if (FAILED(device->CreateInputLayout(desc, 3, v, BYTECODELENGTH, &inputLayout_V_VN_VT)))
				return 0;
		}

		return inputLayout_V_VN_VT;
	}

	static ID3D11InputLayout* inputLayout_V_VT = NULL;
	static ID3D11InputLayout* GetInputLayout_V_VT(ID3D11Device *device, void*v, UINT BYTECODELENGTH)
	{
		if (!inputLayout_V_VT)
		{
			D3D11_INPUT_ELEMENT_DESC desc[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};

			if (FAILED(device->CreateInputLayout(desc, 2, v, BYTECODELENGTH, &inputLayout_V_VT)))
				return 0;
		}

		return inputLayout_V_VT;
	}
}

void InputLayoutManager::MicrosoftFailedWithDirectX(ID3D11Device* device, void*v, UINT BYTECODELENGTH)
{
	GetInputLayout_V_VN_VT(device, v, BYTECODELENGTH);
	GetInputLayout_V_VT(device, v, BYTECODELENGTH);
}
void InputLayoutManager::Release()
{
	if (inputLayout_V_VN_VT) inputLayout_V_VN_VT->Release(); inputLayout_V_VN_VT = 0;
	if (inputLayout_V_VT) inputLayout_V_VT->Release(); inputLayout_V_VT = 0;
}
ID3D11InputLayout* InputLayoutManager::GetLayout_V_VN_VT()
{
	return GetInputLayout_V_VN_VT(0, 0, 0);
}
ID3D11InputLayout* InputLayoutManager::GetLayout_V_VT()
{
	return GetInputLayout_V_VT(0, 0, 0);
}