#ifndef BATCHELOR_SUBSURFACESCATTERINGSCENE_H
#define BATCHELOR_SUBSURFACESCATTERINGSCENE_H

#include "Scene.h"

class SubsurfaceScatteringScene :public Scene
{
public:
	SubsurfaceScatteringScene();
	virtual~SubsurfaceScatteringScene();

	void Frame(float delta) override;
	void Initiate(ID3D11Device* device, ID3D11DeviceContext *dc) override;

private:
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

};

#endif // !BATCHELOR_SUBSURFACESCATTERINGSCENE_H
