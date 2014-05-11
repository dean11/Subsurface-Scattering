#include "Renderer.h"
#include "..\Utilities\DepthCamera.h"
#include "..\Input.h"
#include "..\Utilities\TextRender.h"


static float time = 0.0f;
static float timeMax = 0.5f;
std::string geomPassStrign = "Geometry pass: ";
std::string lightPassStrign = "Light pass: ";
std::string depthPassString = "Depth pass: ";
std::string finalPassString = "Final pass: ";
std::string totalStatsString = "Total stats: ";

Renderer::Renderer()
{
}
Renderer::~Renderer()
{
	
}

void Renderer::Release()
{
	for (size_t i = 0; i < this->models.size(); i++)
	{
		this->models[i].Release();
	}

	for (size_t i = 0; i < this->lightDepthSurface.Size(); i++)
	{
		this->lightDepthSurface[i].Release();
	}

	this->device->Release();
	this->deviceContext->Release();
}
void Renderer::Frame(float delta)
{
	time += delta;

	RenderShadowMaps();
	RenderGeometry();
	RenderSSS();
	RenderLights();
	RenderFinal(delta);

	if (time > timeMax)
		time = 0.0f;
}
bool Renderer::Initiate(RendererInitDesc& desc)
{
	this->desc = desc;
	if(!CreateDeviceAndContext()) 
		return false;

	if (!Pipeline::PipelineManager::Instance().Initiate(this->device, this->deviceContext, (int)this->desc.width, (int)this->desc.height))
		return false;


	Model bth;
	if (!bth.CreateModel("Models\\bth.righthanded.obj", device))
		return false;

	SimpleMath::Matrix m = SimpleMath::Matrix::CreateFromYawPitchRoll(0.2f, 0.3f, 1.0f);
		bth.SetWorld(m);

	this->models.push_back(bth);

	//this->sphereMap.CreateSkyBox(device, dc);

	this->ground.CreatePlane(device, DirectX::XMFLOAT3(0.0, -100.0f, 0.0f), L"Models\\gray.dds", 1000.0f, 1000.0f, 1.0f);
	this->models.push_back(this->ground);

	if(!CreateLights())
		return false;
	
	TextRender::Initiate(this->device, this->deviceContext, L"Models\\arial.spritefont");

	return true;
}
void Renderer::SetMainCamera(Camera* cam)
{
	this->mainCam = cam;
}

