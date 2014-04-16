#include "Shader.h"
#include "..\Utilities\Util.h"
#include <d3dcompiler.h>

#include <iostream>
#include <fstream>

#pragma comment(lib, "d3dcompiler.lib")

using namespace Pipeline;

static int globaShaderID = 0;
ID3D11Device* Shader::device = 0;
ID3D11DeviceContext* Shader::deviceContext = 0;

Shader::Shader()
{
	memset(&this->shaderData, 0, sizeof(ShaderData));
	this->device = 0;
	this->deviceContext = 0;
	this->type = ShaderType_None;
	this->size = 0;
	this->shaderID = -1;
}
Shader::~Shader()
{
}

bool Shader::CreateShader(const char filename[], char* target, UINT flag, const D3D_SHADER_MACRO* macro, ShaderType type, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if(!this->device)			this->device = device;
	if(!this->deviceContext)	this->deviceContext = deviceContext;

	ID3DBlob* s = 0, *err = 0;
	wchar_t buff[255];
	_wgetcwd(buff, 255);
	HRESULT hr = S_OK;
	if(FAILED ( hr = D3DCompileFromFile(Util::StringToWstring(filename, std::wstring()).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, flag, 0, &s, &err)))
	{
		if(err) 
		{
			char* compileErrors = (char*)(err->GetBufferPointer());
			size_t bufferSize = err->GetBufferSize();

			printf("%s", compileErrors);
			
			err->Release();
			err = 0;
		}

		return false;
	}

	memset(&this->shaderData, 0, sizeof(Shader::ShaderData));
	
	switch (type)
	{
	case Pipeline::ShaderType_VS:
		hr = this->device->CreateVertexShader(s->GetBufferPointer(), s->GetBufferSize(), 0, &this->shaderData.vertexShader);
		break;
	case Pipeline::ShaderType_PS:
		hr = this->device->CreatePixelShader(s->GetBufferPointer(), s->GetBufferSize(), 0, &this->shaderData.pixelShader);
		break;
	case Pipeline::ShaderType_CS:
		hr = this->device->CreateComputeShader(s->GetBufferPointer(), s->GetBufferSize(), 0, &this->shaderData.computeShader);
		break;
	case Pipeline::ShaderType_HS:
		break;
	case Pipeline::ShaderType_GS:
		hr = this->device->CreateGeometryShader(s->GetBufferPointer(), s->GetBufferSize(), 0, &this->shaderData.geometryShader);
		break;
	case Pipeline::ShaderType_DS:
		break;
	}
	
	if(FAILED (hr) )	return false;

	this->path = filename;
	this->size = s->GetBufferSize();
	this->type = type;
	this->shaderID = globaShaderID++;

	if(type == ShaderType_VS)
	{
		//D3D11_INPUT_ELEMENT_DESC desc [] = 
		{
			{}
		};
		//this->device->CreateInputLayout(desc, 0, 0, 0, 0);
	}

	s->Release();

	return true;
}
bool Shader::LoadCompiledShader(const char filename[], ShaderType type, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->type = type;
	this->device = device;
	this->deviceContext = deviceContext;
	this->path = filename;

	std::ifstream fstr;
	HRESULT hr = S_OK;
	fstr.open(filename, std::ifstream::in | std::ifstream::binary);

	if (fstr.good())
	{
		fstr.seekg(0, std::ios::end);
		size_t size = size_t(fstr.tellg());
		this->byteCode = new char[size];
		fstr.seekg(0, std::ios::beg);
		fstr.read(&this->byteCode[0], size);
		fstr.close();

		switch (this->type)
		{
		case Pipeline::ShaderType_VS:
			hr = this->device->CreateVertexShader(&this->byteCode[0], size, 0, &this->shaderData.vertexShader);
			break;
		case Pipeline::ShaderType_PS:
			hr = this->device->CreatePixelShader(this->byteCode, size, 0, &this->shaderData.pixelShader);
			delete[] this->byteCode;
			break;
		case Pipeline::ShaderType_CS:
			hr = this->device->CreateComputeShader(this->byteCode, size, 0, &this->shaderData.computeShader);
			delete[] this->byteCode;
			break;
		case Pipeline::ShaderType_HS:
			hr = this->device->CreateHullShader(this->byteCode, size, 0, &this->shaderData.hullShader);
			delete[] this->byteCode;
			break;
		case Pipeline::ShaderType_GS:
			hr = this->device->CreateGeometryShader(this->byteCode, size, 0, &this->shaderData.geometryShader);
			delete[] this->byteCode;
			break;
		case Pipeline::ShaderType_DS:
			hr = this->device->CreateDomainShader(this->byteCode, size, 0, &this->shaderData.domainShader);
			delete[] this->byteCode;
			break;
		}

		this->size = size;
		this->shaderID = globaShaderID++;
	}
	return true;
}
Shader::ShaderData Shader::GetShader()
{
	return this->shaderData;
}
void Shader::Release()
{
	switch (type)
	{
	case Pipeline::ShaderType_VS:
		if(this->shaderData.data) this->shaderData.vertexShader->Release();
		break;
	case Pipeline::ShaderType_PS:
		if(this->shaderData.data) this->shaderData.pixelShader->Release();
		break;
	case Pipeline::ShaderType_CS:
		if(this->shaderData.data) this->shaderData.computeShader->Release();
		break;
	case Pipeline::ShaderType_HS:
		if (this->shaderData.data) this->shaderData.hullShader->Release();
		break;
	case Pipeline::ShaderType_GS:
		if(this->shaderData.data) this->shaderData.geometryShader->Release();
		break;
	case Pipeline::ShaderType_DS:
		if (this->shaderData.data) this->shaderData.domainShader->Release();
		break;
	}
}
void Shader::Apply()
{
	
	switch (type)
	{
	case Pipeline::ShaderType_VS:
		this->deviceContext->VSSetShader(this->shaderData.vertexShader, 0, 0);
		break;
	case Pipeline::ShaderType_PS:
		this->deviceContext->PSSetShader(this->shaderData.pixelShader, 0, 0);
		break;
	case Pipeline::ShaderType_CS:
		this->deviceContext->CSSetShader(this->shaderData.computeShader, 0, 0);
		break;
	case Pipeline::ShaderType_HS:
		this->deviceContext->HSSetShader(this->shaderData.hullShader, 0, 0);
		break;
	case Pipeline::ShaderType_GS:
		this->deviceContext->GSSetShader(this->shaderData.geometryShader, 0, 0);
		break;
	case Pipeline::ShaderType_DS:
		this->deviceContext->DSSetShader(this->shaderData.domainShader, 0, 0);
		break;
	}
}

char* Shader::GetByteCode()
{
	return this->byteCode;
}
size_t Shader::GetByteCodeSize()
{
	return this->size;
}
void Shader::RemoveByteCode()
{
	delete [] this->byteCode;
	this->byteCode = 0;
}

