#ifndef BATCHELOR_MESHMANAGER_H
#define BATCHELOR_MESHMANAGER_H

#include <DirectXMath.h>
#include <d3d11_2.h>
#include <D3DTK\DDSTextureLoader.h>
#include "..\Utilities\Util.h"

class Model
{
public:
	struct Mesh
	{
		unsigned int vertexCount;
		unsigned int vertexStride;
		ID3D11Buffer* vertexBuffer;
		ID3D11ShaderResourceView* diffuse;
	};
	
public:
	Model();
	virtual~Model();

	void Release();
	bool CreateModel(const char path[], ID3D11Device* device);

	Mesh& GetMesh();
	DirectX::XMFLOAT4X4 GetWorld();
	DirectX::XMFLOAT4X4 GetWorld() const;

	DirectX::XMFLOAT4X4 GetWorldInversTranspose();			//NOT IMPLEMENTED
	DirectX::XMFLOAT4X4 GetWorldInversTranspose() const;	//NOT IMPLEMENTED

	void SetWorld(DirectX::XMFLOAT4X4 world);
	//void SetWorldInversTranspose(DirectX::XMFLOAT4X4 worldInvTrans); S�TTA DENNA I SAMBAND MED NY WORLD?
private:
	Mesh mesh;
	DirectX::XMFLOAT4X4 world;
};

#endif // !BATCHELOR_MESHMANAGER_H
