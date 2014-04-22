#ifndef BATCHELOR_SUBSURFACESCATTERINGSCENE_H
#define BATCHELOR_SUBSURFACESCATTERINGSCENE_H

#include "Scene.h"
#include "..\Utilities\Camera.h"

class SubsurfaceScatteringScene :public Scene
{
public:
	struct SSSInitDesc
	{
		int width;
		int height;
	};
public:
	SubsurfaceScatteringScene(SSSInitDesc& desc);
	virtual~SubsurfaceScatteringScene();

	void Frame(float delta) override;
	bool Initiate(ID3D11Device* device, ID3D11DeviceContext *dc) override;
	void SetCamera(Camera* cam);
private:
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;
	Camera *mainCam;

	SSSInitDesc desc;
};

#endif // !BATCHELOR_SUBSURFACESCATTERINGSCENE_H
