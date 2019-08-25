/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: RenderEngine
Timestamp: 4/27/2019 11:18:07 PM

*/

#include "LinaPch.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Utility/Math/Color.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/ModelLoader.hpp"
#include "Utility/ResourceConstants.hpp"
#include "ECS/Components/MovementControlComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MotionComponent.hpp"
#include "ECS/Components/CubeChunkComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Systems/MotionSystem.hpp"
#include "ECS/Systems/MovementControlSystem.hpp"
#include "ECS/Systems/MeshRendererSystem.hpp"
#include "ECS/Systems/CubeChunkRenderSystem.hpp"
#include "ECS/Systems/CubeChunkSystem.hpp"
#include "PackageManager/PAMInputEngine.hpp"
#include "Core/Application.hpp"

#include "Physics/PhysicsInteractionWorld.hpp"
#include "ECS/Components/ColliderComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/Components/FreeLookComponent.hpp"

#include "glad/glad.h"

namespace LinaEngine::Graphics
{

	using namespace ECS;
	using namespace Physics;

	
	EntityHandle cameraEntity;
	TransformComponent transformComponent;

	FreeLookComponent freeLookComponent;
	FreeLookSystem* fss;
	CubeChunkRenderSystem* cbrs;
	CubeChunkSystem* ccs;
	CubeChunkComponent ccc;
	MotionComponent motionComponent;
	EntityHandle entities[100000];