void Renderer::PrintStats(float dt)
{
	if (time > timeMax)
	{
		totalStatsString.resize(0);
		totalStatsString = "Total ";
		char a[50];
		//FPS
		sprintf_s(a, "%d", (int)(1.0f / dt));
		totalStatsString.append("FPS: ");
		totalStatsString.append(a);
		//DELTA TIME
		sprintf_s(a, "%f", dt);
		totalStatsString.append(" - ms:");
		totalStatsString.append(a);
	}

	TextRender::Write(Util::StringToWstring(totalStatsString, std::wstring()).c_str(), 10, 210);
	TextRender::Write(Util::StringToWstring(geomPassStrign, std::wstring()).c_str(), 10, 230);
	TextRender::Write(Util::StringToWstring(lightPassStrign, std::wstring()).c_str(), 10, 250);
	TextRender::Write(Util::StringToWstring(depthPassString, std::wstring()).c_str(), 10, 270);
	TextRender::Write(Util::StringToWstring(finalPassString, std::wstring()).c_str(), 10, 290);
}
bool Renderer::CreateLights()
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
	this->directionalLight.Push(dl);
	
	dl.color = DirectX::XMFLOAT3(0.8f, 0.4f, 0.8f);
	dl.direction = DirectX::XMFLOAT3(0.9f, -0.9f, 0.9f);
	this->directionalLight.Push(dl);

	//Light casts shadow and sss
	ShadowMap lds;
	SimpleMath::Matrix lproj = SimpleMath::Matrix::CreateOrthographic((float)this->desc.width, (float)this->desc.height, 0.1f, 1000.0f);
	SimpleMath::Matrix lview = SimpleMath::Matrix::Identity();
	if(!lds.Create(this->device, this->deviceContext, this->desc.width, this->desc.height, lview, lproj))
		return false;
	this->lightDepthSurface.Push(lds);

	//Light casts shadow and sss
	ShadowMap lds1;
	//lds1.projection = SimpleMath::Matrix::CreateOrthographic((float)this->desc.width, (float)this->desc.height, 0.1f, 1000.0f);
	lproj = XMLoadFloat4x4(& this->mainCam->GetProjectionMatrix() );
	lview = SimpleMath::Matrix::Identity();
	if(!lds1.Create(this->device, this->deviceContext, this->desc.width, this->desc.height, lview, lproj))
		return false;
	this->lightDepthSurface.Push(lds1);
	
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

	return true;
}
bool Renderer::CreateDeviceAndContext()
{
	::D3D_DRIVER_TYPE driverType = ::D3D_DRIVER_TYPE_HARDWARE;
		
	UINT createDeviceFlags = ::D3D11_CREATE_DEVICE_SINGLETHREADED;
	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevelsToTry[] = 
	{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
	D3D_FEATURE_LEVEL initiatedFeatureLevel;
	HRESULT hr = S_OK;
	if( FAILED( hr = ::D3D11CreateDevice(	NULL, // default adapter
											driverType,
											NULL, // no software device
											createDeviceFlags,
											featureLevelsToTry, Util::NumElementsOf(featureLevelsToTry),
											D3D11_SDK_VERSION,
											&this->device,
											&initiatedFeatureLevel,
											&this->deviceContext ) ) )
	{ // if failed
		return false;
	}

	return true;
}

void Renderer::RenderShadowMaps()
{
	clock.reset();

	//this->lightDepthSurface[1].view = XMLoadFloat4x4(& this->mainCam->GetViewMatrix() );

	//Render scene from each view
	for (size_t i = 0; i < this->lightDepthSurface.Size(); i++)
	{
		this->lightDepthSurface[i].Begin();
		{
			for (size_t i = 0; i < this->models.size(); i++)
			{
				Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->models[i].GetWorld(), this->models[i].GetWorldInversTranspose());
				this->models[i].DrawModel(this->deviceContext);
			}

		} this->lightDepthSurface[i].End();
	}

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		depthPassString.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); depthPassString.append("Depth pass - FPS: "); depthPassString.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); depthPassString.append(" - ms:"); depthPassString.append(a);
	}

	//if (this->directionalLight.Size() > 0)
	//{
	//	Pipeline::PipelineManager::Instance().ApplyDepthPass();
	//
	//	for (size_t i = 0; i < this->directionalLight.Size(); i++)
	//	{
	//		//Pipeline::PipelineManager::Instance().RenderDepthMap(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f), this->directionalLight[i].direction);
	//
	//		DirectX::XMFLOAT4X4 m;
	//		DirectX::XMFLOAT3 f(0.2f, 0.3f, 1.0f);
	//		XMStoreFloat4x4(&m, DirectX::XMMatrixRotationNormal(DirectX::XMLoadFloat3(&f),0.7f));
	//
	//		//Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(Pipeline::PipelineManager::Instance().GetDepthCameraView(),
	//		//															Pipeline::PipelineManager::Instance().GetDepthCameraProj());
	//	   
	//		for (size_t i = 0; i < this->models.size(); i++)
	//		{
	//			UINT off = 0;
	//			this->deviceContext->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
	//			Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(m, this->models[i].GetWorldInversTranspose());
	//			this->deviceContext->Draw(this->models[i].GetMesh().vertexCount, 0);
	//		}
	//
	//		this->ground.RenderForDepthMap(this->deviceContext);
	//	}
	//}
	
}
void Renderer::RenderGeometry()
{
	clock.reset();

	Pipeline::PipelineManager::Instance().ApplyGeometryPass();
	{
		Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix());
		
		for (size_t i = 0; i < this->models.size(); i++)
		{
			this->models[i].DrawModel(this->deviceContext);
		}
	}

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		geomPassStrign.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); geomPassStrign.append("Geometry pass - FPS: "); geomPassStrign.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); geomPassStrign.append(" - ms:"); geomPassStrign.append(a);
	}
}
void Renderer::RenderLights()
{
	clock.reset();

	Pipeline::LightPass::LightData lData;
	memset(&lData, 0, sizeof(Pipeline::LightPass::LightData));

	if (this->pointLights.Size())
		lData.pointData = &this->pointLights[0];
	lData.pointCount = (int)this->pointLights.Size();

	if (this->spotLight.Size())
		lData.spotData = &this->spotLight[0];
	lData.spotCount = this->spotLight.Size();

	if (this->directionalLight.Size())
		lData.dirData = &this->directionalLight[0];
	lData.dirCount = (int)this->directionalLight.Size();

	lData.invProj = this->mainCam->GetInverseProjectionMatrix();
	lData.view = this->mainCam->GetViewMatrix();
	lData.ambientLight = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Pipeline::PipelineManager::Instance().ApplyLightPass(lData);

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		lightPassStrign.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); lightPassStrign.append("Light pass - FPS: "); lightPassStrign.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); lightPassStrign.append(" - ms:"); lightPassStrign.append(a);
	}
}
void Renderer::RenderSSS()
{
	if (this->lightDepthSurface.Size() > 0)
	{
		for (size_t i = 0; i < this->lightDepthSurface.Size(); i++)
		{
			//Pipeline::PipelineManager::Instance().ApplySSSPass(
		}
		//Pipeline::PipelineManager::Instance().ApplyTranslucentShadowMap();

		//for (size_t i = 0; i < this->directionalLight.Size(); i++)
		//{
		//	Pipeline::PipelineManager::Instance().RenderDepthMap(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f), this->directionalLight[i].direction);
		//
		//	DirectX::XMFLOAT4X4 m;
		//	DirectX::XMFLOAT3 f(0.2f, 0.3f, 1.0f);
		//	XMStoreFloat4x4(&m, DirectX::XMMatrixRotationNormal(DirectX::XMLoadFloat3(&f),0.7f));
		//
		//	Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(Pipeline::PipelineManager::Instance().GetDepthCameraView(),
		//																Pipeline::PipelineManager::Instance().GetDepthCameraProj());
		//
		//	for (size_t i = 0; i < this->models.size(); i++)
		//	{
		//		UINT off = 0;
		//		this->deviceContext->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
		//		Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(m, this->models[i].GetWorldInversTranspose());
		//		this->deviceContext->Draw(this->models[i].GetMesh().vertexCount, 0);
		//	}
		//
		//	this->ground.RenderForDepthMap(this->deviceContext);
		//}
	}
}
void Renderer::RenderFinal(float dt)
{
	clock.reset();
	{
		PrintStats(dt);
		Pipeline::PipelineManager::Instance().ApplyFinalPass();
	}

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		finalPassString.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); finalPassString.append("Final pass - FPS: "); finalPassString.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); finalPassString.append(" - ms:"); finalPassString.append(a);
	}
}




