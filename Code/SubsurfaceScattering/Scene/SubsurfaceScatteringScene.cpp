#include "SubsurfaceScatteringScene.h"

SubsurfaceScatteringScene::SubsurfaceScatteringScene(SSSInitDesc& desc)
{
	this->desc = desc;
}
SubsurfaceScatteringScene::~SubsurfaceScatteringScene()
{
	for (size_t i = 0; i < this->models.size(); i++)
	{
		this->models[i].Release();
	}
}


void SubsurfaceScatteringScene::Frame(float delta)
{
	Pipeline::PipelineManager::Instance().ApplyGeometryPass(true);
	{
		Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix());

		for (size_t i = 0; i < this->models.size(); i++)
		{
			UINT off = 0;
			this->deviceContext->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
			this->deviceContext->PSSetShaderResources(0, 1, &this->models[i].GetMesh().diffuse);
			Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->models[i].GetWorld(), this->models[i].GetWorldInversTranspose());
			this->deviceContext->Draw(this->models[i].GetMesh().vertexCount, 0);
		}

		this->ground.Render(this->deviceContext);

		this->sphereMap.Render(delta, this->mainCam->GetPosition());
	}
	
	Pipeline::PipelineManager::LightData lData;
	memset(&lData, 0, sizeof(Pipeline::PipelineManager::LightData));
	lData.pointData = &this->pointLights[0];
	lData.pointCount = (int)this->pointLights.size();

	Pipeline::PipelineManager::Instance().ApplyLightPass(lData);

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

	this->sphereMap.CreateSkyBox(device, dc);

	this->ground.CreatePlane(device, DirectX::XMFLOAT3(0.0, -100.0f, 0.0f), L"Models\\gray.dds", 1000.0f, 1000.0f, 1.0f);
	
	CreateLights();

	return true;
}
void SubsurfaceScatteringScene::SetCamera(Camera* cam)
{
	this->mainCam = cam;
}

void SubsurfaceScatteringScene::CreateLights()
{
	BasicLightData::PointLight pLight;

	for (size_t i = 0; i < 255; i++)
	{
		pLight.attenuation = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		pLight.lightColour = DirectX::XMFLOAT4(0.7f, 1.0f, 0.4f, 0.0f);
		pLight.positionRange = DirectX::XMFLOAT4((float)i - ((float)255 / (float)2), 10.0f, 10.0f, 20.0f);
		this->pointLights.push_back(pLight);
	}
}

