
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
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Shader.hpp"
#include "Utility/UtilityFunctions.hpp"

using namespace LinaEngine::Graphics;
using namespace LinaEngine::ECS;

ECSSystemList level1Systems;
FreeLookSystem* ecsFreeLookSystem;
CameraComponent cameraComponent;
TransformComponent cameraTransformComponent;
FreeLookComponent cameraFreeLookComponent;

ECSEntity camera;

Material* objectUnlitMaterial;
Material* helmetMaterial;
Material* sphereMat;
Material* spriteMat;
Material* arcadeMaterial;
Material* floorMaterial;
Material* roadMaterial;


Example1Level::~Example1Level()
{
	delete ecsFreeLookSystem;
}

Texture* albedoSphere;
Texture*  normalSphere;
Texture*  metallicSphere;
Texture*  roughnessSphere;
Texture*  aoSphere;
Texture*  albedoFloor;
Texture*  normalFloor;
Texture*  metallicFloor;
Texture*  roughnessFloor;
Texture*  aoFloor;

bool Example1Level::Install()
{
	LINA_CLIENT_WARN("Example level 1 install.");

	SamplerParameters pbrSampler;
	pbrSampler.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	pbrSampler.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
	pbrSampler.textureParams.wrapS = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.textureParams.wrapT = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.textureParams.pixelFormat = PixelFormat::FORMAT_RGBA;
	pbrSampler.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB;
	pbrSampler.textureParams.generateMipMaps = true;

	LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();

	albedoSphere = &renderEngine.CreateTexture2D("resources/sandbox/textures/rusted_iron/albedo.png", pbrSampler, false, false);
	normalSphere = &renderEngine.CreateTexture2D("resources/sandbox/textures/rusted_iron/normal.png", pbrSampler, false, false);
	metallicSphere = &renderEngine.CreateTexture2D("resources/sandbox/textures/rusted_iron/metallic.png", pbrSampler, false, false);
	roughnessSphere = &renderEngine.CreateTexture2D("resources/sandbox/textures/rusted_iron/roughness.png", pbrSampler, false, false);
	aoSphere = &renderEngine.CreateTexture2D("resources/sandbox/textures/rusted_iron/ao.png", pbrSampler, false, false);
	albedoFloor = &renderEngine.CreateTexture2D("resources/sandbox/textures/grass/albedo.png", pbrSampler, false, false);
	normalFloor = &renderEngine.CreateTexture2D("resources/sandbox/textures/grass/normal.png", pbrSampler, false, false);
	metallicFloor = &renderEngine.CreateTexture2D("resources/sandbox/textures/grass/metallic.png", pbrSampler, false, false);
	roughnessFloor = &renderEngine.CreateTexture2D("resources/sandbox/textures/grass/roughness.png", pbrSampler, false, false);
	aoFloor = &renderEngine.CreateTexture2D("resources/sandbox/textures/grass/ao.png", pbrSampler, false, false);

	return true;
}

void CreateSingleColorSkybox(RenderEngine& renderEngine)
{
	Material& mat = renderEngine.CreateMaterial(-6, Shaders::SKYBOX_SINGLECOLOR);
	mat.SetColor("material.color", Color::Red);
	renderEngine.SetSkyboxMaterial(mat);
}

void CreateGradientSkybox(RenderEngine& renderEngine)
{
	renderEngine.CreateMaterial(-4, Shaders::SKYBOX_GRADIENT);
	renderEngine.GetMaterial(-4).SetColor("material.startColor", Color::Green);
	renderEngine.GetMaterial(-4).SetColor("material.endColor", Color::White);
	renderEngine.SetSkyboxMaterial(renderEngine.GetMaterial(-4));
}

void CreateProceduralSkybox(RenderEngine& renderEngine)
{
	Material& mat = renderEngine.CreateMaterial(-1, Shaders::SKYBOX_PROCEDURAL);
	mat.SetColor("material.startColor", Color::White);
	mat.SetColor("material.endColor", Color(0.2f, 0.2f, 0.2f));
	mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
	renderEngine.SetSkyboxMaterial(mat);
}

