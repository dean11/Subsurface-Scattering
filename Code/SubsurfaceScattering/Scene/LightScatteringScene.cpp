#include "LightScatteringScene.h"
#include "..\Utilities\BasicLightData.h"

LightScatteringScene::LightScatteringScene(LSSInitDesc& desc)
{
	this->desc = desc;
	
}

LightScatteringScene::~LightScatteringScene()
{
	for (size_t i = 0; i < this->models.size(); i++)
	{
		this->models[i].Release();
	}
}

void LightScatteringScene::Frame(float delta)
{
	if (this->directionalLight.size() > 0)
	{

		for (size_t i = 0; i < this->directionalLight.size(); i++)
		{
			//Pipeline::PipelineManager::Instance().SetDepthPointLightData(this->directionalLight[i].positionRange);

			/*Pipeline::PipelineManager::Instance().RenderDepthMap(pos, Pipeline::DepthPass::CubeFace::PositiveX);
			RenderDepthMap();

			Pipeline::PipelineManager::Instance().RenderDepthMap(pos, Pipeline::DepthPass::CubeFace::NegativeX);
			RenderDepthMap();

			Pipeline::PipelineManager::Instance().RenderDepthMap(pos, Pipeline::DepthPass::CubeFace::PositiveY);
			RenderDepthMap();

			Pipeline::PipelineManager::Instance().RenderDepthMap(pos, Pipeline::DepthPass::CubeFace::NegativeY);
			RenderDepthMap();

			Pipeline::PipelineManager::Instance().RenderDepthMap(pos, Pipeline::DepthPass::CubeFace::PositiveZ);
			RenderDepthMap();

			Pipeline::PipelineManager::Instance().RenderDepthMap(pos, Pipeline::DepthPass::CubeFace::NegativeZ);
			RenderDepthMap();*/

			Pipeline::PipelineManager::Instance().ApplyDepthPass(Pipeline::DepthPass::DepthMapType::SingleDepthMap);
			//SINGLE DEPTHMAP REMOVE THIS FROM HERE WHEN ALL IS WORKING
			Pipeline::PipelineManager::Instance().RenderDepthMap(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f), this->directionalLight[i].direction);
			RenderDepthMap();
			//########################################
		}

	}

	Pipeline::PipelineManager::Instance().ApplyGeometryPass();
	{
		DirectX::XMFLOAT4X4 m;
		DirectX::XMFLOAT3 f(0.2f, 0.3f, 1.0f);
		XMStoreFloat4x4(&m, DirectX::XMMatrixRotationNormal(DirectX::XMLoadFloat3(&f), 0.7f));

		Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix());

		for (size_t i = 0; i < this->models.size(); i++)
		{
			UINT off = 0;
			this->dc->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
			this->dc->IASetIndexBuffer(this->models[i].GetMesh().indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			this->dc->PSSetShaderResources(0, 1, &this->models[i].GetMesh().diffuse);
			Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->models[i].GetWorld(), this->models[i].GetWorldInversTranspose());
			//this->dc->Draw(this->models[i].GetMesh().vertexCount, 0);
			this->dc->DrawIndexed(this->models[i].GetMesh().indexCount, 0, 0);
		}

		this->ground.Render(this->dc);
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

	lData.proj = Pipeline::PipelineManager::Instance().GetDepthCameraProj();
	lData.view = Pipeline::PipelineManager::Instance().GetDepthCameraView();
	lData.ambientLight = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Pipeline::PipelineManager::Instance().ApplyLightPass(lData);

	Pipeline::PipelineManager::Instance().Present();
}

bool LightScatteringScene::Initiate(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	this->device = device;
	this->dc = dc;

	Model bth;
	if (!bth.CreateModel("Models\\sphere.obj", device))
		return false;
	//if (!bth.CreateModel("Models\\box.obj", device))
		//return false;

	this->models.push_back(bth);

	//this->sphereMap.CreateSkyBox(device, dc);

	this->ground.CreatePlane(device, DirectX::XMFLOAT3(0.0, -100.0f, 0.0f), L"Models\\gray.dds", 1000.0f, 1000.0f, 1.0f);

	CreateLights();

	return true;
}

void LightScatteringScene::SetMainCamera(Camera* cam)
{
	this->mainCam = cam;
}

void LightScatteringScene::CreateLights()
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
	dl.color = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
	//dl.direction = DirectX::XMFLOAT3(0.22f, -0.71f, 0.35f);
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

void LightScatteringScene::RenderDepthMap()
{
	DirectX::XMFLOAT4X4 m;
	DirectX::XMFLOAT3 f(0.2f, 0.3f, 1.0f);
	XMStoreFloat4x4(&m, DirectX::XMMatrixRotationNormal(DirectX::XMLoadFloat3(&f), 0.7f));

	Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(Pipeline::PipelineManager::Instance().GetDepthCameraView(),
		Pipeline::PipelineManager::Instance().GetDepthCameraProj());
	//Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix());

	for (size_t i = 0; i < this->models.size(); i++)
	{
		UINT off = 0;
		this->dc->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
		Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->models[i].GetWorld(), this->models[i].GetWorldInversTranspose());
		this->dc->Draw(this->models[i].GetMesh().vertexCount, 0);
	}

	this->ground.RenderForDepthMap(this->dc);

}