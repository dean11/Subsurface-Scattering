#include "SubsurfaceScatteringScene.h"
#include "..\Input.h"

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
	//if (Input::IsKeyDown(VK_LEFT)) this->spotLight[0].position.x -= 1.0f;
	//if (Input::IsKeyDown(VK_RIGHT)) this->spotLight[0].position.x += 1.0f;
	//if (Input::IsKeyDown(VK_UP)) this->spotLight[0].position.y += 1.0f;
	//if (Input::IsKeyDown(VK_DOWN)) this->spotLight[0].position.y -= 1.0f;
	//
	//if (Input::IsKeyDown(VK_PLUS)) 
	//	for (size_t i = 0; i < this->spotLight.size(); i++)
	//		this->spotLight[i].coneAngle += 1.0f;
	//if (Input::IsKeyDown(VK_MINUS)) 
	//	for (size_t i = 0; i < this->spotLight.size(); i++)
	//		this->spotLight[i].coneAngle -= 1.0f;

	Pipeline::PipelineManager::Instance().ApplyGeometryPass();
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
	
	Pipeline::LightPass::LightData lData;
	memset(&lData, 0, sizeof(Pipeline::LightPass::LightData));

	if (this->pointLights.size())
		lData.pointData = &this->pointLights[0];
	lData.pointCount = (int)this->pointLights.size();

	if (this->spotLight.size())
		lData.spotData = &this->spotLight[0];
	lData.spotCount = this->spotLight.size();

	if (this->directionalLight.size())
		lData.dirData = &this->directionalLight[0];
	lData.dirCount = (int)this->directionalLight.size();

	lData.invProj = this->mainCam->GetInverseProjectionMatrix();
	lData.view = this->mainCam->GetViewMatrix();
	lData.ambientLight = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Pipeline::PipelineManager::Instance().ApplyLightPass(lData);

	//Pipeline::PipelineManager::Instance().ApplySSSPass(true);

	Pipeline::PipelineManager::Instance().Present();
}
bool SubsurfaceScatteringScene::Initiate(ID3D11Device* device, ID3D11DeviceContext *dc)
{
	this->device = device;
	this->deviceContext = dc;

	Model bth;
	if (!bth.CreateModel("Models\\bth.righthanded.obj", device))
	//if (!bth.CreateModel("Models\\bth.lefthanded.obj", device))
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
	//BasicLightData::PointLight pLight;
	//size_t totPoint = 5;
	//for (size_t i = 0; i < totPoint; i++)
	//{
	//	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	pLight.lightColour = DirectX::XMFLOAT3(r, g, b);
	//	pLight.positionRange = DirectX::XMFLOAT4((-((float)totPoint / 2.0f) * 5) + (float)i * 5, -90.0f, -10.0f, 190.0f);
	//	this->pointLights.push_back(pLight);
	//}
	//
	BasicLightData::Directional dl;
	dl.color = DirectX::XMFLOAT3(0.3f, 0.3f, 0.3f);
	dl.direction = DirectX::XMFLOAT3(0.22f, -0.71f, 0.35f);
	this->directionalLight.push_back(dl);
	
	//size_t totSpot = 1;
	//BasicLightData::Spotlight sl;
	//for (size_t i = 0; i < totSpot; i++)
	//{
	//	sl.coneAngle = 20.0f;
	//	sl.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	//	sl.position = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
	//	sl.range = 1000.0f;
	//	sl.unitDir = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	//	sl.attenuation = DirectX::XMFLOAT3(0.4f, 0.02f, 0.0f);
	//	this->spotLight.push_back(sl);
	//}

}

