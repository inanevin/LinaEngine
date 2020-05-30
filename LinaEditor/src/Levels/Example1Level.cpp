
/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Example1Level
Timestamp: 5/6/2019 9:22:56 PM

*/

#include "Levels/Example1Level.hpp"
#include "Rendering/RenderConstants.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Shader.hpp"

using namespace LinaEngine::Graphics;

ECSSystemList level1Systems;
FreeLookSystem* ecsFreeLookSystem;
CameraComponent cameraComponent;
TransformComponent cameraTransformComponent;
FreeLookComponent cameraFreeLookComponent;

ECSEntity camera;

Material* objectUnlitMaterial;
Material* gunMaterial;
Material* sphereMat;
Material* arcadeMaterial;



Example1Level::~Example1Level()
{
	delete ecsFreeLookSystem;
}

void Example1Level::Install()
{
	LINA_CLIENT_WARN("Example level 1 install.");
}

void CreateSingleColorSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_SINGLECOLOR);
	mat.SetColor("material.color", Color::Red);
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateGradientSkybox(RenderEngine* renderEngine)
{
	renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_GRADIENT);
	renderEngine->GetMaterial("skyboxMaterial").SetColor("material.startColor", Color::Green);
	renderEngine->GetMaterial("skyboxMaterial").SetColor("material.endColor", Color::White);
	renderEngine->SetSkyboxMaterial(renderEngine->GetMaterial("skyboxMaterial"));
}

void CreateProceduralSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial("skyboxMaterialP", Shaders::SKYBOX_PROCEDURAL);
	mat.SetColor("material.startColor", Color::Black);
	mat.SetColor("material.endColor", Color::Black);
	mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateCubemapSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_CUBEMAP);

	const std::string fp[6] = {
		"resources/textures/defaultSkybox/right.png",
		"resources/textures/defaultSkybox/left.png",
		"resources/textures/defaultSkybox/down.png",
		"resources/textures/defaultSkybox/up.png",
		"resources/textures/defaultSkybox/front.png",
		"resources/textures/defaultSkybox/back.png",
	};


	SamplerData data = SamplerData();
	SamplerParameters samplerParams;
	samplerParams.textureParams.generateMipMaps = true;
	samplerParams.textureParams.minFilter = FILTER_NEAREST;
	Texture& t = renderEngine->CreateTextureCubemap(fp, samplerParams, false);
	mat.SetTexture(MC_TEXTURE2D_DIFFUSE, &t, TextureBindMode::BINDTEXTURE_CUBEMAP);
	renderEngine->SetSkyboxMaterial(mat);
}
Texture* hdri;
void CreateHDRISkybox(RenderEngine* renderEngine)
{
	hdri = &renderEngine->CreateTextureHDRI("resources/textures/HDRI/canyon3K.hdr");
	renderEngine->CaptureCalculateHDRI(*hdri);
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_HDRI);
	mat.SetTexture(UF_MAP_ENVIRONMENT, &renderEngine->GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
	renderEngine->SetSkyboxMaterial(mat);
}

Vector3 cubePositions[] = {
	Vector3(0.0f, 1, -4.0),
	Vector3(0.0f, 1.0f, 4.0),
	Vector3(-4.0f, 1.0f, 0.0),
	Vector3(4.0f, 1.5f, 0.0f),
	Vector3(0.0f, -9.0f, 5.0f),
	Vector3(0.0f, -9.0f, 15.0f),
	Vector3(-5.0f, -4.0f, 10.0f),
	Vector3(0.0f, -4.0f, 5.0f),
	Vector3(5.0f, -4.0f, 15.0f),
};


