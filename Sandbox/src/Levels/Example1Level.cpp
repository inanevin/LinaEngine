
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

	Level::Install(loadFromFile, path, levelName);

	SamplerParameters pbrSampler;
	pbrSampler.m_textureParams.m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	pbrSampler.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
	pbrSampler.m_textureParams.m_wrapS = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGBA;
	pbrSampler.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB;
	pbrSampler.m_textureParams.m_generateMipMaps = true;

	return true;
}

void CreateSingleColorSkybox(RenderEngine& renderEngine)
{
	//Material& mat = LinaEngine::Graphics::Material::CreateMaterial(Shaders::Skybox_SingleColor);
	//mat.SetColor("material.color", Color::Red);
	//renderEngine.SetSkyboxMaterial(&mat);
}

void CreateGradientSkybox(RenderEngine& renderEngine)
{
	//Material& mat = LinaEngine::Graphics::Material::CreateMaterial(Shaders::Skybox_Gradient);
	//mat.SetColor("material.startColor", Color::Green);
	//mat.SetColor("material.endColor", Color::White);
	//renderEngine.SetSkyboxMaterial(&mat);
}

void CreateProceduralSkybox(RenderEngine& renderEngine)
{
//Material& mat = LinaEngine::Graphics::Material::CreateMaterial(Shaders::Skybox_Procedural);
//mat.SetColor("material.startColor", Color::White);
//mat.SetColor("material.endColor", Color(0.2f, 0.2f, 0.2f));
//mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
//renderEngine.SetSkyboxMaterial(&mat);
}

void CreateHDRISkybox(RenderEngine& renderEngine)
{
	Texture* hdri = &Texture::CreateTextureHDRI("resources/sandbox/textures/HDRI/canyon3K.hdr");
	renderEngine.CaptureCalculateHDRI(*hdri);
///	Material& mat = LinaEngine::Graphics::Material::CreateMaterial(Shaders::Skybox_HDRI);
//	mat.SetTexture(MAT_MAP_ENVIRONMENT, &renderEngine.GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
//	renderEngine.SetSkyboxMaterial(&mat);
}

void CreateAtmosphericSkybox(RenderEngine& renderEngine)
{
//	Material& mat = LinaEngine::Graphics::Material::CreateMaterial(Shaders::Skybox_Atmospheric);
	//mat.SetFloat(MAT_TIME, 15);
	//mat.SetFloat(UF_FLOAT_TIME, 15);
//	renderEngine.SetSkyboxMaterial(&mat);
}
ECSEntity cube1;
ECSEntity cube2;
ECS::FreeLookSystem freeLookSystem;

void Example1Level::Initialize()
{
	LINA_CLIENT_WARN("Example level 1 initialize.");
	// Create a simple procedural skybox.
	LinaEngine::Graphics::RenderEngine& renderEngine = LinaEngine::Application::GetRenderEngine();
	LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();
	//freeLookSystem.Construct(ecs, LinaEngine::Application::GetInputEngine());
	//freeLookSystem.SystemActivation(true);
	//LinaEngine::Application::GetApp().AddToMainPipeline(freeLookSystem);
	//cube1 = ecs.GetEntity("Entity");
}

bool locTog = false;
void Example1Level::Tick(bool isInPlayMode, float delta)
{

	return;
	LinaEngine::ECS::ECSRegistry& ecs = LinaEngine::Application::GetECSRegistry();

	TransformComponent& tr = ecs.get<TransformComponent>(cube1);
	float speed = 10.0f;
	Vector3 loc = tr.transform.GetLocation();

	if (!locTog && loc.x > 9.9f)
	{
		locTog = true;
	}

	if (locTog && loc.x < -9.9f)
		locTog = false;

	if (!locTog)
	{
		loc.x += speed * delta;
	}
	else if (locTog)
	{
		loc.x -= speed * delta;
	}


	tr.transform.SetLocation(loc);
}

