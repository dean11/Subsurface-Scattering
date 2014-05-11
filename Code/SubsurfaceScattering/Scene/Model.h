#ifndef BATCHELOR_MESHMANAGER_H
#define BATCHELOR_MESHMANAGER_H

#include <DirectXMath.h>
#include <d3d11_2.h>
#include <D3DTK\DDSTextureLoader.h>
#include "..\Utilities\Util.h"
#include <D3DTK\SimpleMath.h>

class Model
{
public:
	struct Mesh
	{
		unsigned int vertexCount;
		unsigned int vertexStride;
		ID3D11Buffer* vertexBuffer;
		ID3D11ShaderResourceView* diffuse;
		ID3D11ShaderResourceView* thickness;
	};
	
public:
	Model();
	virtual~Model();

	virtual void Release();
	virtual bool CreateModel(const char path[], ID3D11Device* device);

	void DrawModel(ID3D11DeviceContext* dc);

	Mesh& GetMesh();
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT4X4 GetWorld() const;

	DirectX::XMFLOAT4X4 GetWorldInversTranspose();
	DirectX::XMFLOAT4X4 GetWorldInversTranspose() const;

	void SetWorld(DirectX::XMFLOAT4X4 world);
	//void SetWorldInversTranspose(DirectX::XMFLOAT4X4 worldInvTrans); SÄTTA DENNA I SAMBAND MED NY WORLD?

protected:
	Mesh mesh;
	DirectX::SimpleMath::Matrix world;
};

#endif // !BATCHELOR_MESHMANAGER_H
