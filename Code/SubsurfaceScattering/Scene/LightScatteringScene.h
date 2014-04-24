#ifndef LIGHTSCATTERINGSCENE_H
#define LIGHTSCATTERINGSCENE_H

#include "Scene.h"
#include "..\Utilities\Camera.h"
#include "SphereMap.h"

class LightScatteringScene : public Scene
{
public:
	struct LSSInitDesc
	{
		int width;
		int height;
	};
	
	LightScatteringScene(LSSInitDesc& desc);
	virtual~LightScatteringScene();

	void Frame(float delta) override;
	bool Initiate(ID3D11Device* device, ID3D11DeviceContext* dc) override;
	void SetMainCamera(Camera* cam);

private:
	ID3D11Device* device;
	ID3D11DeviceContext* dc;
	Camera* mainCam, *shadowMap;
	LSSInitDesc desc;
	SphereMap sphereMap;
};

#endif