Vector3 pointLightPositions[]
{
	glm::vec3(-10.0f,  10.0f, 10.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
};

Vector3 spotLightPositions[]
{
	Vector3(0,0, -4)
};

int pLightSize = 4;
int cubeSize = 4;
int sLightSize = 0;



Texture* bricksParallax;
Texture* bricksParallax2;
ECSEntity cubeEntity;
TransformComponent* dirLightT;
void Example1Level::Initialize()
{


	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	CreateHDRISkybox(m_RenderEngine);

	camera = m_ECS->CreateEntity("Camera");
	auto& camFreeLook = m_ECS->emplace<FreeLookComponent>(camera);
	auto& camTransform = m_ECS->emplace<TransformComponent>(camera);
	auto& camCamera = m_ECS->emplace<CameraComponent>(camera);
	camTransform.transform.location = Vector3(0, 5, 0);
	camCamera.isActive = true;
	camFreeLook.movementSpeedX = camFreeLook.movementSpeedZ = 12.0f;
	camFreeLook.rotationSpeedX = camFreeLook.rotationSpeedY = 3;


	SamplerParameters pbrSampler;
	pbrSampler.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	pbrSampler.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
	pbrSampler.textureParams.wrapS = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.textureParams.wrapT = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.textureParams.pixelFormat = PixelFormat::FORMAT_RGBA;
	pbrSampler.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB;
	pbrSampler.textureParams.generateMipMaps = true;



	Texture& albedoSphere = m_RenderEngine->CreateTexture2D("resources/textures/gold/albedo.png", pbrSampler, false, false);
	Texture& normalSphere = m_RenderEngine->CreateTexture2D("resources/textures/gold/normal.png", pbrSampler, false, false);
	Texture& metallicSphere = m_RenderEngine->CreateTexture2D("resources/textures/gold/metallic.png", pbrSampler, false, false);
	Texture& roughnessSphere = m_RenderEngine->CreateTexture2D("resources/textures/gold/roughness.png", pbrSampler, false, false);
	Texture& aoSphere = m_RenderEngine->CreateTexture2D("resources/textures/gold/ao.png", pbrSampler, false, false);


	// Load example mesh.
	Mesh& cubeMesh = m_RenderEngine->GetPrimitive(Primitives::SPHERE);

	// Create material for example mesh.
	objectUnlitMaterial = &m_RenderEngine->CreateMaterial("object2Material", Shaders::STANDARD_UNLIT);


	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 3.0f;

	sphereMat = &m_RenderEngine->CreateMaterial("sp", Shaders::PBR_LIT);
	sphereMat->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoSphere);
	sphereMat->SetTexture(MC_TEXTURE2D_NORMALMAP, &normalSphere);
	sphereMat->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessSphere);
	sphereMat->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicSphere);
	sphereMat->SetTexture(MC_TEXTURE2D_AOMAP, &aoSphere);
	//sphereMat.SetFloat(MC_ROUGHNESSMULTIPLIER, glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
	//sphereMat.SetFloat(MC_METALLICMULTIPLIER, metallic);
	sphereMat->SetTexture(MC_TEXTURE2D_IRRADIANCEMAP, &m_RenderEngine->GetIrradianceMap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
	sphereMat->SetTexture(MC_TEXTURE2D_BRDFLUTMAP, &m_RenderEngine->GetBRDFMap(), TextureBindMode::BINDTEXTURE_TEXTURE2D);
	sphereMat->SetTexture(MC_TEXTURE2D_PREFILTERMAP, &m_RenderEngine->GetPrefilterMap(), TextureBindMode::BINDTEXTURE_CUBEMAP);


	for (int row = 0; row < nrRows; ++row)
	{
		float metallic = (float)row / (float)(nrRows);

		for (int col = 0; col < nrColumns; ++col)
		{

			ECSEntity entity;
			MeshRendererComponent mr;
			TransformComponent sphereTransform;
			mr.mesh = &m_RenderEngine->GetPrimitive(Primitives::SPHERE);
			mr.material = sphereMat;
			entity = m_ECS->CreateEntity("Cube " + std::to_string(row + col));

			sphereTransform.transform.location = glm::vec3(
				(float)(col - (nrColumns / 2)) * spacing,
				(float)(row - (nrRows / 2)) * spacing,
				-2.0f
			);
			m_ECS->emplace<TransformComponent>(entity, sphereTransform);
			m_ECS->emplace<MeshRendererComponent>(entity, mr);
		}
	}



	for (int i = 0; i < pLightSize; i++)
	{

		ECSEntity entity;
		TransformComponent lightTransform;
		MeshRendererComponent lightRenderer;
		lightRenderer.mesh = &m_RenderEngine->GetPrimitive(Primitives::SPHERE);
		lightRenderer.material = objectUnlitMaterial;
		lightTransform.transform.location = pointLightPositions[i];
		lightTransform.transform.scale = 0.1f;
		entity = m_ECS->CreateEntity("Point Light " + i);
		auto lightT = m_ECS->emplace<TransformComponent>(entity, lightTransform);
		auto& pLight1 = m_ECS->emplace<PointLightComponent>(entity);
		pLight1.color = Color(300, 300, 300);
		m_ECS->emplace<MeshRendererComponent>(entity, lightRenderer);
		pLight1.distance = 100;

	}



	for (int i = 0; i < sLightSize; i++)
	{
		ECSEntity entity;
		TransformComponent lightTransform;
		MeshRendererComponent lightRenderer;
		lightRenderer.mesh = &m_RenderEngine->GetPrimitive(Primitives::SPHERE);
		lightRenderer.material = objectUnlitMaterial;
		lightTransform.transform.location = pointLightPositions[i];
		lightTransform.transform.scale = 0.1f;
		entity = m_ECS->CreateEntity("Point Light " + i);
		auto lightT = m_ECS->emplace<TransformComponent>(entity, lightTransform);
		auto& sLight1 = m_ECS->emplace<SpotLightComponent>(entity);
		sLight1.color = Color(300, 300, 300);
		m_ECS->emplace<MeshRendererComponent>(entity, lightRenderer);
		sLight1.distance = 100;



		sLight1.color = Color(0.05f, 0.05f, 0.05f);
		sLight1.distance = 150;
		sLight1.cutOff = Math::Cos(Math::ToRadians(12.5f));
		sLight1.outerCutOff = Math::Cos(Math::ToRadians(15.5f));


	}

	// Create the free look system & push it.
	ecsFreeLookSystem = new FreeLookSystem();
	ecsFreeLookSystem->Construct(*m_ECS, *m_InputEngine);
	level1Systems.AddSystem(*ecsFreeLookSystem);


}

static float t2 = 0.0f;

void Example1Level::Tick(float delta)
{
	// Update the systems in this level.
	level1Systems.UpdateSystems(delta);
	t2 += delta;



	//t.transform.location = Vector3(0, 0, Math::Sin(t2) * 5);
}