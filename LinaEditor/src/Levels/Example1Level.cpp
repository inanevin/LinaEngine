
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
ECSEntity directionalLight;
ECSEntity quad;
ECSEntity quad2;
ECSEntity quad3;
ECSEntity quad4;

Material* objectLitMaterial;
Material* objectUnlitMaterial;
Material* objectUnlitMaterial2;
Material* quadMaterial;
Material* cubemapReflectiveMaterial;
Material* floorMaterial;

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
	mat.SetColor("material.startColor", Color::Black);
	mat.SetColor("material.endColor", Color::Gray);
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
	SamplerParameters samplerParams;
	samplerParams.textureParams.generateMipMaps = true;
	samplerParams.textureParams.minFilter = FILTER_NEAREST;
	Texture& t = renderEngine->CreateTexture(fp, samplerParams, false);
	mat.SetTexture(MC_TEXTURE2D_DIFFUSE, &t, TextureBindMode::BINDTEXTURE_CUBEMAP);
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
	Vector3(1.0f,  2.0f,  0.0f),
	Vector3(-1.0f,  2.5f,  2.5f),
	Vector3(5.0f,  -6.5f,  13.5f),
	Vector3(0.0f,  -7.5f,  13.5f),
};

Vector3 spotLightPositions[]
{
	Vector3(0,0, -4)
};

int pLightSize = 2;
int cubeSize = 4;
int sLightSize = 1;




