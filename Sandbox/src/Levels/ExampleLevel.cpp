
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

#include "Levels/ExampleLevel.hpp"
#include "Core/Application.hpp"
#include "ECS/Components/AnimationComponent.hpp"
#include "Rendering/Model.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Audio/Audio.hpp"
#include "Physics/PhysicsMaterial.hpp"

using namespace Lina::Graphics;
using namespace Lina::ECS;
using namespace Lina;


bool ExampleLevel::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
{
	Level::Install(loadFromFile, path, levelName);
	return true;
}

void ExampleLevel::Initialize()
{
	LINA_TRACE("ExampleLevel initialized. Implement your logic for instantiating entities, players, assigning cameras etc. from now on.");
	Level::Initialize();

	// auto& reg = Application::GetECSRegistry();
	// auto entity = reg.CreateEntity("Test");
	// auto& mr = reg.emplace<ECS::ModelRendererComponent>(entity);
	// 
	// auto& model = Graphics::Model::CreateModel("Resources/SandboxMeshes/MarkerMan.fbx");
	// mr.SetModel(reg, entity, model);
	// 
	// auto& mat = Graphics::Material::LoadMaterialFromFile("Resources/SandboxNewMaterial207.mat");
	// mr.SetMaterial(reg, entity, 1, mat);
	//auto& anim = Application::GetECSRegistry().emplace<ECS::AnimationComponent>(ybot);
	//anim.m_animationName = "Armature_Move";

	// Texture* hdri = &Texture::GetTexture("Resources/SandboxHDRI/studio.hdr");
	// Application::GetRenderEngine().CaptureCalculateHDRI(*hdri);
	// Material& mat = Lina::Graphics::Material::CreateMaterial(Graphics::Shader::GetShader("Resources/Engine/Shaders/Skybox/SkyboxHDRI.glsl"));
	// mat.SetTexture(MAT_MAP_ENVIRONMENT, &Application::GetRenderEngine().GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
	// Application::GetRenderEngine().SetSkyboxMaterial(&mat);
	// Application::GetRenderEngine().GetCameraSystem()->SetActiveCamera(Application::GetECSRegistry().GetEntity("Entity"));
	//Graphics::RenderEngineBackend::Get()->GetCameraSystem()->SetActiveCamera(ECS::Registry::Get()->GetEntity("Entity"));
	//auto entity = ECS::Registry::Get()->GetEntity("Entity");
	//ModelRendererComponent& mr = ECS::Registry::Get()->get<ModelRendererComponent>(entity);
	//mr.SetModel(entity, Graphics::Model::GetModel("Resources/SandboxTarget/RicochetTarget.fbx"));

}

void ExampleLevel::Tick(Event::ETick ev)
{
	auto entity = ECS::Registry::Get()->GetEntity("Capsule.fbx");
	if (entity != ECSNULL)
	{
		auto& data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(entity);
		data.AddRotation(Vector3(65 * ev.m_deltaTime, 0, 0));
		Vector3 location = data.GetLocation();
		location.x = Math::Sin(Engine::Get()->GetElapsedTime() * 1.8f) * 1.5f;
		data.SetLocation(location);
	}

}