void CreateCubemapSkybox(RenderEngine& renderEngine)
{
	Material& mat = renderEngine.CreateMaterial(-2, Shaders::SKYBOX_CUBEMAP);

	const std::string fp[6] = {
		"resources/sandbox/textures/defaultSkybox/right.png",
		"resources/sandbox/textures/defaultSkybox/left.png",
		"resources/sandbox/textures/defaultSkybox/down.png",
		"resources/sandbox/textures/defaultSkybox/up.png",
		"resources/sandbox/textures/defaultSkybox/front.png",
		"resources/sandbox/textures/defaultSkybox/back.png",
	};

	renderEngine.SetSkyboxMaterial(mat);
}
void CreateHDRISkybox(RenderEngine& renderEngine)
{
	Texture* hdri = &renderEngine.CreateTextureHDRI("resources/sandbox/textures/HDRI/canyon3K.hdr");
	renderEngine.CaptureCalculateHDRI(*hdri);
	Material& mat = renderEngine.CreateMaterial(-5, Shaders::SKYBOX_HDRI);
	mat.SetTexture(MAT_MAP_ENVIRONMENT, &renderEngine.GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
	renderEngine.SetSkyboxMaterial(mat);
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

	LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();
	LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();

	// Create, setup & assign skybox material.
	CreateProceduralSkybox(renderEngine);
	objectUnlitMaterial = &renderEngine.CreateMaterial(LinaEngine::Utility::GetUniqueID(), Shaders::STANDARD_UNLIT);

	MeshRendererComponent cr;
	cr.m_meshID = Primitives::CUBE;
	cr.m_materialID = objectUnlitMaterial->m_MaterialID;
	TransformComponent objectTransform;
	RigidbodyComponent r;

	ECSEntity c1;
	c1 = ecs.CreateEntity("Sphere");
	objectTransform.transform.m_location = Vector3(0,0,0);
	ecs.emplace<TransformComponent>(c1, objectTransform);
	ecs.emplace<MeshRendererComponent>(c1, cr);
	ecs.emplace<RigidbodyComponent>(c1, r);


	camera = ecs.CreateEntity("Camera");
	auto& camFreeLook = ecs.emplace<FreeLookComponent>(camera);
	auto& camTransform = ecs.emplace<TransformComponent>(camera);
	auto& camCamera = ecs.emplace<CameraComponent>(camera);
	camTransform.transform.m_location = Vector3(0,5,-5);
	camFreeLook.m_movementSpeeds = Vector2(12, 12);
	camFreeLook.m_rotationSpeeds = Vector2(3, 3);


	// Create the free look system & push it.
	ecsFreeLookSystem = new FreeLookSystem();
	ecsFreeLookSystem->Construct(ecs, LinaEngine::Application::GetInputEngine());
	level1Systems.AddSystem(*ecsFreeLookSystem);

	return;

	Texture& sprite = renderEngine.CreateTexture2D("resources/sandbox/textures/sprite.png");
	Mesh& floorMesh = renderEngine.GetPrimitive(Primitives::PLANE);

	spriteMat = &renderEngine.CreateMaterial(LinaEngine::Utility::GetUniqueID(), Shaders::RENDERER2D_SPRITE);
	spriteMat->SetTexture(MAT_TEXTURE2D_DIFFUSE, &sprite);
	spriteMat->SetSurfaceType(Graphics::MaterialSurfaceType::Transparent);
	sphereMat = &renderEngine.CreateMaterial(LinaEngine::Utility::GetUniqueID(), Shaders::PBR_LIT);

	sphereMat->SetTexture(MAT_TEXTURE2D_ALBEDOMAP, albedoSphere);
	sphereMat->SetTexture(MAT_TEXTURE2D_NORMALMAP, normalSphere);
	sphereMat->SetTexture(MAT_TEXTURE2D_ROUGHNESSMAP, roughnessSphere);
	sphereMat->SetTexture(MAT_TEXTURE2D_METALLICMAP, metallicSphere);
	sphereMat->SetTexture(MAT_TEXTURE2D_AOMAP, aoSphere);
	renderEngine.SetHDRIData(sphereMat);


	floorMaterial = &renderEngine.CreateMaterial(-55, Shaders::PBR_LIT);
	floorMaterial->SetTexture(MAT_TEXTURE2D_ALBEDOMAP, albedoFloor);
	floorMaterial->SetTexture(MAT_TEXTURE2D_NORMALMAP, normalFloor);
	floorMaterial->SetTexture(MAT_TEXTURE2D_ROUGHNESSMAP, roughnessFloor);
	floorMaterial->SetTexture(MAT_TEXTURE2D_METALLICMAP, metallicFloor);
	floorMaterial->SetTexture(MAT_TEXTURE2D_AOMAP, aoFloor);
	floorMaterial->SetVector2(MAT_TILING, Vector2(100, 100));
	renderEngine.SetHDRIData(floorMaterial);

	MeshRendererComponent cubeRenderer;
	cubeRenderer.m_meshID = Primitives::CUBE;
	cubeRenderer.m_materialID = sphereMat->m_MaterialID;

	MeshRendererComponent portalRenderer;
	portalRenderer.m_meshID = Primitives::PLANE;
	portalRenderer.m_materialID = sphereMat->m_MaterialID;

	RigidbodyComponent sphereRB;
	sphereRB.m_mass = 1.0f;
	sphereRB.m_halfExtents = Vector3(1, 1, 1);
	sphereRB.m_collisionShape = CollisionShape::BOX;

	MeshRendererComponent floorRenderer;
	floorRenderer.m_meshID = Primitives::PLANE;
	floorRenderer.m_materialID = floorMaterial->m_MaterialID;

	DirectionalLightComponent dirLightComp;

	ECSEntity directionalLightEntity;
	directionalLightEntity = ecs.CreateEntity("DirLight");
	objectTransform.transform.m_location = Vector3(0, 15, -15);
	objectTransform.transform.Rotate(Vector3(25, 0, 0));
	ecs.emplace<TransformComponent>(directionalLightEntity, objectTransform);
	ecs.emplace<DirectionalLightComponent>(directionalLightEntity, dirLightComp);

	ECSEntity cube1;
	cube1 = ecs.CreateEntity("Sphere");
	objectTransform.transform.m_location = Vector3(-13, 5, 5);
	ecs.emplace<TransformComponent>(cube1, objectTransform);
	ecs.emplace<MeshRendererComponent>(cube1, cubeRenderer);
	//m_ECS->emplace<RigidbodyComponent>(cube1, sphereRB);

	ECSEntity portal;
	portal = ecs.CreateEntity("PortalFrame");
	objectTransform.transform.m_location = Vector3(0, 5, 5);
	objectTransform.transform.Rotate(Vector3(-90, 0, 0));
	objectTransform.transform.m_scale = Vector3::One * 2;
	ecs.emplace<TransformComponent>(portal, objectTransform);
	ecs.emplace<MeshRendererComponent>(portal, portalRenderer);

	//SpriteRendererComponent spriteRenderer;
	//spriteRenderer.materialID = spriteMat->m_MaterialID;
	//ECSEntity sphereEntity2;
	//sphereEntity2 = m_ECS->CreateEntity("Sprite");
	//objectTransform.transform.location = Vector3(-15, 5, 5);
	//cubeRenderer.meshID = Primitives::CUBE;
	//cubeRenderer.materialID = sphereMat->m_MaterialID;
	//m_ECS->emplace<TransformComponent>(sphereEntity2, objectTransform);
	//m_ECS->emplace<MeshRendererComponent>(sphereEntity2, cubeRenderer);
	//m_ECS->emplace<RigidbodyComponent>(sphereEntity2, sphereRB);



	ECSEntity floorEntity;
	floorEntity = ecs.CreateEntity("Floor");
	objectTransform.transform.m_scale = Vector3(100, 1, 100);
	objectTransform.transform.m_location = Vector3(0, 0, 0);
	objectTransform.transform.m_rotation = Vector4::Zero;
	ecs.emplace<TransformComponent>(floorEntity, objectTransform);
	ecs.emplace<MeshRendererComponent>(floorEntity, floorRenderer);



	for (int i = 0; i < pLightSize; i++)
	{

		ECSEntity entity;
		TransformComponent lightTransform;
		//MeshRendererComponent lightRenderer;
		//lightRenderer.mesh = &m_RenderEngine->GetPrimitive(Primitives::SPHERE);
		//lightRenderer.material = objectUnlitMaterial;
		lightTransform.transform.m_location = pointLightPositions[i];
		lightTransform.transform.m_scale = 0.1f;
		entity = ecs.CreateEntity("Point Light " + i);
		auto lightT = ecs.emplace<TransformComponent>(entity, lightTransform);
		auto& pLight1 = ecs.emplace<PointLightComponent>(entity);
		pLight1.m_color = Color(300, 300, 300);
		//	m_ECS->emplace<MeshRendererComponent>(entity, lightRenderer);
		pLight1.m_distance = 100;

	}



	for (int i = 0; i < sLightSize; i++)
	{
		ECSEntity sLight;
		TransformComponent lightTransform;
		MeshRendererComponent lightRenderer;
		SpotLightComponent sLight1;
		lightTransform.transform.m_location = spotLightPositions[i];
		lightRenderer.m_materialID = objectUnlitMaterial->m_MaterialID;
		lightRenderer.m_meshID = Primitives::CUBE;
		sLight = ecs.CreateEntity("Spot light" + i);


		sLight1.m_color = Color(0.05f, 0.05f, 0.05f);
		sLight1.m_distance = 150;
		sLight1.m_cutoff = Math::Cos(Math::ToRadians(12.5f));
		sLight1.m_outerCutoff = Math::Cos(Math::ToRadians(15.5f));
		ecs.emplace<MeshRendererComponent>(sLight, lightRenderer);
		ecs.emplace<TransformComponent>(sLight, lightTransform);
		ecs.emplace<SpotLightComponent>(sLight, sLight1);

	}


}

static float t2 = 0.0f;

void Example1Level::Tick(float delta)
{
	// Update the systems in this level.
	level1Systems.UpdateSystems(delta);
	t2 += delta * 0.75f;
}