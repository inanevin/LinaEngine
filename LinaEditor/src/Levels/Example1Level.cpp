
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
Material* floorPBRMaterial;
Material* glockMaterial;
Material* sofaMaterial;
Material* arcadeMaterial;
Material* sphereMaterial;
Material* sphereMaterial2;

TransformComponent object1Transform;
MeshRendererComponent object1Renderer;
MeshRendererComponent smallCubeRenderer;

ECSEntity oneSphere;

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
	mat.SetColor("material.endColor", Color::Black);
	mat.SetVector3("material.sunDirection", Vector3(0.0f, -1.0f, 0.0f));
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateCubemapSkybox(RenderEngine* renderEngine)
{
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_CUBEMAP);

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
	Texture& t = renderEngine->CreateTextureCubemap(fp, samplerParams, false);
	mat.SetTexture(MC_TEXTURE2D_DIFFUSE, &t, TextureBindMode::BINDTEXTURE_CUBEMAP);
	renderEngine->SetSkyboxMaterial(mat);
}

void CreateHDRISkybox(RenderEngine* renderEngine)
{
	Texture& hdri = renderEngine->CreateTextureHDRI("resources/textures/HDRI/canyon3K.hdr");
	renderEngine->CaptureCalculateHDRI(hdri);
	Material& mat = renderEngine->CreateMaterial("skyboxMaterial", Shaders::SKYBOX_HDRI);
	mat.SetTexture(UF_MAP_ENVIRONMENT, &renderEngine->GetHDRICubemap(), TextureBindMode::BINDTEXTURE_CUBEMAP);
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

int pLightSize = 1;
int cubeSize = 4;
int sLightSize = 0;



Texture* bricksParallax;
Texture* bricksParallax2;
ECSEntity cubeEntity;
TransformComponent* dirLightT;
void Example1Level::Initialize()
{

	LINA_CLIENT_WARN("Example level 1 initialize.");

	// Create, setup & assign skybox material.
	CreateHDRISkybox(m_RenderEngine);

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



	SamplerParameters pbrSampler;
	pbrSampler.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
	pbrSampler.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
	pbrSampler.textureParams.wrapS = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.textureParams.wrapT = SamplerWrapMode::WRAP_REPEAT;
	pbrSampler.textureParams.pixelFormat = PixelFormat::FORMAT_RGBA;
	pbrSampler.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB;
	pbrSampler.textureParams.generateMipMaps = true;



	// Create texture for example mesh.
	Texture& brickWall = m_RenderEngine->CreateTexture2D("resources/textures/bricks2.jpg", woodTextureSampler, false);
	Texture& brickWallNormal = m_RenderEngine->CreateTexture2D("resources/textures/bricks2_normal.jpg", crateSampler, false);
	bricksParallax = &m_RenderEngine->CreateTexture2D("resources/textures/bricks2_disp3.jpg", crateSampler, false);
	bricksParallax2 = &m_RenderEngine->CreateTexture2D("resources/textures/bricks2_disp4.jpg", crateSampler, false);

	Texture& albedo = m_RenderEngine->CreateTexture2D("resources/textures/wall/albedo.png", pbrSampler, false, false);
	Texture& normal = m_RenderEngine->CreateTexture2D("resources/textures/wall/normal.png", pbrSampler, false, false);
	Texture& metallic = m_RenderEngine->CreateTexture2D("resources/textures/wall/metallic.png", pbrSampler, false, false);
	Texture& roughness = m_RenderEngine->CreateTexture2D("resources/textures/wall/roughness.png", pbrSampler, false, false);
	Texture& ao = m_RenderEngine->CreateTexture2D("resources/textures/wall/ao.png", pbrSampler, false, false);

	//Texture& albedoGlock = m_RenderEngine->CreateTexture2D("resources/textures/glock/albedo.png", pbrSampler, false, false);
	//Texture& normalGlock = m_RenderEngine->CreateTexture2D("resources/textures/glock/normal.png", pbrSampler, false, false);
	//Texture& metallicGlock = m_RenderEngine->CreateTexture2D("resources/textures/glock/metallic.png", pbrSampler, false, false);
	//Texture& roughnessGlock = m_RenderEngine->CreateTexture2D("resources/textures/glock/roughness.png", pbrSampler, false, false);
	//Texture& aoGlock = m_RenderEngine->CreateTexture2D("resources/textures/glock/ao.png", pbrSampler, false, false);
	//
	//Texture& albedoSofa = m_RenderEngine->CreateTexture2D("resources/textures/sofa/albedo.png", pbrSampler, false, false);
	//Texture& normalSofa = m_RenderEngine->CreateTexture2D("resources/textures/sofa/normal.png", pbrSampler, false, false);
	//Texture& metallicSofa = m_RenderEngine->CreateTexture2D("resources/textures/sofa/metallic.png", pbrSampler, false, false);
	//Texture& roughnessSofa = m_RenderEngine->CreateTexture2D("resources/textures/sofa/roughness.png", pbrSampler, false, false);

	//Texture& albedoArcade = m_RenderEngine->CreateTexture2D("resources/textures/arcade/albedo.png", pbrSampler, false, false);
	//Texture& normalArcade = m_RenderEngine->CreateTexture2D("resources/textures/arcade/normal.png", pbrSampler, false, false);
	//Texture& metallicArcade = m_RenderEngine->CreateTexture2D("resources/textures/arcade/metallic.png", pbrSampler, false, false);
	//Texture& roughnessArcade = m_RenderEngine->CreateTexture2D("resources/textures/arcade/roughness.png", pbrSampler, false, false);
	//Texture& aoArcade = m_RenderEngine->CreateTexture2D("resources/textures/arcade/ao.png", pbrSampler, false, false);

	Texture& albedoSphere = m_RenderEngine->CreateTexture2D("resources/textures/rusted_iron/albedo.png", pbrSampler, false, false);
	Texture& normalSphere = m_RenderEngine->CreateTexture2D("resources/textures/rusted_iron/normal.png", pbrSampler, false, false);
	Texture& metallicSphere = m_RenderEngine->CreateTexture2D("resources/textures/rusted_iron/metallic.png", pbrSampler, false, false);
	Texture& roughnessSphere = m_RenderEngine->CreateTexture2D("resources/textures/rusted_iron/roughness.png", pbrSampler, false, false);
	Texture& aoSphere = m_RenderEngine->CreateTexture2D("resources/textures/rusted_iron/ao.png", pbrSampler, false, false);


	// Load example mesh.
	Mesh& cubeMesh = m_RenderEngine->GetPrimitive(Primitives::SPHERE);

	// Create material for example mesh.
	objectLitMaterial = &m_RenderEngine->CreateMaterial("object1Material", Shaders::STANDARD_LIT);
	objectUnlitMaterial = &m_RenderEngine->CreateMaterial("object2Material", Shaders::STANDARD_UNLIT);
	objectUnlitMaterial2 = &m_RenderEngine->CreateMaterial("object3Material", Shaders::STANDARD_UNLIT);
	floorPBRMaterial = &m_RenderEngine->CreateMaterial("floorPBR", Shaders::PBR_LIT);
	glockMaterial = &m_RenderEngine->CreateMaterial("glock", Shaders::PBR_LIT);
	sofaMaterial = &m_RenderEngine->CreateMaterial("sofa", Shaders::PBR_LIT);
	sphereMaterial = &m_RenderEngine->CreateMaterial("sp", Shaders::PBR_LIT);
	sphereMaterial2 = &m_RenderEngine->CreateMaterial("sp2", Shaders::PBR_LIT);
	//arcadeMaterial = &m_RenderEngine->CreateMaterial("arcade", Shaders::PBR_LIT);

	floorPBRMaterial->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedo);
	floorPBRMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &normal);
	floorPBRMaterial->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughness);
	floorPBRMaterial->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallic);
	floorPBRMaterial->SetTexture(MC_TEXTURE2D_AOMAP, &ao);
	floorPBRMaterial->SetVector2(MC_TILING, Vector2(20, 20));



	sphereMaterial->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoSphere);
	sphereMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &normalSphere);
	sphereMaterial->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessSphere);
	sphereMaterial->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicSphere);
	sphereMaterial->SetTexture(MC_TEXTURE2D_AOMAP, &aoSphere);

	//sphereMaterial2->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoSphere);
	//sphereMaterial2->SetTexture(MC_TEXTURE2D_NORMALMAP, &normalSphere);
	//sphereMaterial2->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessSphere);
	//sphereMaterial2->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicSphere);
	//sphereMaterial2->SetTexture(MC_TEXTURE2D_AOMAP, &aoSphere);
	//sphereMaterial2->SetFloat(MC_METALLICMULTIPLIER, 4);
	//sphereMaterial2->SetFloat(MC_ROUGHNESSMULTIPLIER, .2f);
	//	sphereMaterial->SetVector2(MC_TILING, Vector2(20, 20));



	//sofaMaterial->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoSofa);
	//sofaMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &normalSofa);
	//sofaMaterial->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessSofa);
	//sofaMaterial->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicSofa);
	//sofaMaterial->SetVector2(MC_TILING, Vector2(1, 1));
	////
	//glockMaterial->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoGlock);
	//glockMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &normalGlock);
	//glockMaterial->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessGlock);
	//glockMaterial->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicGlock);
	//glockMaterial->SetTexture(MC_TEXTURE2D_AOMAP, &aoGlock);



	//arcadeMaterial->SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoArcade);
	//arcadeMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &normalArcade);
	//arcadeMaterial->SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessArcade);
	//arcadeMaterial->SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicArcade);
