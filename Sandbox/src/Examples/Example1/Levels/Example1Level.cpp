
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
#include <entt/entity/registry.hpp>
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"

using namespace LinaEngine::Graphics;

ECSSystemList level1Systems;
FreeLookSystem* ecsFreeLookSystem;
CameraComponent cameraComponent;
TransformComponent cameraTransformComponent;
FreeLookComponent cameraFreeLookComponent;

ECSEntity camera;
ECSEntity object1;
ECSEntity object2;
ECSEntity object3;
ECSEntity object4;
ECSEntity object5;
ECSEntity directionalLight;
ECSEntity pointLight;
ECSEntity spotLight;

TransformComponent object1Transform;
MeshRendererComponent object1Renderer;



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
	mat.SetColor("material.color", Colors::Red);
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateGradientSkybox(RenderEngine* renderEngine)
{
	renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_GRADIENT);
	renderEngine->GetMaterial("skyboxMaterial").SetColor("material.startColor", Colors::Green);
	renderEngine->GetMaterial("skyboxMaterial").SetColor("material.endColor", Colors::White);
	renderEngine->SetSkyboxMaterial(renderEngine->GetMaterial("skyboxMaterial"));
}

void CreateProceduralSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_PROCEDURAL);
	mat.SetColor("material.startColor", Colors::LightBlue);
	mat.SetColor("material.endColor", Colors::DarkBlue);
	mat.SetVector3("material.sunDirection", Vector3F(0.0f, -1.0f, 0.0f));
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
	renderEngine->CreateTexture(fp, PixelFormat::FORMAT_RGB, true, false, data);
	mat.SetTexture(MC_DIFFUSETEXTUREPROPERTY, &renderEngine->GetTexture("skyboxTexture"), 0, BindTextureMode::BINDTEXTURE_CUBEMAP);
	renderEngine->SetSkyboxMaterial(renderEngine->GetMaterial("skyboxMaterial"));
}
Material* objectLitMaterial;
Material* objectUnlitMaterial;

