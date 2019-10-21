
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

#include "..\..\..\..\include\Examples\Example1\Levels\Example1Level.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"


ECSSystemList level1Systems;
FreeLookSystem* ecsFreeLookSystem;
EntityHandle m_SceneCamera;
CameraComponent m_SceneCameraComponent;
TransformComponent m_CameraTransformComponent;
FreeLookComponent freeLookComponent;
EntityHandle m_ExampleMesh;
EntityHandle m_ExampleMesh2;
TransformComponent m_ExampleMeshTransform;
MeshRendererComponent m_ExampleMeshRenderer;


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

	// Set the default cubemap skybox.
	m_RenderEngine->ChangeSkyboxRenderType(RenderEngine::SkyboxType::Gradient);
	m_RenderEngine->SetSingleColorSkyboxColor(Colors::Red);
	m_RenderEngine->SetGradientSkyboxColors(Colors::Black, Colors::LightBlue);

	// Disable default scene camera.
	m_RenderEngine->DefaultSceneCameraActivation(false);

	// Set the properties of our the free look component for the camera.
	freeLookComponent.movementSpeedX = freeLookComponent.movementSpeedZ = 12.0f;
	freeLookComponent.rotationSpeedX = freeLookComponent.rotationSpeedY = 3;

	// Activate a camera component and make a camera entity out of it.
	m_SceneCameraComponent.isActive = true;
	m_SceneCamera = m_ECS->MakeEntity(m_SceneCameraComponent, m_CameraTransformComponent, freeLookComponent);

	// Create an example mesh.
	m_ExampleMeshRenderer.vertexArray = m_RenderEngine->LoadModelResource("resources/meshes/linalogo.fbx").GetVertexArray(0);
	
	MeshMaterial* material1 = m_RenderEngine->GetMaterial("material1");
	material1->shaderID = m_RenderEngine->GetShaderID("_standardUnlit");
	material1->texture = m_RenderEngine->GetDefaultDiffuseTexture();
	material1->floats["diffIntensity"] = 15.0f;

	MeshMaterial* material2 = m_RenderEngine->GetMaterial("material2");
	material2->shaderID = m_RenderEngine->GetShaderID("_standardUnlit");
	material2->texture = m_RenderEngine->GetDefaultDiffuseTexture();
	material2->floats["diffIntensity"] = 3.0f;

	m_ExampleMeshRenderer.material = material1;
	m_ExampleMeshTransform.transform.SetLocation(Vector3F(-3, 0, 10));
	m_ExampleMesh = m_ECS->MakeEntity(m_ExampleMeshTransform, m_ExampleMeshRenderer);
	
	m_ExampleMeshRenderer.material = material2;
	m_ExampleMeshTransform.transform.SetLocation(Vector3F(3, 0, 10));
	m_ExampleMesh2 = m_ECS->MakeEntity(m_ExampleMeshTransform, m_ExampleMeshRenderer);

	m_ECS->GetComponent<TransformComponent>(m_ExampleMesh)->transform.GetLocation().Print();

	// Create the free look system & push it.
	ecsFreeLookSystem = new FreeLookSystem(*m_InputEngine);
	level1Systems.AddSystem(*ecsFreeLookSystem);
}


void Example1Level::Tick(float delta)
{
	// Update the systems in this level.
	m_ECS->UpdateSystems(level1Systems, delta);


}
