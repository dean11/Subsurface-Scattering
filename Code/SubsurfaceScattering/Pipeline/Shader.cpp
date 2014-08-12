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
	this->type = ShaderType_None;
	this->size = 0;
	this->shaderID = -1;
}
Shader::~Shader()
{
}

bool Shader::CompileShaderToCSO(const char filename[], const char output[], char* target, UINT flag, const D3D_SHADER_MACRO* macro, ShaderType type, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ID3DBlob* s = 0, *err = 0;
	HRESULT hr = S_OK;
	
	if(FAILED ( hr = D3DCompileFromFile(Util::StringToWstring(filename, std::wstring()).c_str(), macro, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, flag, 0, &s, &err)))
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

	std::ofstream f;
	f.open(output, std::ifstream::out | std::ifstream::binary);
	if(f.is_open())
	{
		f.write((char*)s->GetBufferPointer(), s->GetBufferSize());
		f.close();
	}
	s->Release();
	return true;
}
bool Shader::CreateShader(const char filename[], char* target, UINT flag, const D3D_SHADER_MACRO* macro, ShaderType type, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ID3DBlob* s = 0, *err = 0;
	HRESULT hr = S_OK;

	if(!this->device)			this->device = device;
	if(!this->deviceContext)	this->deviceContext = deviceContext;
	
	if(FAILED ( hr = D3DCompileFromFile(Util::StringToWstring(filename, std::wstring()).c_str(), macro, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", target, flag, 0, &s, &err)))
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
	
	this->Release();
	void* raw = s->GetBufferPointer();
	char* rawc = (char*)s->GetBufferPointer();
	size_t size = s->GetBufferSize();

	switch (type)
	{
	case Pipeline::ShaderType_VS:
		hr = this->device->CreateVertexShader(raw, size, 0, &this->shaderData.vertexShader);
		break;
	case Pipeline::ShaderType_PS:
		hr = this->device->CreatePixelShader(raw, size, 0, &this->shaderData.pixelShader);
		break;
	case Pipeline::ShaderType_CS:
		hr = this->device->CreateComputeShader(raw, size, 0, &this->shaderData.computeShader);
		break;
	case Pipeline::ShaderType_HS:
		break;
	case Pipeline::ShaderType_GS:
		hr = this->device->CreateGeometryShader(raw, size, 0, &this->shaderData.geometryShader);
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
	if (!device || !deviceContext)
		return false;

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

		//ShaderData is a union so it does not matter wich variable we release..
		Util::SAFE_RELEASE(this->shaderData.computeShader);

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
	else
	{
		return false;
	}

	return hr == S_OK ? true : false;
}
Shader::ShaderData Shader::GetShader()
{
	return this->shaderData;
}
void Shader::Release()
{
	//ShaderData is a union.
	Util::SAFE_RELEASE(this->shaderData.computeShader);
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