void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	CreateCubemapSkybox(m_RenderEngine);


	camera.entity = m_ECS->reg.create();

	auto& camFreeLook = m_ECS->reg.emplace<FreeLookComponent>(camera.entity);
	auto& camTransform = m_ECS->reg.emplace<TransformComponent>(camera.entity);
	auto& camCamera = m_ECS->reg.emplace<CameraComponent>(camera.entity);

	// Activate a camera component.
	camCamera.isActive = true;

	// Set the properties of our the free look component for the camera.
	camFreeLook.movementSpeedX = camFreeLook.movementSpeedZ = 12.0f;
	camFreeLook.rotationSpeedX = camFreeLook.rotationSpeedY = 3;


	// Load example mesh.
	Mesh& cubeMesh = m_RenderEngine->CreateMesh("resources/meshes/cube.obj");

	// Create material for example mesh.
	objectLitMaterial = &m_RenderEngine->CreateMaterial("object1Material", Shaders::STANDARD_LIT);
	objectUnlitMaterial = &m_RenderEngine->CreateMaterial("object2Material", Shaders::STANDARD_UNLIT);

	// Create texture for example mesh.
	Texture& crateTexture = m_RenderEngine->CreateTexture("resources/textures/box.png", PixelFormat::FORMAT_RGB, true, false, SamplerData());
	Texture& crateSpecTexture = m_RenderEngine->CreateTexture("resources/textures/boxSpecular.png", PixelFormat::FORMAT_RGB, true, false, SamplerData());
	objectLitMaterial->SetTexture(MC_DIFFUSETEXTUREPROPERTY, &crateTexture, 0);
	objectLitMaterial->SetTexture(MC_SPECULARTEXTUREPROPERTY, &crateSpecTexture, 1);

	// Create a cube object.
	object1Renderer.mesh = &cubeMesh;
	object1Renderer.material = objectLitMaterial;
	object1Transform.transform.SetLocation(Vector3F(0.0f, 0.0f, 10.0f));
	object1.entity = m_ECS->reg.create();
	m_ECS->reg.emplace<TransformComponent>(object1.entity, object1Transform);
	m_ECS->reg.emplace<MeshRendererComponent>(object1.entity, object1Renderer);

	directionalLight.entity = m_ECS->reg.create();
	auto& dirLight = m_ECS->reg.emplace<DirectionalLightComponent>(directionalLight.entity);
	dirLight.ambient = Color(0.02f, 0.02f, 0.02f);
	dirLight.specular = Color(0.1f, 0.1f, 0.1f);
	dirLight.diffuse = Color(0.2f, 0.2f, 0.2f);
	dirLight.direction = Vector3F(0, 0, 1);


	object1Transform.transform.SetLocation(Vector3F(0.0f, 0.0f, 5.0f));
	pointLight.entity = m_ECS->reg.create();
	auto& pLight1 = m_ECS->reg.emplace<PointLightComponent>(pointLight.entity);
	m_ECS->reg.emplace<TransformComponent>(pointLight.entity, object1Transform);

	pLight1.ambient = Vector3F(0.05f, 0.05f, 0.05f);
	pLight1.diffuse = Vector3F(0.2f, 0.9f, 0.2f);
	pLight1.specular = Vector3F(1, 1, 1);
	pLight1.constant = 1.0f;
	pLight1.linear = 0.09f;
	pLight1.quadratic = 0.032f;

	object1Transform.transform.SetLocation(Vector3F(-5.0f, 0.0f, 7.0f));
	spotLight.entity = m_ECS->reg.create();
	auto& sLight1 = m_ECS->reg.emplace<SpotLightComponent>(spotLight.entity);
	m_ECS->reg.emplace<TransformComponent>(spotLight.entity, object1Transform);

	sLight1.ambient = Vector3F(1.05f, 1.05f, 1.55f);
	sLight1.diffuse = Vector3F(1, 1, 1);
	sLight1.specular = Vector3F(1, 1, 1);
	sLight1.direction = Vector3F(0.0f, 0.0f, 1.0f);
	sLight1.constant = 1.0f;
	sLight1.linear = 0.09f;
	sLight1.quadratic = 0.032f;
	sLight1.cutOff = Math::Cos(Math::ToRadians(15.5f));
	sLight1.outerCutOff = Math::Cos(Math::ToRadians(45.5f));



	//object1 = m_ECS->MakeEntity(object1Transform, object1Renderer);

	object1Transform.transform.SetLocation(Vector3F(5.0f, 0.0f, 10.0f));
	object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));

	//object2 = m_ECS->MakeEntity(object1Transform, object1Renderer);
	object2.entity = m_ECS->reg.create();
	m_ECS->reg.emplace<TransformComponent>(object2.entity, object1Transform);
	m_ECS->reg.emplace<MeshRendererComponent>(object2.entity, object1Renderer);

	object1Transform.transform.SetLocation(Vector3F(-5.0f, 0.0f, 10.0f));
	object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));

	//object3 = m_ECS->MakeEntity(object1Transform, object1Renderer);

	object3.entity = m_ECS->reg.create();
	m_ECS->reg.emplace<TransformComponent>(object3.entity, object1Transform);
	m_ECS->reg.emplace<MeshRendererComponent>(object3.entity, object1Renderer);

	object1Transform.transform.SetLocation(Vector3F(-5.0f, 5.0f, 10.0f));
	object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));

	//object4 = m_ECS->MakeEntity(object1Transform, object1Renderer);

	object4.entity = m_ECS->reg.create();
	m_ECS->reg.emplace<TransformComponent>(object4.entity, object1Transform);
	m_ECS->reg.emplace<MeshRendererComponent>(object4.entity, object1Renderer);

	object1Transform.transform.SetLocation(Vector3F(5.0f, 5.0f, 10.0f));
	object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));

	//object5 = m_ECS->MakeEntity(object1Transform, object1Renderer);


	object1Transform.transform.SetLocation(Vector3F(5.0f, -5.0f, 10.0f));
	object1Transform.transform.SetRotation(Quaternion::Euler(Vector3F(Math::RandF(-90, 90), Math::RandF(-90, 90), Math::RandF(-90, 90))));
	object1Renderer.material = objectUnlitMaterial;

	object5.entity = m_ECS->reg.create();
	m_ECS->reg.emplace<TransformComponent>(object5.entity, object1Transform);
	m_ECS->reg.emplace<MeshRendererComponent>(object5.entity, object1Renderer);


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
//
//TransformComponent tc = m_ECS->reg.get<TransformComponent>(object1.entity);
//t += delta;
//tc.transform.SetLocation(Vector3F(Math::Sin(t) * 5, 0, 10));
//	
//m_ECS->reg.replace<TransformComponent>(object1.entity, tc);
}