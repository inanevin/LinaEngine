
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
#include "Rendering/ShaderConstants.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Core/Application.hpp"
#include "Rendering/Material.hpp"

using namespace LinaEngine::Graphics;


Material* skyboxMaterial = nullptr;
Material* object1Material = nullptr;
Mesh* cubeMesh = nullptr;

ECSSystemList level1Systems;
FreeLookSystem* ecsFreeLookSystem;
CameraComponent cameraComponent;
TransformComponent cameraTransformComponent;
FreeLookComponent ameraFreeLookComponent;

EntityHandle camera;
EntityHandle object1;
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

void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	m_RenderEngine->CreateMaterial("skyboxMaterial", ShaderConstants::skyboxProceduralShader, &skyboxMaterial);
	skyboxMaterial->SetColor("material.startColor", Colors::LightBlue);
	skyboxMaterial->SetColor("material.endColor", Colors::DarkBlue);
	skyboxMaterial->SetVector3("material.sunDirection", Vector3F(0.0f, -1.0f, 0.0f));
	m_RenderEngine->SetSkyboxMaterial("skyboxMaterial");

	// Set the properties of our the free look component for the camera.
	ameraFreeLookComponent.movementSpeedX = ameraFreeLookComponent.movementSpeedZ = 12.0f;
	ameraFreeLookComponent.rotationSpeedX = ameraFreeLookComponent.rotationSpeedY = 3;

	// Activate a camera component and make a camera entity out of it.
	cameraComponent.isActive = true;
	camera = m_ECS->MakeEntity(cameraComponent, cameraTransformComponent, ameraFreeLookComponent);

	// Load example mesh.
	m_RenderEngine->CreateMesh("cube", "resources/meshes/cube.obj", &cubeMesh);

	// Create material for example mesh.
	m_RenderEngine->CreateMaterial("object1Material", ShaderConstants::standardLitShader, &object1Material);

	object1Renderer.mesh = cubeMesh;
	object1Renderer.material = object1Material;
	object1Transform.transform.SetLocation(Vector3F(0.0f, 0.0f, 10.0f));
	object1 = m_ECS->MakeEntity(object1Transform, object1Renderer);

	// Create the free look system & push it.
	ecsFreeLookSystem = new FreeLookSystem(*m_InputEngine);
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


void Example1Level::Tick(float delta)
{
	// Update the systems in this level.
	m_ECS->UpdateSystems(level1Systems, delta);


}