//	arcadeMaterial->SetTexture(MC_TEXTURE2D_AOMAP, &aoArcade);

	//quadMaterial = &m_RenderEngine->CreateMaterial("quadMaterial", Shaders::STANDARD_LIT);
	floorMaterial = &m_RenderEngine->CreateMaterial("floor", Shaders::STANDARD_LIT);
	//cubemapReflectiveMaterial = &m_RenderEngine->CreateMaterial("cubemapReflective", Shaders::CUBEMAP_REFLECTIVE);


	//objectLitMaterial->SetTexture(MC_TEXTURE2D_DIFFUSE, &brickWall);
	//objectLitMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &brickWallNormal);
	//objectLitMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 1, 1));
	//objectLitMaterial->SetSurfaceType(MaterialSurfaceType::Opaque);
	//objectUnlitMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(200, 200, 200));
	//objectUnlitMaterial2->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 1, 1));
	//
	floorMaterial->SetTexture(MC_TEXTURE2D_DIFFUSE, &brickWall);
	floorMaterial->SetTexture(MC_TEXTURE2D_NORMALMAP, &brickWallNormal);
	floorMaterial->SetTexture(MC_TEXTURE2D_PARALLAXMAP, bricksParallax);
	floorMaterial->SetVector2(MC_TILING, Vector2(20, 20));
	floorMaterial->SetColor(MC_OBJECTCOLORPROPERTY, Color(1, 1, 1));

	//floorMaterial->SetTexture(MC_TEXTURE2D_SPECULAR, &wood);
	//cubemapReflectiveMaterial->SetTexture(UF_SKYBOXTEXTURE, &cubemap, 0);
	//quadMaterial->SetTexture(MC_TEXTURE2D_DIFFUSE, &window);
	//quadMaterial->SetSurfaceType(MaterialSurfaceType::Transparent);


	object1Renderer.mesh = &cubeMesh;
	object1Renderer.material = objectLitMaterial;
	smallCubeRenderer.mesh = &cubeMesh;
	smallCubeRenderer.material = objectUnlitMaterial;



	//directionalLight = m_ECS->CreateEntity("Directional Light");
	//auto& dirLight = m_ECS->emplace<DirectionalLightComponent>(directionalLight);
	//auto& dirLightTransform = m_ECS->emplace<TransformComponent>(directionalLight);
	//dirLight.color = Color(0.0f, 0.0f, 0.0f);
	//dirLight.direction = Vector3(0, -0.5, 1);
	//dirLightTransform.transform.location = Vector3(2.5f, 5.5f, 0.0f);
	//dirLightTransform.transform.scale = Vector3(0.2f);
	//dirLightT = &m_ECS->get<TransformComponent>(directionalLight);


