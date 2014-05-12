#ifndef BATCHELOR_PIPELINEMANAGER_H
#define BATCHELOR_PIPELINEMANAGER_H

#include <d3d11.h>
#include <vector>

namespace Pipeline
{
	enum ShaderType
	{ 
		ShaderType_VS, 
		ShaderType_PS, 
		ShaderType_CS, 
		ShaderType_HS, 
		ShaderType_GS, 
		ShaderType_DS, 
		ShaderType_None,
	};

	class Shader
	{
	public:
		union ShaderData
		{
			ID3D11VertexShader*		vertexShader;
			ID3D11DomainShader*		domainShader;
			ID3D11HullShader*		hullShader;
			ID3D11GeometryShader*	geometryShader;
			ID3D11ComputeShader*	computeShader;
			ID3D11PixelShader*		pixelShader;
			void*					data;
		};

	public:
		Shader();
		virtual~Shader();
		bool CreateShader(const char filename[], char* target, UINT flag, const D3D_SHADER_MACRO* macro, ShaderType type, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		bool LoadCompiledShader(const char filename[], ShaderType type, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		ShaderData GetShader();
		void Release();
		void Apply(ID3D11DeviceContext *dc);

		char* GetByteCode();
		size_t GetByteCodeSize();
		void RemoveByteCode();

	private:
		static ID3D11DeviceContext* deviceContext;
		static ID3D11Device* device;
		std::string path;
		ShaderType type;
		ShaderData shaderData;
		int size;
		int shaderID;
		char* byteCode;

	};

}
#endif // !BATCHELOR_PIPELINEMANAGER_H
