
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

#include "Examples/Example1/Levels/Example1Level.hpp"
#include "Rendering/RenderConstants.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/QuadRendererComponent.hpp"
#include <entt/entity/registry.hpp>
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
ECSEntity directionalLight;
ECSEntity quad;


TransformComponent object1Transform;
MeshRendererComponent object1Renderer;
MeshRendererComponent smallCubeRenderer;



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
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_PROCEDURAL);
	mat.SetColor("material.startColor", Color::LightBlue);
	mat.SetColor("material.endColor", Color::DarkBlue);
	mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
	renderEngine->SetSkyboxMaterial(mat);

}

void CreateCubemapSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_CUBEMAP);

	const std::string fp[6] = {
		"resources/textures/defaultSkybox/right.png",
		"resources/textures/defaultSkybox/left.png",
		"resources/textures/defaultSkybox/up.png",
		"resources/textures/defaultSkybox/down.png",
		"resources/textures/defaultSkybox/front.png",
		"resources/textures/defaultSkybox/back.png",
	};

	SamplerData data = SamplerData();
	data.minFilter = FILTER_NEAREST;
	Texture& t = renderEngine->CreateTexture(fp, PixelFormat::FORMAT_RGB, true, false, data);
	mat.SetTexture(MC_DIFFUSETEXTUREPROPERTY, &t, 0, BindTextureMode::BINDTEXTURE_CUBEMAP);
	renderEngine->SetSkyboxMaterial(mat);
}
Material* objectLitMaterial;
Material* objectUnlitMaterial;
Material* objectUnlitMaterial2;
Material* quadMaterial;

Vector3 cubePositions[] = {
	Vector3(0.0f, 0.0f, 10.0f),
	Vector3(5.0f,  0.0f, 10.0f),
	Vector3(-5.0f, 0.0f, -10.0f),
	Vector3(0.0f, 0.0f, -15.0f),
	Vector3(5.0f,  0.0f, 15.0f),
	Vector3(-5.0f, 0.0f, 15.0f),
	Vector3(0.0f, -5.0f, 10.0f),
	Vector3(5.0f,  -5.0f, -10.0f),
	Vector3(-5.0f, -5.0f, 10.0f),
	Vector3(0.0f, -5.0f, 15.0f),
	Vector3(5.0f,  -5.0f, 15.0f),
	Vector3(-5.0f, -5.0f, 15.0f),
};

Vector3 pointLightPositions[]
{
	Vector3(0.0f,  -0.5f,  13.5f),
	Vector3(-5.0f,  -0.5f,  13.5f),
	Vector3(5.0f,  -0.5f,  13.5f),
	Vector3(0.0f,  -7.5f,  13.5f),
};

Vector3 spotLightPositions[]
{
	Vector3(0.0f, 0.0f, 20.0f)
};

int pLightSize = 4;
int cubeSize = 10;
int sLightSize = 1;

ECSEntity cubeEntity;

void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	CreateProceduralSkybox(m_RenderEngine);

	camera.entity = m_ECS->reg.create();

	auto& camFreeLook = m_ECS->reg.emplace<FreeLookComponent>(camera.entity);
	auto& camTransform = m_ECS->reg.emplace<TransformComponent>(camera.entity);
	auto& camCamera = m_ECS->reg.emplace<CameraComponent>(camera.entity);
	camTransform.transform.location = Vector3::Zero;
	// Activate a camera component.
	camCamera.isActive = true;
	quadMaterial = &m_RenderEngine->CreateMaterial("quadMaterial", Shaders::TRANSPARENT_QUAD);
	Texture& crateTexture = m_RenderEngine->CreateTexture("resources/textures/box.png", PixelFormat::FORMAT_RGB, true, false, SamplerData());
	quadMaterial->SetTexture(MC_DIFFUSETEXTUREPROPERTY, &crateTexture, 0);

	// Set the properties of our the free look component for the camera.