//ECSEntity glock;
//glock = m_ECS->CreateEntity("Glock");
//MeshRendererComponent glockMR;
//glockMR.mesh = &m_RenderEngine->CreateMesh("resources/meshes/glock.fbx");
//glockMR.material = glockMaterial;
//object1Transform.transform.location = Vector3(0, 4, 0);
//object1Transform.transform.scale = Vector3(0.1f);
//m_ECS->emplace<TransformComponent>(glock, object1Transform);
//m_ECS->emplace<MeshRendererComponent>(glock, glockMR);
//
//

//ECSEntity sofa;
//sofa = m_ECS->CreateEntity("sofa");
//MeshRendererComponent sofaMR;
//sofaMR.mesh = &m_RenderEngine->CreateMesh("resources/meshes/sofa.fbx");
//sofaMR.material = sofaMaterial;
//object1Transform.transform.location = Vector3(0, 4, 0);
//object1Transform.transform.scale = Vector3(0.05f);
//m_ECS->emplace<TransformComponent>(sofa, object1Transform);
//m_ECS->emplace<MeshRendererComponent>(sofa, sofaMR);

//ECSEntity arcade;
//arcade = m_ECS->CreateEntity("arcade");
//MeshRendererComponent arcadeMR;
//arcadeMR.mesh = &m_RenderEngine->CreateMesh("resources/meshes/arcade.obj");
//arcadeMR.material = arcadeMaterial;
//object1Transform.transform.location = Vector3(0, 4, 0);
//object1Transform.transform.scale = Vector3(0.05f);
//m_ECS->emplace<TransformComponent>(arcade, object1Transform);
//m_ECS->emplace<MeshRendererComponent>(arcade, arcadeMR);


