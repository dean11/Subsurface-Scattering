#ifndef BACHELOR_SCENE_H
#define BACHELOR_SCENE_H

#include <d2d1.h>
#include <vector>
#include "Model.h"
#include "..\Pipeline\PipelineManager.h"

class Scene
{
public:
	virtual void Frame(float delta) = 0;
	virtual bool Initiate(ID3D11Device* device, ID3D11DeviceContext *dc) = 0;

protected:
	std::vector<Model> models;
};

#endif // !BACHELOR_SCENE_H
