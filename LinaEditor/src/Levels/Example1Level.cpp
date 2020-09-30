
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
#include "ECS/Components/RigidbodyComponent.hpp"
#include "Levels/GroundCubeSystem.hpp"
#include "Levels/GroundCubeComponent.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Shader.hpp"

using namespace LinaEngine::Graphics;
using namespace LinaEngine::ECS;

ECSSystemList level1Systems;
FreeLookSystem* ecsFreeLookSystem;
GroundCubeSystem* groundCubeSystem;
CameraComponent cameraComponent;
TransformComponent cameraTransformComponent;
FreeLookComponent cameraFreeLookComponent;

ECSEntity camera;

Material* objectUnlitMaterial;
Material* helmetMaterial;
Material* sphereMat;
Material* arcadeMaterial;
Material* floorMaterial;
Material* roadMaterial;


Example1Level::~Example1Level()
{
	delete ecsFreeLookSystem;
	delete groundCubeSystem;
}

void Example1Level::Install()
{
	LINA_CLIENT_WARN("Example level 1 install.");
}

void CreateSingleColorSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial(-6, Shaders::SKYBOX_SINGLECOLOR);
	mat.SetColor("material.color", Color::Red);
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateGradientSkybox(RenderEngine* renderEngine)
{
	renderEngine->CreateMaterial(-4, Shaders::SKYBOX_GRADIENT);
	renderEngine->GetMaterial(-4).SetColor("material.startColor", Color::Green);
	renderEngine->GetMaterial(-4).SetColor("material.endColor", Color::White);
	renderEngine->SetSkyboxMaterial(renderEngine->GetMaterial(-4));
}

void CreateProceduralSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial(-1, Shaders::SKYBOX_PROCEDURAL);
	mat.SetColor("material.startColor", Color::White);
	mat.SetColor("material.endColor", Color(0.2f, 0.2f, 0.2f));
	mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateCubemapSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial(-2, Shaders::SKYBOX_CUBEMAP);

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
	//Texture& t = renderEngine->CreateTextureCubemap(fp, samplerParams, false);
	//mat.SetTexture(MAT_MAP_ENVIRONMENT, &t, TextureBindMode::BINDTEXTURE_CUBEMAP);
	renderEngine->SetSkyboxMaterial(mat);
}
void CreateHDRISkybox(RenderEngine* renderEngine)
{
	Texture* hdri = &renderEngine->CreateTextureHDRI(0, "resources/textures/HDRI/canyon3K.hdr");
	renderEngine->CaptureCalculateHDRI(*hdri);
	Material& mat = renderEngine->CreateMaterial(-5, Shaders::SKYBOX_HDRI);
	mat.SetTexture(MAT_MAP_ENVIRONMENT, &renderEngine->GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
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
	glm::vec3(0.0f, 8.0f, 0.0f),
	glm::vec3(10.0f,  10.0f, 10.0f),
	glm::vec3(-10.0f, -10.0f, 10.0f),
	glm::vec3(10.0f, -10.0f, 10.0f),
};

Vector3 spotLightPositions[]
{
	glm::vec3(0.0f, 4.0f, 0.0f)
};

int pLightSize = 0;
int cubeSize = 4;
int sLightSize = 0;


void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	CreateProceduralSkybox(m_RenderEngine);





	camera = m_ECS->CreateEntity("Camera");
	auto& camFreeLook = m_ECS->emplace<FreeLookComponent>(camera);
	auto& camTransform = m_ECS->emplace<TransformComponent>(camera);
	auto& camCamera = m_ECS->emplace<CameraComponent>(camera);
	camTransform.transform.location = Vector3(-25, 1.5f, -15);
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



Texture& albedoSphere = m_RenderEngine->CreateTexture2D(-5, "resources/textures/rusted_iron/albedo.png", pbrSampler, false, false);
Texture& normalSphere = m_RenderEngine->CreateTexture2D(1, "resources/textures/rusted_iron/normal.png", pbrSampler, false, false);
Texture& metallicSphere = m_RenderEngine->CreateTexture2D(2, "resources/textures/rusted_iron/metallic.png", pbrSampler, false, false);
Texture& roughnessSphere = m_RenderEngine->CreateTexture2D(3, "resources/textures/rusted_iron/roughness.png", pbrSampler, false, false);
Texture& aoSphere = m_RenderEngine->CreateTexture2D(4, "resources/textures/rusted_iron/ao.png", pbrSampler, false, false);
	//
	//Texture& albedoFloor = m_RenderEngine->CreateTexture2D("resources/textures/wall/albedo.png", pbrSampler, false, false);
	//Texture& normalFloor = m_RenderEngine->CreateTexture2D("resources/textures/wall/normal.png", pbrSampler, false, false);
	//Texture& metallicFloor = m_RenderEngine->CreateTexture2D("resources/textures/wall/metallic.png", pbrSampler, false, false);
	//Texture& roughnessFloor = m_RenderEngine->CreateTexture2D("resources/textures/wall/roughness.png", pbrSampler, false, false);
	//Texture& aoFloor = m_RenderEngine->CreateTexture2D("resources/textures/wall/ao.png", pbrSampler, false, false);
	//
	//
	//
	//Texture& albedoHelmet = m_RenderEngine->CreateTexture2D("resources/textures/helmet/albedo.jpg", pbrSampler, false, false);
	//Texture& normalHelmet = m_RenderEngine->CreateTexture2D("resources/textures/helmet/normal.jpg", pbrSampler, false, false);
	//Texture& metallicHelmet = m_RenderEngine->CreateTexture2D("resources/textures/helmet/metallic.jpg", pbrSampler, false, false);
	//Texture& roughnessHelmet = m_RenderEngine->CreateTexture2D("resources/textures/helmet/roughness.jpg", pbrSampler, false, false);
	//Texture& aoHelmet = m_RenderEngine->CreateTexture2D("resources/textures/helmet/ao.jpg", pbrSampler, false, false);

//Texture& albedoRoad = m_RenderEngine->CreateTexture2D("resources/textures/road/albedo.png", pbrSampler, false, false);
//Texture& normalRoad = m_RenderEngine->CreateTexture2D("resources/textures/road/normal.png", pbrSampler, false, false);
//Texture& roughnessRoad = m_RenderEngine->CreateTexture2D("resources/textures/road/roughness.png", pbrSampler, false, false);
//Texture& aoRoad = m_RenderEngine->CreateTexture2D("resources/textures/road/ao.png", pbrSampler, false, false);


	
	// Load example mesh.
//	Mesh& cubeMesh = m_RenderEngine->GetPrimitive(Primitives::CUBE);
//	Mesh& floorMesh = m_RenderEngine->GetPrimitive(Primitives::PLANE);
//	Mesh& helmetMesh = m_RenderEngine->CreateMesh("resources/meshes/glock.fbx");
//	Mesh& roadMesh = m_RenderEngine->GetPrimitive(Primitives::PLANE);
	// Create material for example mesh.
	objectUnlitMaterial = &m_RenderEngine->CreateMaterial(LinaEngine::Utility::GetUniqueID(), Shaders::STANDARD_UNLIT);


	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 3.0f;


	sphereMat = &m_RenderEngine->CreateMaterial(LinaEngine::Utility::GetUniqueID(), Shaders::PBR_LIT);
sphereMat->SetTexture(MAT_TEXTURE2D_ALBEDOMAP, &albedoSphere);
sphereMat->SetTexture(MAT_TEXTURE2D_NORMALMAP, &normalSphere);
sphereMat->SetTexture(MAT_TEXTURE2D_ROUGHNESSMAP, &roughnessSphere);
sphereMat->SetTexture(MAT_TEXTURE2D_METALLICMAP, &metallicSphere);
sphereMat->SetTexture(MAT_TEXTURE2D_AOMAP, &aoSphere);
	m_RenderEngine->SetHDRIData(sphereMat);

//roadMaterial = &m_RenderEngine->CreateMaterial("road", Shaders::PBR_LIT);
//roadMaterial->SetTexture(MAT_TEXTURE2D_ALBEDOMAP, &albedoRoad);
//roadMaterial->SetTexture(MAT_TEXTURE2D_NORMALMAP, &normalRoad);
//roadMaterial->SetTexture(MAT_TEXTURE2D_ROUGHNESSMAP, &roughnessRoad);
//roadMaterial->SetTexture(MAT_TEXTURE2D_AOMAP, &aoRoad);
//roadMaterial->SetVector2(MAT_TILING, Vector2(5.0f, 2.0f));
	//_RenderEngine->SetHDRIData(roadMaterial);

	//helmetMaterial = &m_RenderEngine->CreateMaterial("hp", Shaders::PBR_LIT);
	//helmetMaterial->SetTexture(MAT_TEXTURE2D_ALBEDOMAP, &albedoHelmet);
	//helmetMaterial->SetTexture(MAT_TEXTURE2D_NORMALMAP, &normalHelmet);
	//helmetMaterial->SetTexture(MAT_TEXTURE2D_ROUGHNESSMAP, &roughnessHelmet);
	//helmetMaterial->SetTexture(MAT_TEXTURE2D_METALLICMAP, &metallicHelmet);
	//helmetMaterial->SetTexture(MAT_TEXTURE2D_AOMAP, &aoHelmet);
	//
	//floorMaterial = &m_RenderEngine->CreateMaterial("fs", Shaders::PBR_LIT);
	//floorMaterial->SetTexture(MAT_TEXTURE2D_ALBEDOMAP, &albedoFloor);
	//floorMaterial->SetTexture(MAT_TEXTURE2D_NORMALMAP, &normalFloor);
	//floorMaterial->SetTexture(MAT_TEXTURE2D_ROUGHNESSMAP, &roughnessFloor);
	//floorMaterial->SetTexture(MAT_TEXTURE2D_METALLICMAP, &metallicFloor);
	//floorMaterial->SetTexture(MAT_TEXTURE2D_AOMAP, &aoFloor);
	//floorMaterial->SetVector2(MAT_TILING, Vector2(100, 100));


	MeshRendererComponent sphereMR;
	sphereMR.meshID = Primitives::CUBE;
	sphereMR.materialID = sphereMat->m_MaterialID;

	RigidbodyComponent sphereRB;
	sphereRB.m_mass = 1.0f;
	sphereRB.m_halfExtents = Vector3(1.0f);

	//
	//MeshRendererComponent helmetMR;
	//helmetMR.mesh = &helmetMesh;
	//helmetMR.material = helmetMaterial;
	//
	//MeshRendererComponent floorMR;
	//floorMR.mesh = &floorMesh;
	//floorMR.material = floorMaterial;

	DirectionalLightComponent dirLightComp;
	dirLightComp.direction = Vector3(0, 0, 1);
	TransformComponent objectTransform;

	ECSEntity directionalLightEntity;
	directionalLightEntity = m_ECS->CreateEntity("DirLight");
	m_ECS->emplace<TransformComponent>(directionalLightEntity, objectTransform);
	m_ECS->emplace<DirectionalLightComponent>(directionalLightEntity, dirLightComp);


ECSEntity sphereEntity;
sphereEntity = m_ECS->CreateEntity("Sphere");
objectTransform.transform.location = Vector3(0, 5, 5);
m_ECS->emplace<TransformComponent>(sphereEntity, objectTransform);
m_ECS->emplace<MeshRendererComponent>(sphereEntity, sphereMR);
m_ECS->emplace<RigidbodyComponent>(sphereEntity, sphereRB);


	//
	//ECSEntity helmetEntity;
	//helmetEntity = m_ECS->CreateEntity("Helmet");
	//objectTransform.transform.location = Vector3(0, 5, -5);
	//m_ECS->emplace<TransformComponent>(helmetEntity, objectTransform);
	//m_ECS->emplace<MeshRendererComponent>(helmetEntity, helmetMR);
	//
	//ECSEntity floorEntity;
	//floorEntity = m_ECS->CreateEntity("Floor");
	//objectTransform.transform.scale = Vector3(100, 1, 100);
	//objectTransform.transform.location = Vector3(0, 0, 0);
	//m_ECS->emplace<TransformComponent>(floorEntity, objectTransform);
	//m_ECS->emplace<MeshRendererComponent>(floorEntity, floorMR);

	//MeshRendererComponent roadMR;
	//roadMR.mesh = &roadMesh;
	//roadMR.material = roadMaterial;
	//
	//ECSEntity roadEntity;
	//roadEntity = m_ECS->CreateEntity("Floor");
	//objectTransform.transform.scale = Vector3(5, 1, 50);
	//objectTransform.transform.location = Vector3(0, 0, 0);
	//m_ECS->emplace<TransformComponent>(roadEntity, objectTransform);
	//m_ECS->emplace<MeshRendererComponent>(roadEntity, roadMR);

	for (int i = 0; i < pLightSize; i++)
	{

		ECSEntity entity;
		TransformComponent lightTransform;
		//MeshRendererComponent lightRenderer;
		//lightRenderer.mesh = &m_RenderEngine->GetPrimitive(Primitives::SPHERE);
		//lightRenderer.material = objectUnlitMaterial;
		lightTransform.transform.location = pointLightPositions[i];
		lightTransform.transform.scale = 0.1f;
		entity = m_ECS->CreateEntity("Point Light " + i);
		auto lightT = m_ECS->emplace<TransformComponent>(entity, lightTransform);
		auto& pLight1 = m_ECS->emplace<PointLightComponent>(entity);
		pLight1.color = Color(300, 300, 300);
		//	m_ECS->emplace<MeshRendererComponent>(entity, lightRenderer);
		pLight1.distance = 100;

	}



	for (int i = 0; i < sLightSize; i++)
	{
		ECSEntity sLight;
		TransformComponent lightTransform;
		MeshRendererComponent lightRenderer;
		SpotLightComponent sLight1;
		lightTransform.transform.location = spotLightPositions[i];
		lightRenderer.materialID = objectUnlitMaterial->m_MaterialID;
		lightRenderer.meshID = Primitives::CUBE;
		sLight = m_ECS->CreateEntity("Spot light" + i);


		sLight1.color = Color(0.05f, 0.05f, 0.05f);
		sLight1.distance = 150;
		sLight1.cutOff = Math::Cos(Math::ToRadians(12.5f));
		sLight1.outerCutOff = Math::Cos(Math::ToRadians(15.5f));
		m_ECS->emplace<MeshRendererComponent>(sLight, lightRenderer);
		m_ECS->emplace<TransformComponent>(sLight, lightTransform);
		m_ECS->emplace<SpotLightComponent>(sLight, sLight1);

	}

	//int rows = 100;
	//int cols = 100;
	//float posInc = 25.0f;
	//TransformComponent cubeTransform;
	//MeshRendererComponent cubeRenderer;
	//GroundCubeComponent gc;
	//Vector3 initialPos = Vector3(0, -50, 0);
	//cubeTransform.transform.location = initialPos;
	//cubeTransform.transform.scale = 1.0f;
	//gc.initialPos = initialPos;
	//
	//cubeRenderer.material = floorMaterial;
	//cubeRenderer.mesh = &cubeMesh;

	//float delay = 0.0f;
	//for (int i = 0; i < rows; i+=2)
	//{
	//	for (int j = 0; j < cols; j+=2)
	//	{
	//		delay += 0.1f;
	//		gc.delay = delay;
	//		gc.targetPos = Vector3(j - posInc, 0.0f, i - posInc);
	//		ECSEntity cube = m_ECS->CreateEntity("Cube");
	//		m_ECS->emplace<TransformComponent>(cube, cubeTransform);
	//		m_ECS->emplace<MeshRendererComponent>(cube, cubeRenderer);
	//		m_ECS->emplace<GroundCubeComponent>(cube, gc);
	//	}
	//}
	//
	groundCubeSystem = new GroundCubeSystem();
	groundCubeSystem->Construct(*m_ECS);



	// Create the free look system & push it.
	ecsFreeLookSystem = new FreeLookSystem();
	ecsFreeLookSystem->Construct(*m_ECS, *m_InputEngine);
	level1Systems.AddSystem(*ecsFreeLookSystem);
	level1Systems.AddSystem(*groundCubeSystem);

}

static float t2 = 0.0f;

void Example1Level::Tick(float delta)
{
	// Update the systems in this level.
	level1Systems.UpdateSystems(delta);
	t2 += delta * 0.75f;
}