//ECSEntity floor;
//floor = m_ECS->CreateEntity("Floor");
//MeshRendererComponent mr;
//mr.mesh = &m_RenderEngine->GetPrimitive(Primitives::PLANE);
//mr.material = floorMaterial;
//
//object1Transform.transform.location = Vector3(0, 0, 0);
//object1Transform.transform.scale = Vector3(40.0f);
//m_ECS->emplace<TransformComponent>(floor, object1Transform);
//m_ECS->emplace<MeshRendererComponent>(floor, mr);





	object1Transform.transform.location = Vector3(-6, 18, 5);
	object1Transform.transform.scale = Vector3::One;
	object1Transform.transform.rotation = Quaternion::Euler(Vector3::Zero);

/*	for (int i = 0; i < 25; i++)
	{
		ECSEntity entity;

		object1Renderer.material = sphereMaterial;
		entity = m_ECS->CreateEntity("Cube " + std::to_string(i));
		m_ECS->emplace<TransformComponent>(entity, object1Transform);
		m_ECS->emplace<MeshRendererComponent>(entity, object1Renderer);
		object1Transform.transform.location += i % 5 != 4 ? Vector3(3, 0, 0) : Vector3(-12, -3, 0);


	}*/
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 3.0f;
	
	for (int row = 0; row < nrRows; ++row)
	{
	//	float metallic = (float)nrRows / (float)(row+1);

		for (int col = 0; col < nrColumns; ++col)
		{
			Material& mat = m_RenderEngine->CreateMaterial("sp" + std::to_string(row) + std::to_string(col), Shaders::PBR_LIT);
			mat.SetTexture(MC_TEXTURE2D_ALBEDOMAP, &albedoSphere);
			mat.SetTexture(MC_TEXTURE2D_NORMALMAP, &normalSphere);
			mat.SetTexture(MC_TEXTURE2D_ROUGHNESSMAP, &roughnessSphere);
			mat.SetTexture(MC_TEXTURE2D_METALLICMAP, &metallicSphere);
			mat.SetTexture(MC_TEXTURE2D_AOMAP, &aoSphere);
			//mat.SetFloat(MC_ROUGHNESSMULTIPLIER, glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
		//	mat.SetFloat(MC_METALLICMULTIPLIER, metallic);

			ECSEntity entity;

			object1Renderer.material = &mat;
			entity = m_ECS->CreateEntity("Cube " + std::to_string(row+col));
			m_ECS->emplace<TransformComponent>(entity, object1Transform);
			m_ECS->emplace<MeshRendererComponent>(entity, object1Renderer);
			object1Transform.transform.location = glm::vec3(
				(float)(col - (nrColumns / 2)) * spacing,
				(float)(row - (nrRows / 2)) * spacing,
				5.0f
			);
		}
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
		pLight1.color = Color(1, 1, 1);
		smallCubeRenderer.material->SetColor(MC_OBJECTCOLORPROPERTY, pLight1.color);
		m_ECS->emplace<MeshRendererComponent>(entity, smallCubeRenderer);

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



	//t.transform.location = Vector3(0, 0, Math::Sin(t2) * 5);
}