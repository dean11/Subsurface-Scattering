#ifndef RENDERER_H
#define RENDERER_H

#include <dxgidebug.h>

#include "..\Utilities\Camera.h"
#include "SphereMap.h"
#include "Plane.h"
#include "..\Utilities\DynamicArray.h"
#include "..\Utilities\clock.h"


class Renderer
{
public:
	struct RendererInitDesc
	{
		int width;
		int height;
	};

public:
	void WaitForDataAndUpdate ();
	Renderer();
	virtual~Renderer();

	void Release();
	void Frame(float delta);
	bool Initiate(RendererInitDesc& desc);
	
	void SetMainCamera(Camera* cam);

private:
	Utility::WinTimer clock;
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;

	Camera *mainCam;
	Plane *ground;
	RendererInitDesc desc;
	std::vector<Model*> models;
	SphereMap sphereMap;

	Util::DynamicArray<BasicLightData::PointLight> pointLights;
	Util::DynamicArray<BasicLightData::Directional> directionalLight;
	Util::DynamicArray<BasicLightData::Spotlight> spotLight;
	Util::DynamicArray<BasicLightData::ShadowMapLight> shadowMaps;
	Model* player;

private:
	void RenderShadowMaps();
	void RenderGeometry();
	void RenderPostPass(float dt);

	void PrintStats(float dt);

	bool CreateLights();
	bool CreateDeviceAndContext();

};

#endif // !BATCHELOR_SUBSURFACESCATTERINGSCENE_H