ECSEntity cubeEntity;
TransformComponent* dirLightT;
void Example1Level::Initialize()
{

	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	CreateProceduralSkybox(m_RenderEngine);




	camera = m_ECS->CreateEntity("Camera");
	auto& camFreeLook = m_ECS->emplace<FreeLookComponent>(camera);
	auto& camTransform = m_ECS->emplace<TransformComponent>(camera);
	auto& camCamera = m_ECS->emplace<CameraComponent>(camera);
	camTransform.transform.location = Vector3(0, 5, 0);
	camCamera.isActive = true;
	camFreeLook.movementSpeedX = camFreeLook.movementSpeedZ = 12.0f;
	camFreeLook.rotationSpeedX = camFreeLook.rotationSpeedY = 3;

	SamplerParameters woodTextureSampler;
	woodTextureSampler.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	woodTextureSampler.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
	woodTextureSampler.textureParams.wrapS = SamplerWrapMode::WRAP_REPEAT;
	woodTextureSampler.textureParams.wrapT = SamplerWrapMode::WRAP_REPEAT;
	woodTextureSampler.textureParams.pixelFormat = PixelFormat::FORMAT_RGBA;
	woodTextureSampler.textureParams.internalPixelFormat = PixelFormat::FORMAT_SRGBA;
	woodTextureSampler.textureParams.generateMipMaps = true;

	SamplerParameters crateSampler;
	crateSampler.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	crateSampler.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
	crateSampler.textureParams.wrapS = SamplerWrapMode::WRAP_REPEAT;
	crateSampler.textureParams.wrapT = SamplerWrapMode::WRAP_REPEAT;
	crateSampler.textureParams.pixelFormat = PixelFormat::FORMAT_RGBA;
	crateSampler.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB;
	crateSampler.textureParams.generateMipMaps = true;

	// Create texture for example mesh.
	Texture& crateTexture = m_RenderEngine->CreateTexture("resources/textures/box.png", crateSampler);
	Texture& crateSpecTexture = m_RenderEngine->CreateTexture("resources/textures/boxSpecular.png");
	Texture& window = m_RenderEngine->CreateTexture("resources/textures/window.png");
	Texture& wood = m_RenderEngine->CreateTexture("resources/textures/wood.png", woodTextureSampler, false);
	Texture& brickWall = m_RenderEngine->CreateTexture("resources/textures/bricks2.jpg", woodTextureSampler, false);
	Texture& brickWallNormal = m_RenderEngine->CreateTexture("resources/textures/bricks2_normal.jpg", crateSampler, false);
	Texture& bricksParallax = m_RenderEngine->CreateTexture("resources/textures/bricks2_disp.jpg", crateSampler, false);
	//Texture& cubemap = m_RenderEngine->GetTexture("resources/textures/defaultSkybox/right.png");





	// Load example mesh.
	Mesh& cubeMesh = m_RenderEngine->CreateMesh("resources/meshes/cube.obj");

	// Create material for example mesh.
	objectLitMaterial = &m_RenderEngine->CreateMaterial("object1Material", Shaders::STANDARD_LIT);
	objectUnlitMaterial = &m_RenderEngine->CreateMaterial("object2Material", Shaders::STANDARD_UNLIT);
	objectUnlitMaterial2 = &m_RenderEngine->CreateMaterial("object3Material", Shaders::STANDARD_UNLIT);
	//quadMaterial = &m_RenderEngine->CreateMaterial("quadMaterial", Shaders::STANDARD_LIT);
	floorMaterial = &m_RenderEngine->CreateMaterial("floor", Shaders::STANDARD_LIT);
	//cubemapReflectiveMaterial = &m_RenderEngine->CreateMaterial("cubemapReflective", Shaders::CUBEMAP_REFLECTIVE);


	objectLitMaterial->SetTexture(MC_TEXTURE2D_DIFFUSE, &brickWall);
	objectLitMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &brickWallNormal);
	objectLitMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 1, 1));
	objectLitMaterial->SetSurfaceType(MaterialSurfaceType::Opaque);
	objectUnlitMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 1, 1));
	objectUnlitMaterial2->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 1, 1));
	
	floorMaterial->SetTexture(MC_TEXTURE2D_DIFFUSE, &brickWall);
	floorMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &brickWallNormal);
	floorMaterial->SetTexture(MC_TEXTURE2D_PARALLAXMAP, &bricksParallax);
	floorMaterial->SetVector2(MC_TILING, Vector2(20, 20));
	floorMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(1,1,1));

	//floorMaterial->SetTexture(MC_TEXTURE2D_SPECULAR, &wood);
	//cubemapReflectiveMaterial->SetTexture(UF_SKYBOXTEXTURE, &cubemap, 0);
	//quadMaterial->SetTexture(MC_TEXTURE2D_DIFFUSE, &window);
	//quadMaterial->SetSurfaceType(MaterialSurfaceType::Transparent);

	object1Renderer.mesh = &cubeMesh;
	object1Renderer.material = objectLitMaterial;
	smallCubeRenderer.mesh = &cubeMesh;
	smallCubeRenderer.material = objectUnlitMaterial;



	directionalLight = m_ECS->CreateEntity("Directional Light");
	auto& dirLight = m_ECS->emplace<DirectionalLightComponent>(directionalLight);
	auto& dirLightTransform = m_ECS->emplace<TransformComponent>(directionalLight);
	dirLight.color = Color(0.4f,0.4,0.4f);
	dirLight.direction = Vector3(0, -0.5, 1);
	dirLightTransform.transform.location = Vector3(2.5f, 5.5f, 0.0f);
	dirLightTransform.transform.scale = Vector3(0.2f);
	dirLightT = &m_ECS->get<TransformComponent>(directionalLight);
	m_ECS->emplace<MeshRendererComponent>(directionalLight, smallCubeRenderer);


	ECSEntity floor;
	floor = m_ECS->CreateEntity("Floor");
	MeshRendererComponent mr;
	mr.mesh = &m_RenderEngine->GetPrimitive(Primitives::PLANE);
	mr.material = floorMaterial;

	object1Transform.transform.location = Vector3(0, 0, 0);
	object1Transform.transform.scale = Vector3(40.0f);
	m_ECS->emplace<TransformComponent>(floor, object1Transform);
	m_ECS->emplace<MeshRendererComponent>(floor, mr);



	for (int i = 0; i < cubeSize; i++)
	{
		ECSEntity entity;
		object1Renderer.material = objectLitMaterial;

		object1Transform.transform.rotation = Quaternion::Euler(Vector3::Zero);
		object1Transform.transform.location = cubePositions[i];
		object1Transform.transform.scale = Vector3::One;

		if (i == 0)
			object1Transform.transform.scale = Vector3(0.1f);

		if (i == 1)
			object1Transform.transform.scale = Vector3(0.3f);

		if (i == 2)
			object1Transform.transform.scale = Vector3(0.7f);
		if (i == 3)
			object1Transform.transform.scale = Vector3(1.1f);
		//object1Transform.transform.location = Vector3(Math::RandF(-100, 100), Math::RandF(-100, 100), Math::RandF(-100, 100));
		entity = m_ECS->CreateEntity("Cube " + std::to_string(i));
		m_ECS->emplace<TransformComponent>(entity, object1Transform);
		m_ECS->emplace<MeshRendererComponent>(entity, object1Renderer);
	}


	object1Transform.transform.scale = Vector3::One;




	for (int i = 0; i < pLightSize; i++)
	{
		smallCubeRenderer.material = objectUnlitMaterial;

		ECSEntity entity;
		object1Transform.transform.location = pointLightPositions[i];
		object1Transform.transform.scale = 0.1f;
		entity = m_ECS->CreateEntity("Point Light " + i);
		auto lightT = m_ECS->emplace<TransformComponent>(entity, object1Transform);
		auto& pLight1 = m_ECS->emplace<PointLightComponent>(entity);
		m_ECS->emplace<MeshRendererComponent>(entity, smallCubeRenderer);



		pLight1.color = Color(0.85f, 0.85f, 0.85f);
		pLight1.distance = 100;

	}



	for (int i = 0; i < sLightSize; i++)
	{
		smallCubeRenderer.material = objectUnlitMaterial;

		ECSEntity entity;
		object1Transform.transform.location = (spotLightPositions[i]);
		entity = m_ECS->CreateEntity("Spot Light " + i);
		object1Transform.transform.rotation = (Quaternion::Euler(-25, 40, 0));
		object1Transform.transform.scale = Vector3(0.1f);
		smallCubeRenderer.material = objectUnlitMaterial2;
		m_ECS->emplace<TransformComponent>(entity, object1Transform);
		m_ECS->emplace<MeshRendererComponent>(entity, smallCubeRenderer);

		auto& sLight1 = m_ECS->emplace<SpotLightComponent>(entity);


		sLight1.color = Color(0.05f, 0.05f, 0.05f);
		sLight1.distance = 150;
		sLight1.cutOff = Math::Cos(Math::ToRadians(12.5f));
		sLight1.outerCutOff = Math::Cos(Math::ToRadians(15.5f));


	}
	object1Transform.transform.rotation = (Quaternion::Euler(Vector3::Zero));

	object1Transform.transform.scale = (Vector3(1));
	object1Transform.transform.location = (Vector3(0.7f, 0, 5.5f));
	object1Transform.transform.Rotate(-90, 0, 0);



	//quad = m_ECS->create();
	//m_ECS->emplace<TransformComponent>(quad, object1Transform);
	//MeshRendererComponent quadR;
	//quadR.material = quadMaterial;
	//quadR.mesh = &m_RenderEngine->GetPrimitive(Primitives::PLANE);
	//m_ECS->emplace<MeshRendererComponent>(quad, quadR);
	//
	//object1Transform.transform.location = (Vector3(-2.5f, 0, 11.0f));
	//quad2 = m_ECS->create();
	//m_ECS->emplace<TransformComponent>(quad2, object1Transform);
	//m_ECS->emplace<MeshRendererComponent>(quad2, quadR);
	//
	//object1Transform.transform.location = (Vector3(-2.5f, 0, 7.5f));
	//quad3 = m_ECS->create();
	//m_ECS->emplace<TransformComponent>(quad3, object1Transform);
	//m_ECS->emplace<MeshRendererComponent>(quad3, quadR);
	//
	//object1Transform.transform.location = (Vector3(-2.1f, 0, 7.0f));
	//quad4 = m_ECS->create();
	//m_ECS->emplace<TransformComponent>(quad4, object1Transform);
	//m_ECS->emplace<MeshRendererComponent>(quad4, quadR);
	//
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

}