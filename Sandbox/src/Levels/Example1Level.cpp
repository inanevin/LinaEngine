
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
#include "Input/InputEngine.hpp"

using namespace LinaEngine::Graphics;
using namespace LinaEngine::ECS;


Example1Level::~Example1Level()
{

}

bool Example1Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
{
	LINA_CLIENT_WARN("Example level 1 install.");

	SamplerParameters pbrSampler;
	pbrSampler.m_textureParams.m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	pbrSampler.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
	pbrSampler.m_textureParams.m_wrapS = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGBA;
	pbrSampler.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB;
	pbrSampler.m_textureParams.m_generateMipMaps = true;

	if (LinaEngine::Utility::FileExists("resources/sandbox/levels/Example1Level.linaleveldata"))
	{
		DeserializeLevelData("resources/sandbox/levels/", "Example1Level");
	}

	return true;
}

void CreateSingleColorSkybox(RenderEngine& renderEngine)
{
	Material& mat = renderEngine.CreateMaterial(Shaders::Skybox_SingleColor);
	mat.SetColor("material.color", Color::Red);
	renderEngine.SetSkyboxMaterial(mat);
}

void CreateGradientSkybox(RenderEngine& renderEngine)
{
	Material& mat = renderEngine.CreateMaterial(Shaders::Skybox_Gradient);
	mat.SetColor("material.startColor", Color::Green);
	mat.SetColor("material.endColor", Color::White);
	renderEngine.SetSkyboxMaterial(mat);
}

void CreateProceduralSkybox(RenderEngine& renderEngine)
{
	Material& mat = renderEngine.CreateMaterial(Shaders::Skybox_Procedural);
	mat.SetColor("material.startColor", Color::White);
	mat.SetColor("material.endColor", Color(0.2f, 0.2f, 0.2f));
	mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
	renderEngine.SetSkyboxMaterial(mat);
}

void CreateHDRISkybox(RenderEngine& renderEngine)
{
	Texture* hdri = &renderEngine.CreateTextureHDRI("resources/sandbox/textures/HDRI/canyon3K.hdr");
	renderEngine.CaptureCalculateHDRI(*hdri);
	Material& mat = renderEngine.CreateMaterial(Shaders::Skybox_HDRI);
	mat.SetTexture(MAT_MAP_ENVIRONMENT, &renderEngine.GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
	renderEngine.SetSkyboxMaterial(mat);
}




ECSEntity cube1;
ECSEntity cube2;

void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create a simple procedural skybox.
	LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();
	CreateProceduralSkybox(renderEngine);


	LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();

	cube1 = ecs.GetEntity("Cube1");
	cube2 = ecs.GetEntity("Cube2");
	//ecs.AddChildTo(cube1, cube2);

	TransformComponent& tr1 = ecs.get<TransformComponent>(cube1);
	TransformComponent& tr2 = ecs.get<TransformComponent>(cube2);
	//
	tr1.transform.AddChild(&tr2.transform);

	tr1.transform.SetGlobalLocation(Vector3(-5, 0, 0));
	tr2.transform.SetLocalLocation(Vector3(5, 0, 0));
	tr1.transform.SetGlobalLocation(Vector3(-5, 15, 0));
}

void Example1Level::Tick(float delta)
{
	LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();

	if (LinaEngine::Application::GetInputEngine().GetKey(LinaEngine::Input::InputCode::Key::K))
	{
		LinaEngine::ECS::TransformComponent& tr = ecs.get<LinaEngine::ECS::TransformComponent>(cube1);

		tr.transform.m_location.x += delta * 10;
		ecs.EntityTransformationUpdate(cube1);
	}


}