/*	camFreeLook.movementSpeedX = camFreeLook.movementSpeedZ = 12.0f;
	camFreeLook.rotationSpeedX = camFreeLook.rotationSpeedY = 3;


	// Load example mesh.
	Mesh& cubeMesh = m_RenderEngine->CreateMesh("resources/meshes/cube.obj");

	// Create material for example mesh.
	objectLitMaterial = &m_RenderEngine->CreateMaterial("object1Material", Shaders::STANDARD_LIT);
	objectUnlitMaterial = &m_RenderEngine->CreateMaterial("object2Material", Shaders::STANDARD_UNLIT);
	objectUnlitMaterial2 = &m_RenderEngine->CreateMaterial("object4Material", Shaders::STANDARD_UNLIT);
	quadMaterial = &m_RenderEngine->CreateMaterial("quadMaterial", Shaders::TRANSPARENT_QUAD);

	// Create texture for example mesh.
	Texture& crateTexture = m_RenderEngine->CreateTexture("resources/textures/box.png", PixelFormat::FORMAT_RGB, true, false, SamplerData());
	Texture& crateSpecTexture = m_RenderEngine->CreateTexture("resources/textures/boxSpecular.png", PixelFormat::FORMAT_RGB, true, false, SamplerData());
	Texture& grassTexture = m_RenderEngine->CreateTexture("resources/textures/grass.png", PixelFormat::FORMAT_RGBA, true, false, SamplerData());
	objectLitMaterial->SetTexture(MC_DIFFUSETEXTUREPROPERTY, &crateTexture, 0);
	objectLitMaterial->SetTexture(MC_SPECULARTEXTUREPROPERTY, &crateSpecTexture, 1);
	objectUnlitMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(0, 0, 1));
	objectUnlitMaterial2->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 0, 0));	
	quadMaterial->SetTexture(MC_DIFFUSETEXTUREPROPERTY, &grassTexture, 0);
	
	object1Renderer.mesh = &cubeMesh;
	object1Renderer.material = objectLitMaterial;
	smallCubeRenderer.mesh = &cubeMesh;
	smallCubeRenderer.material = objectUnlitMaterial;

	directionalLight.entity = m_ECS->reg.create();
	auto& dirLight = m_ECS->reg.emplace<DirectionalLightComponent>(directionalLight.entity);
	dirLight.ambient = Color(0.00f, 0.00f, 0.00f);
	dirLight.specular = Color(0.1f, 0.1f, 0.1f);
	dirLight.diffuse = Color(0.01f, 0.01f, 0.01f);
	dirLight.direction = Vector3(0, 0, 1);

	for (int i = 0; i < cubeSize; i++)
	{
		ECSEntity entity;
		
		//object1Transform.transform.rotation = Quaternion::Euler(Vector3::Zero);
		object1Transform.transform.location = cubePositions[i];
		entity.entity = m_ECS->reg.create();
		m_ECS->reg.emplace<TransformComponent>(entity.entity, object1Transform);
		m_ECS->reg.emplace<MeshRendererComponent>(entity.entity, object1Renderer);
	}

	cubeEntity.entity = m_ECS->reg.create();
	object1Transform.transform.location = Vector3(0, 4, 10);
	m_ECS->reg.emplace<TransformComponent>(cubeEntity.entity, object1Transform);
	m_ECS->reg.emplace<MeshRendererComponent>(cubeEntity.entity, object1Renderer);

	for (int i = 0; i < pLightSize; i++)
	{
		smallCubeRenderer.material = objectUnlitMaterial;

		ECSEntity entity;
		object1Transform.transform.location = pointLightPositions[i];
		object1Transform.transform.scale = 1;
		//object1Transform.transform.rotation = Quaternion::Euler(Vector3::Zero);
		entity.entity = m_ECS->reg.create();
		m_ECS->reg.emplace<TransformComponent>(entity.entity, object1Transform);
		
		auto& pLight1 = m_ECS->reg.emplace<PointLightComponent>(entity.entity);

		pLight1.ambient = Color(0.05f, 0.05f, 0.05f);
		pLight1.diffuse = Color(0.8f, 0.8f, 0.8f);
		pLight1.specular = Color(.4f, .4f, .4f);
		pLight1.constant = 1.0f;
		pLight1.linear = 0.09f;
		pLight1.quadratic = 0.032f;

	
		ECSEntity visuals;
		visuals.entity = m_ECS->reg.create();
		object1Transform.transform.scale = 0.2f;
		m_ECS->reg.emplace<TransformComponent>(visuals.entity, object1Transform);
		m_ECS->reg.emplace<MeshRendererComponent>(visuals.entity, smallCubeRenderer);


	}

	for (int i = 0; i < sLightSize; i++)
	{
		smallCubeRenderer.material = objectUnlitMaterial2;
	//	object1Transform.transform.rotation = (Quaternion::Euler(Vector3::Zero));

		ECSEntity entity;
		object1Transform.transform.location = (spotLightPositions[i]);
		entity.entity = m_ECS->reg.create();
		m_ECS->reg.emplace<TransformComponent>(entity.entity, object1Transform);

		auto& sLight1 = m_ECS->reg.emplace<SpotLightComponent>(entity.entity);

		sLight1.ambient = Color(0.45f, 0.05f, 0.55f);
		sLight1.diffuse = Color(1, 1, 1);
		sLight1.specular = Color(1, 1, 1);
		sLight1.direction = Vector3(0.0f, 0.0f, 1.0f);
		sLight1.constant = 1.0f;
		sLight1.linear = 0.09f;
		sLight1.quadratic = 0.032f;
		sLight1.cutOff = Math::Cos(Math::ToRadians(12.5f));
		sLight1.outerCutOff = Math::Cos(Math::ToRadians(15.5f));


		ECSEntity visuals;
		visuals.entity = m_ECS->reg.create();
		object1Transform.transform.scale = (0.2f);
		m_ECS->reg.emplace<TransformComponent>(visuals.entity, object1Transform);
		m_ECS->reg.emplace<MeshRendererComponent>(visuals.entity, smallCubeRenderer);
	}
	object1Transform.transform.rotation = (Quaternion::Euler(Vector3::Zero));
	*/
	object1Transform.transform.scale = (Vector3(1,1,1));
	object1Transform.transform.location = (Vector3(0,0, -5));
	//object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(0, -180, 0)));
	quad.entity = m_ECS->reg.create();
	m_ECS->reg.emplace<TransformComponent>(quad.entity, object1Transform);
	QuadRendererComponent quadR;
	quadR.material = quadMaterial;
	m_ECS->reg.emplace<QuadRendererComponent>(quad.entity, quadR);

	// Create a cube object.