	RenderEngine::RenderEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());

		// Create the render device
		m_RenderDevice = std::make_unique<PAMRenderDevice>();
	}

	RenderEngine::~RenderEngine()
	{
		// Clear texture resources.
		for (uint32 i = 0; i < m_TextureResources.size(); i++)
			delete m_TextureResources[i];

		m_TextureResources.clear();

		// Clear model resources.
		for (uint32 i = 0; i < m_RenderableObjectDataResources.size(); i++)
			delete m_RenderableObjectDataResources[i];

		m_RenderableObjectDataResources.clear();

		// Dump the remaining memory.
		DumpMemory();

		// Release Vertex Array Objects
		m_SkyboxVAO = m_RenderDevice->ReleaseVertexArray(m_SkyboxVAO);

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	static const int DEMO_TEXTURE_SIZES = 5;
	RenderableObjectData* cube1;
	RenderableObjectData* cube2;
	EntityHandle entity;
	EntityHandle entity2;
	Texture* texture;
	Texture* textureNew;
	Texture* textures[DEMO_TEXTURE_SIZES];
	Texture texture0;
	Texture texture1;
	Texture texture2;
	Texture texture3;
	Texture texture4;
	Texture skybox2;
	Texture skybox3;
	Texture skybox4;
	Texture skybox5;
	Texture* skyboxToDraw;

	MeshRendererComponent renderableMesh;


	void RenderEngine::Initialize(EntityComponentSystem* ecsIn)
	{
		// Set ECS reference
		m_ECS = ecsIn;

		// Initialize the render device.
		m_RenderDevice->Initialize(m_LightingSystem);

		// Initialize default sampler.
		m_DefaultSampler.Construct("diffuse", *m_RenderDevice.get(), SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

		// Initialize skybox sampler.
		m_SkyboxSampler.Construct("skybox", *m_RenderDevice.get(), SamplerFilter::FILTER_NEAREST);

		// Initialize default texture.
		m_DefaultDiffuseTexture = LoadTextureResource("seamless1.jpg", PixelFormat::FORMAT_RGB, true, false);

		// Initialize default skybox texture
		m_SkyboxTexture = LoadCubemapTextureResource("Skybox/Skybox1/right.jpg", "Skybox/Skybox1/left.jpg", "Skybox/Skybox1/up.jpg", "Skybox/Skybox1/down.jpg", "Skybox/Skybox1/front.jpg", "Skybox/Skybox1/back.jpg");

		skybox2 = LoadCubemapTextureResource("Skybox/Skybox2/right.png", "Skybox/Skybox2/left.png", "Skybox/Skybox2/up.png", "Skybox/Skybox2/down.png", "Skybox/Skybox2/front.png", "Skybox/Skybox2/back.png");
		skybox3 = LoadCubemapTextureResource("Skybox/Skybox3/right.png", "Skybox/Skybox3/left.png", "Skybox/Skybox3/up.png", "Skybox/Skybox3/down.png", "Skybox/Skybox3/front.png", "Skybox/Skybox3/back.png");
		skybox4 = LoadCubemapTextureResource("Skybox/Skybox4/right.png", "Skybox/Skybox4/left.png", "Skybox/Skybox4/up.png", "Skybox/Skybox4/down.png", "Skybox/Skybox4/front.png", "Skybox/Skybox4/back.png");
		skybox5 = LoadCubemapTextureResource("Skybox/Skybox5/right.png", "Skybox/Skybox5/left.png", "Skybox/Skybox5/up.png", "Skybox/Skybox5/down.png", "Skybox/Skybox5/front.png", "Skybox/Skybox5/back.png");

		texture0 = LoadTextureResource("checker.png", PixelFormat::FORMAT_RGB, true, false);
		texture1 = LoadTextureResource("chicken.png", PixelFormat::FORMAT_RGB, true, false);
		texture2 = LoadTextureResource("seamless1.jpg", PixelFormat::FORMAT_RGB, true, false);
		texture3 = LoadTextureResource("redgrid.png", PixelFormat::FORMAT_RGB, true, false);
		texture4 = LoadTextureResource("seamless2.jpg", PixelFormat::FORMAT_RGB, true, false);

		// Initialize basic shader.
		LinaString basicShaderText;
		LinaEngine::Internal::LoadTextFileWithIncludes(basicShaderText, "Resources/Shaders/basicStandardUnlit.glsl", "#include");
		m_BasicStandardShader.Construct(*m_RenderDevice.get(), basicShaderText);
		m_BasicStandardShader.SetSampler(m_DefaultSampler.GetSamplerName(), m_DefaultDiffuseTexture, m_DefaultSampler, 0);

		// Initialize default skybox shader.
		LinaString skyboxShaderText;
		LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, "Resources/Shaders/basicSkybox.glsl", "#include");
		m_BasicSkyboxShader.Construct(*m_RenderDevice.get(), skyboxShaderText);
		m_BasicSkyboxShader.SetSampler(m_SkyboxSampler.GetSamplerName(), m_SkyboxTexture, m_SkyboxSampler, 0);

		// Initialize the render target.
		m_RenderTarget.Construct(*m_RenderDevice.get());

		// Set default drawing parameters.
		m_DefaultDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_DefaultDrawParams.shouldWriteDepth = true;
		m_DefaultDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;
	
		// Set skybox draw params.
		m_SkyboxDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_SkyboxDrawParams.faceCulling = FaceCulling::FACE_CULL_NONE;
		m_SkyboxDrawParams.shouldWriteDepth = true;
		m_SkyboxDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LEQUAL;

		// Initialize default camera.
		SetActiveCameraComponent(m_DefaultCameraComponent);
		m_DefaultCamera = m_ECS->MakeEntity(m_DefaultCameraTransform, m_DefaultCameraComponent);

		// Initialize default perspective.
		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		m_CurrentProjectionMatrix = Matrix::perspective(Math::ToRadians(m_ActiveCameraComponent->fieldOfView / 2.0f), windowSize.GetX() / windowSize.GetY(), m_ActiveCameraComponent->zNear, m_ActiveCameraComponent->zFar);

		// Initialize the render context.
		m_DefaultRenderContext.Construct(*m_RenderDevice.get(), m_RenderTarget, m_DefaultDrawParams, m_BasicStandardShader, m_DefaultSampler, m_CurrentProjectionMatrix);

		// Initialize skybox vertex array object.
		m_SkyboxVAO = m_RenderDevice->CreateSkyboxVertexArray();
	
		// Initialize ECS Camera System.
		m_CameraSystem.Construct(m_DefaultRenderContext);
		m_CameraSystem.SetProjectionMatrix(m_CurrentProjectionMatrix);

		// Initialize ECS Mesh Render System.
		m_MeshRendererSystem.Construct(m_DefaultRenderContext);

		// Initialize ECS lighting system.
		m_LightingSystem.Construct(*m_RenderDevice.get(), m_BasicStandardShader);
		AmbientLightComponent ambientLight;
		ambientLight.color = Color(0, 0, 0.4f);
		ambientLight.intensity = 1.0f;
		SetAmbientLight(ambientLight);

		// CUSTOM 
		freeLookComponent.movementSpeedX = freeLookComponent.movementSpeedZ = 18.0f;		
		freeLookComponent.rotationSpeedX = freeLookComponent.rotationSpeedY = 1;
		fss = new FreeLookSystem(Application::Get().GetInputDevice());

		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, -5.0f));
	
		cameraEntity = m_ECS->MakeEntity(transformComponent, m_DefaultCameraComponent, freeLookComponent);

		textures[0] = &texture0;
		textures[1] = &texture1;
		textures[2] = &texture2;
		textures[3] = &texture3;
		textures[4] = &texture4;

		renderableMesh.texture = &m_DefaultDiffuseTexture;
		cube1 = &LoadModelResource("tinycube.obj");

		renderableMesh.vertexArray = cube1->GetVertexArray(0);

		cbrs = new CubeChunkRenderSystem(m_DefaultRenderContext, *renderableMesh.vertexArray, textures, ARRAY_SIZE_IN_ELEMENTS(textures));
		ccs = new CubeChunkSystem();

		// Add the ECS systems into the pipeline.
		//m_RenderingPipeline.AddSystem(m_MeshRendererSystem);
		m_RenderingPipeline.AddSystem(m_CameraSystem);
		m_RenderingPipeline.AddSystem(m_LightingSystem);
		m_RenderingPipeline.AddSystem(*cbrs);
		m_RenderingPipeline.AddSystem(*ccs);
		m_RenderingPipeline.AddSystem(*fss);

		transformComponent.transform.SetLocation(Vector3F(-12.0f, 0, 0.0f));
		//transformComponent.transform.SetScale(Vector3F(10, 0.2f, 10));
		

	
		for (uint32 i = 0; i < 100000; i++)
		{
			transformComponent.transform.SetLocation(Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f,
				Math::RandF()*10.0f - 5.0f + 20.0f));

			float vf = -4.0f;
			float af = 5.0f;
			motionComponent.acceleration = Vector3F(Math::RandF(-af, af), Math::RandF(-af, af), Math::RandF(-af, af));
			motionComponent.velocity = motionComponent.acceleration * vf;

			for (uint32 i = 0; i < 3; i++)
			{
				ccc.position[i] = transformComponent.transform.GetLocation()[i];
				ccc.velocity[i] = motionComponent.velocity[i];
				ccc.acceleration[i] = motionComponent.acceleration[i];
				//ccc.textureIndex = Math::RandF() > 0.5f ? 0 : 1;

			}
			entities[i] = m_ECS->MakeEntity(ccc);
		}

		//entity = m_ECS->MakeEntity(transformComponent, renderableMesh);

		//transformComponent.transform.SetLocation(Vector3F(12.0f, 0.0f, 0.0f));
		//entity2 = m_ECS->MakeEntity(transformComponent, renderableMesh);

		skyboxToDraw = &m_SkyboxTexture;

	}

	void RenderEngine::DemoSwitchTexture(int index)
	{
		if (index > DEMO_TEXTURE_SIZES - 1) return;
		for (uint32 i = 0; i < 100000; i++)
		{
			m_ECS->GetComponent<CubeChunkComponent>(entities[i])->textureIndex = index;
		}
	}
	void RenderEngine::DemoSwitchMovement(int index)
	{
		ccs->SetMovementIndex(index);
	}

	void RenderEngine::DemoSwitchSkybox(int index)
	{
		if (index == 0)
			skyboxToDraw = &m_SkyboxTexture;
		else if (index == 1)
			skyboxToDraw = &skybox2;
		else if (index == 2)
			skyboxToDraw = &skybox3;
		else if (index == 3)
			skyboxToDraw = &skybox4;
		else if (index == 4)
			skyboxToDraw = &skybox5;
	}


	void RenderEngine::Tick(float delta)
	{

		// Clear color.
		m_DefaultRenderContext.Clear(m_ActiveCameraComponent->clearColor, true);

		m_LightingSystem.SetCameraPosition(m_ECS->GetComponent<TransformComponent>(cameraEntity)->transform.GetLocation());

		m_DefaultRenderContext.UpdateViewMatrix(m_CameraSystem.GetViewMatrix());
		m_DefaultRenderContext.UpdateProjectionMatrix(m_CurrentProjectionMatrix);

		// Update pipeline.
		m_ECS->UpdateSystems(m_RenderingPipeline, delta);


		
		// Draw scene.
		m_DefaultRenderContext.Flush();

		// Draw skybox.
		RenderSkybox(skyboxToDraw);

		// Update window.
		m_RenderDevice->TickWindow();

	}

	void RenderEngine::OnWindowResized(float width, float height)
	{
		// Propogate to render device.
		m_RenderDevice->OnWindowResized(width, height);

		// Update camera system's projection matrix.
		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		m_CurrentProjectionMatrix = Matrix::perspective(Math::ToRadians(m_ActiveCameraComponent->fieldOfView / 2.0f), windowSize.GetX() / windowSize.GetY(), m_ActiveCameraComponent->zNear, m_ActiveCameraComponent->zFar);
		m_CameraSystem.SetProjectionMatrix(m_CurrentProjectionMatrix);
		//fss->SetWindowCenter(Vector2F(windowSize.GetX() / 2, windowSize.GetY() / 2));

	}

	Texture & RenderEngine::LoadTextureResource(const LinaString & fileName, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
	{
		// Create pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();
		textureBitmap->Load(ResourceConstants::textureFolderPath + fileName);

		// Create texture based on pixel data.
		Texture* texture = new Texture();
		texture->Construct(*m_RenderDevice.get(), *textureBitmap, internalPixelFormat, generateMipMaps, compress);

		// Add to the resources.
		m_TextureResources.push_back(texture);

		// Delete pixel data.
		delete textureBitmap;

		// Return texture.
		return *texture;
	}

	RenderableObjectData & RenderEngine::LoadModelResource(const LinaString & fileName)
	{
		// Create object data & feed it from model.
		RenderableObjectData* objectData = new RenderableObjectData();
		ModelLoader::LoadModels(ResourceConstants::meshFolderPath + fileName, objectData->GetIndexedModels(), objectData->GetMaterialIndices(), objectData->GetMaterialSpecs());

		if (objectData->GetIndexedModels().size() == 0)
			LINA_CORE_ERR("Indexed model array is empty! The model with the name: {0} at path {1} could not be found or model scene does not contain any mesh! This will cause undefined behaviour or crashes if it is assigned to a ECS MeshRendererComponent."
				, fileName, ResourceConstants::meshFolderPath);

		// Create vertex array for each mesh.
		for (uint32 i = 0; i < objectData->GetIndexedModels().size(); i++)
		{
			VertexArray* vertexArray = new VertexArray();
			vertexArray->Construct(*m_RenderDevice.get(), objectData->GetIndexedModels()[i], BufferUsage::USAGE_STATIC_DRAW);
			objectData->GetVertexArrays().push_back(vertexArray);
		}

		// Push & return.
		m_RenderableObjectDataResources.push_back(objectData);
		return *objectData;
	}

	LINA_API Texture& RenderEngine::LoadCubemapTextureResource(const LinaString & filePosX, const LinaString & fileNegX, const LinaString & filePosY, const LinaString & fileNegY, const LinaString & filePosZ, const LinaString & fileNegZ)
	{
		LinaArray<ArrayBitmap*> bitmaps;
		const LinaString fileNames[] = { filePosX, fileNegX, filePosY, fileNegY, filePosZ, fileNegZ };

		// Load bitmap for each face.
		for (uint32 i = 0; i < 6; i++)
		{	
			ArrayBitmap* faceBitmap = new ArrayBitmap();
			faceBitmap->Load(ResourceConstants::textureFolderPath + fileNames[i]);
			bitmaps.push_back(faceBitmap);
			
		}

		// Create texture.
		Texture* cubeMapTexture = new Texture();
		cubeMapTexture->Construct(*m_RenderDevice.get(), bitmaps, PixelFormat::FORMAT_RGB, true, false);

		// Add to the resources.
		m_TextureResources.push_back(cubeMapTexture);

		// Delete pixel data.
		for (uint32 i = 0; i < bitmaps.size(); i++)
			delete bitmaps[i];

		bitmaps.clear();
		
		return *cubeMapTexture;
	}

	void RenderEngine::UnloadTextureResource(Texture & textureResource)
	{
		// Find the resource.
		for (size_t i = 0; i < m_TextureResources.size(); i++)
		{
			// If found.
			if (m_TextureResources[i] == &textureResource)
			{
				// Add to dump.
				m_TextureDump.push_back(&textureResource);

				// Push to the end of the list & pop.
				m_TextureResources.swap_remove(i);
			}
		}
	}

	void RenderEngine::UnloadModelResource(RenderableObjectData & modelResource)
	{
		// Find the resource.
		for (size_t i = 0; i < m_RenderableObjectDataResources.size(); i++)
		{
			// If found.
			if (m_RenderableObjectDataResources[i] == &modelResource)
			{
				// Add to dump.
				m_RenderableObjectDataDump.push_back(&modelResource);

				// Push to the end of the list & pop.
				m_RenderableObjectDataResources.swap_remove(i);
			}
		}

	}

	void RenderEngine::OnRemoveEntity(EntityHandle handle)
	{
		// Get the camera component from the entity to be removed.
		CameraComponent* cameraComponent = m_ECS->GetComponent<CameraComponent>(handle);

		// If the camera component exists...
		if (cameraComponent)
		{
			// Check if the camera component is the active one in use, if so, switch back to default camera component.
			if (cameraComponent == m_ActiveCameraComponent)
			{
				cameraComponent->isActive = false;
				m_ActiveCameraComponent = &m_DefaultCameraComponent;
				m_ActiveCameraComponent->isActive = true;
			}
		}
	}

	void RenderEngine::OnRemoveComponent(EntityHandle handle, uint32 id)
	{
		// If we are removing a camera component.
		if (id == CameraComponent::ID)
		{
			// Get the camera component from the entity to be removed.
			CameraComponent* cameraComponent = m_ECS->GetComponent<CameraComponent>(handle);

			// If the camera component exists...
			if (cameraComponent)
			{
				// Check if the camera component is the active one in use, if so, switch back to default camera component.
				if (cameraComponent == m_ActiveCameraComponent)
				{
					cameraComponent->isActive = false;
					m_ActiveCameraComponent = &m_DefaultCameraComponent;
					m_ActiveCameraComponent->isActive = true;
				}
			}
		}
	}

	void RenderEngine::DumpMemory()
	{
		// Free renderable object dump
		for (uint32 i = 0; i < m_RenderableObjectDataDump.size(); i++)
			delete m_RenderableObjectDataDump[i];

		// Free texture dump
		for (uint32 i = 0; i < m_TextureDump.size(); i++)
			delete m_TextureDump[i];

		// Clear dumps.
		m_RenderableObjectDataDump.clear();
		m_TextureDump.clear();
	}

	void RenderEngine::RenderSkybox(Texture* skyboxTexture)
	{
		m_BasicSkyboxShader.SetSampler(m_SkyboxSampler.GetSamplerName(), *skyboxTexture, m_SkyboxSampler, 0, BindTextureMode::BINDTEXTURE_CUBEMAP);
		m_RenderDevice->DrawSkybox(m_RenderTarget.GetID(), m_BasicSkyboxShader.GetID(),  m_SkyboxVAO, skyboxTexture->GetID(), m_SkyboxDrawParams, m_CurrentProjectionMatrix, m_CameraSystem.GetSkyboxViewTransformation());
	}

}
