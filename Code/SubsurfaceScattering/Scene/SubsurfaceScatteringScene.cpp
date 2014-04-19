#include "SubsurfaceScatteringScene.h"

SubsurfaceScatteringScene::SubsurfaceScatteringScene()
{

}
SubsurfaceScatteringScene::~SubsurfaceScatteringScene()
{

}


void SubsurfaceScatteringScene::Frame(float delta)
{
	Pipeline::PipelineManager::Instance().ApplyGeometryPass(true);
	{
		ID3D11Buffer* vBuff[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1] = { 0 };
		UINT vStride[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1] = { 0 };

		int counter = 0;
		int tot = 0;
		for (size_t i = 0; i < this->models.size(); i++)
		{
			vBuff[counter] = this->models[i].GetMesh().vertexBuffer;
			vStride[counter] = this->models[i].GetMesh().vertexStride;

			counter++;
			tot += this->models[i].GetMesh().vertexCount;
			UINT off = 0;
			if (counter == D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 || i + 1 >= this->models.size())
			{
				this->deviceContext->IASetVertexBuffers(0, counter, vBuff, vStride, &off);
				this->deviceContext->Draw(tot, 0);
	
				counter = 0;
				tot = 0;
			}
			
			
		}
	}
	
	//Pipeline::PipelineManager::Instance().ApplyLightPass(true);

	//Pipeline::PipelineManager::Instance().ApplySSSPass(true);

	Pipeline::PipelineManager::Instance().Present();
}
bool SubsurfaceScatteringScene::Initiate(ID3D11Device* device, ID3D11DeviceContext *dc)
{
	this->device = device;
	this->deviceContext = dc;

	Model bth;
	if (!bth.CreateModel("Models\\bth.obj", device))
		return false;
	this->models.push_back(bth);

	Model sphere;
	if (!sphere.CreateModel("Models\\sphere.obj", device))
		return false;
	this->models.push_back(sphere);

	return true;
}