//object1Renderer.mesh = &cubeMesh;
//object1Renderer.material = objectLitMaterial;
//object1Transform.transform.SetLocation(Vector3F(0.0f, 0.0f, 10.0f));
//object1.entity = m_ECS->reg.create();
//m_ECS->reg.emplace<TransformComponent>(object1.entity, object1Transform);
//m_ECS->reg.emplace<MeshRendererComponent>(object1.entity, object1Renderer);
//
//directionalLight.entity = m_ECS->reg.create();
//auto& dirLight = m_ECS->reg.emplace<DirectionalLightComponent>(directionalLight.entity);
//dirLight.ambient = Color(0.00f, 0.00f, 0.00f);
//dirLight.specular = Color(0.1f, 0.1f, 0.1f);
//dirLight.diffuse = Color(0.01f, 0.01f, 0.01f);
//dirLight.direction = Vector3F(0, 0, 1);
//
//
//object1Transform.transform.SetLocation(Vector3F(0.0f, 0.0f, 5.0f));
//pointLight.entity = m_ECS->reg.create();
//auto& pLight1 = m_ECS->reg.emplace<PointLightComponent>(pointLight.entity);
//m_ECS->reg.emplace<TransformComponent>(pointLight.entity, object1Transform);
//
//pLight1.ambient = Vector3F(0.05f, 0.05f, 0.05f);
//pLight1.diffuse = Vector3F(0.1f, 0.1f, 0.1f);
//pLight1.specular = Vector3F(1, 1, 1);
//pLight1.constant = 1.0f;
//pLight1.linear = 0.09f;
//pLight1.quadratic = 0.032f;
//
//object1Transform.transform.SetLocation(Vector3F(-5.0f, 0.0f, 7.0f));
//spotLight.entity = m_ECS->reg.create();
//auto& sLight1 = m_ECS->reg.emplace<SpotLightComponent>(spotLight.entity);
//m_ECS->reg.emplace<TransformComponent>(spotLight.entity, object1Transform);
//
//sLight1.ambient = Vector3F(0.45f, 0.05f, 0.55f);
//sLight1.diffuse = Vector3F(1, 1, 1);
//sLight1.specular = Vector3F(1, 1, 1);
//sLight1.direction = Vector3F(0.0f, 0.0f, 1.0f);
//sLight1.constant = 1.0f;
//sLight1.linear = 0.09f;
//sLight1.quadratic = 0.032f;
//sLight1.cutOff = Math::Cos(Math::ToRadians(12.5f));
//sLight1.outerCutOff = Math::Cos(Math::ToRadians(15.5f));
//
//
////object1 = m_ECS->MakeEntity(object1Transform, object1Renderer);
//
//object1Transform.transform.SetLocation(Vector3F(5.0f, 0.0f, 10.0f));
//object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));
//
////object2 = m_ECS->MakeEntity(object1Transform, object1Renderer);
//object2.entity = m_ECS->reg.create();
//m_ECS->reg.emplace<TransformComponent>(object2.entity, object1Transform);
//m_ECS->reg.emplace<MeshRendererComponent>(object2.entity, object1Renderer);
//
//object1Transform.transform.SetLocation(Vector3F(-5.0f, 0.0f, 10.0f));
//object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));
//
////object3 = m_ECS->MakeEntity(object1Transform, object1Renderer);
//
//object3.entity = m_ECS->reg.create();
//m_ECS->reg.emplace<TransformComponent>(object3.entity, object1Transform);
//m_ECS->reg.emplace<MeshRendererComponent>(object3.entity, object1Renderer);
//
//object1Transform.transform.SetLocation(Vector3F(-5.0f, 5.0f, 10.0f));
//object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));
//
////object4 = m_ECS->MakeEntity(object1Transform, object1Renderer);
//
//object4.entity = m_ECS->reg.create();
//m_ECS->reg.emplace<TransformComponent>(object4.entity, object1Transform);
//m_ECS->reg.emplace<MeshRendererComponent>(object4.entity, object1Renderer);
//
//object1Transform.transform.SetLocation(Vector3F(5.0f, 5.0f, 10.0f));
//object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));
//
////object5 = m_ECS->MakeEntity(object1Transform, object1Renderer);
//
//
//object1Transform.transform.SetLocation(Vector3F(5.0f, -5.0f, 10.0f));
//object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));
//object1Renderer.material = objectUnlitMaterial;
//
//object5.entity = m_ECS->reg.create();
//m_ECS->reg.emplace<TransformComponent>(object5.entity, object1Transform);
//m_ECS->reg.emplace<MeshRendererComponent>(object5.entity, object1Renderer);


	//object5 = m_ECS->MakeEntity(object1Transform, object1Renderer);


	// Create the free look system & push it.
	ecsFreeLookSystem = new FreeLookSystem();
	ecsFreeLookSystem->Construct(*m_ECS, *m_InputEngine);
	level1Systems.AddSystem(*ecsFreeLookSystem);

	/*
		// Create an example mesh.
		//.mesh = &m_RenderEngine->LoadMeshResource("resources/meshes/cube.obj");

		//material2->diffuseTexture = &m_RenderEngine->LoadTextureResource("box.png", "resources/textures/", PixelFormat::FORMAT_RGB, true, false);
		//material2->specularTexture = &m_RenderEngine->LoadTextureResource("boxSpecular.png", "resources/textures/", PixelFormat::FORMAT_RGB, true, false);

		// Set entity component settings & create an entity out of them.
		//m_ExampleMeshRenderer.material = material1;
		//m_ExampleMeshTransform.transform.SetLocation(Vector3F(-3, 0, 10));
		//m_ExampleMesh = m_ECS->MakeEntity(m_ExampleMeshTransform, m_ExampleMeshRenderer);

		// Set entity component settings & create an entity out of them.
		//m_ExampleMeshRenderer.material = material2;
		//m_ExampleMeshTransform.transform.SetLocation(Vector3F(3, 0, 10));
		//m_ExampleMesh2 = m_ECS->MakeEntity(m_ExampleMeshTransform, m_ExampleMeshRenderer);

		// Set ambient intensity.
		//m_RenderEngine->SetAmbientLightIntensity(0.5f);

		// Create the free look system & push it.
		ecsFreeLookSystem = new FreeLookSystem(*m_InputEngine);
		level1Systems.AddSystem(*ecsFreeLookSystem);*/

}

static float t = 0.0f;

void Example1Level::Tick(float delta)
{
	// Update the systems in this level.
	level1Systems.UpdateSystems(delta);
	t += delta;
	LINA_CLIENT_INFO("{0}", t);
	TransformComponent& cube = m_ECS->reg.get<TransformComponent>(quad.entity);
	cube.transform.location = Vector3(Math::Sin(t) * 1, 0, -5);
	cube.transform.Rotate(Vector3::Forward, t);
//	cube.transform.rotation = q;
	//TransformComponent& tSpotLight = m_ECS->reg.get<TransformComponent>(spotLight.entity);
//	TransformComponent& tCamera = m_ECS->reg.get<TransformComponent>(camera.entity);
	
//tSpotLight.transform.SetLocation(tCamera.transform.location);
//tSpotLight.transform.SetRotation(tCamera.transform.rotation);
//
//TransformComponent tc = m_ECS->reg.get<TransformComponent>(object1.entity);
//t += delta;
//tc.transform.SetLocation(Vector3F(Math::Sin(t) * 5, 0, 10));
//	
//m_ECS->reg.replace<TransformComponent>(object1.entity, tc);
}