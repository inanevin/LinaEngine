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
#include "ECS/Components/RenderableMeshComponent.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/MotionComponent.hpp"
#include "ECS/Components/CubeChunkComponent.hpp"
#include "ECS/Systems/MotionSystem.hpp"
#include "ECS/Systems/MovementControlSystem.hpp"
#include "ECS/Systems/RenderableMeshSystem.hpp"
#include "ECS/Systems/CubeChunkRenderSystem.hpp"
#include "ECS/Systems/CubeChunkSystem.hpp"
#include "PackageManager/PAMInputEngine.hpp"
#include "Core/Application.hpp"

#include "Physics/PhysicsInteractionWorld.hpp"
#include "ECS/Components/ColliderComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/Components/FreeLookComponent.hpp"


namespace LinaEngine::Graphics
{

	using namespace ECS;
	using namespace Physics;

	EntityHandle entity;
	EntityHandle cameraEntity;
	TransformComponent transformComponent;
	RenderableMeshComponent renderableMesh;


	RenderableObjectData* cube1;
	RenderableObjectData* cube2;
	Texture* text1;
	Texture* text2;


	FreeLookComponent freeLookComponent;
	FreeLookSystem* fss;

	RenderEngine::RenderEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());

		// Create the render device
		m_RenderDevice = std::make_unique<PAMRenderDevice>();
	}

	RenderEngine::~RenderEngine()
	{
		// Clear texture resources.
		for (LinaMap<Texture*, ArrayBitmap*>::iterator it = m_TextureResources.begin(); it != m_TextureResources.end(); ++it)
		{
			delete (*it).second;
			delete (*it).first;
		}

		m_TextureResources.clear();

		// Clear model resources.
		for (uint32 i = 0; i < m_RenderableObjectDataResources.size(); i++)
			delete m_RenderableObjectDataResources[i];

		m_RenderableObjectDataResources.clear();

		// Dump the remaining memory.
		DumpMemory();

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(EntityComponentSystem* ecsIn)
	{
		// Set ECS reference
		m_ECS = ecsIn;

		// Initialize the render device.
		m_RenderDevice->Initialize();

		// Initialize default sampler.
		m_DefaultSampler.Construct(*m_RenderDevice.get(), SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

		// Initialize default texture.
		m_DefaultTextureBitmap.Load("Resources/Textures/seamless1.jpg");
		m_DefaultDiffuseTexture.Construct(*m_RenderDevice.get(), m_DefaultTextureBitmap, PixelFormat::FORMAT_RGB, true, false);
	
		LinaString shaderText;
		LinaEngine::Internal::LoadTextFileWithIncludes(shaderText, "Resources/Shaders/basicShader.glsl", "#include");
		m_DefaultShader.Construct(*m_RenderDevice.get(), shaderText);
		m_DefaultShader.SetSampler("diffuse", m_DefaultDiffuseTexture, m_DefaultSampler, 0);
		
		// Initialize the render target.
		m_DefaultRenderTarget.Construct(*m_RenderDevice.get());

		// Set default drawing parameters.
		m_DefaultDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_DefaultDrawParams.shouldWriteDepth = true;
		m_DefaultDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;

		// Initialize default perspective.
		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		m_DefaultPerspective = Matrix::perspective(Math::ToRadians(m_ActiveCameraComponent.fieldOfView / 2.0f), windowSize.GetX() / windowSize.GetY(), m_ActiveCameraComponent.zNear, m_ActiveCameraComponent.zFar);

		// Initialize the render context.
		m_DefaultRenderContext.Construct(*m_RenderDevice.get(), m_DefaultRenderTarget, m_DefaultDrawParams, m_DefaultShader, m_DefaultSampler, m_DefaultPerspective);

		// Initialize ECS Camera System.
		m_CameraSystem.Construct(m_DefaultRenderContext);
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());

		// Initialize ECS Mesh Render System.
		m_RenderableMeshSystem.Construct(m_DefaultRenderContext);

		freeLookComponent.movementSpeedX = freeLookComponent.movementSpeedZ = 10.0f;
		freeLookComponent.rotationSpeedX = freeLookComponent.rotationSpeedY = 1;
		fss = new FreeLookSystem(Application::Get().GetInputDevice());

		cube1 = &LoadModelResource("cube.obj");
		cube2 = &LoadModelResource("tinycube.obj");
		text1 = &LoadTextureResource("chicken.png", PixelFormat::FORMAT_RGB, true, false);
		text2 = &LoadTextureResource("default_display.png", PixelFormat::FORMAT_RGB, true, false);

		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 0.0f));

		transformComponent.transform.SetScale(Vector3F(1.0f));



		renderableMesh.vertexArray = cube1->GetVertexArray(0);
		renderableMesh.texture = text1;



		cameraEntity = m_ECS->MakeEntity(transformComponent, m_ActiveCameraComponent, freeLookComponent);

		

		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 5.0f));

		transformComponent.transform.SetScale(1);
		entity = m_ECS->MakeEntity(transformComponent, renderableMesh);

		transformComponent.transform.SetLocation(Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f, 5.0f));
		m_ECS->MakeEntity(transformComponent, renderableMesh);
		m_RenderingPipeline.AddSystem(m_RenderableMeshSystem);
		m_RenderingPipeline.AddSystem(m_CameraSystem);
		m_RenderingPipeline.AddSystem(*fss);
	
	}


	void RenderEngine::Tick(float delta)
	{
		// Clear color.
		m_DefaultRenderContext.Clear(m_ActiveCameraComponent.clearColor, true);

		// Update pipeline.
		m_ECS->UpdateSystems(m_RenderingPipeline, delta);

		// Flush data.
		m_DefaultRenderContext.Flush();

		// Update window.
		m_RenderDevice->TickWindow();
	}

	void RenderEngine::OnWindowResized(float width, float height)
	{
		// Propogate to render device.
		m_RenderDevice->OnWindowResized(width, height);

		// Update camera system's aspect ratio.
		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());

	}

	Texture & RenderEngine::LoadTextureResource(const LinaString & fileName, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
	{
		// Create pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();
		textureBitmap->Load(ResourceConstants::textureFolderPath + fileName);

		// Create texture based on pixel data.
		Texture* texture = new Texture();
		texture->Construct(*m_RenderDevice.get(), *textureBitmap, internalPixelFormat, generateMipMaps, compress);

		// Feed the array bitmap data into memory dump to be cleared later.
		if (m_TextureResources.count(texture) == 1)
			m_PixelDump.push_back(m_TextureResources.at(texture));

		// Assign resource.
		m_TextureResources[texture] = textureBitmap;

		return *texture;
	}

	RenderableObjectData & RenderEngine::LoadModelResource(const LinaString & fileName)
	{
		// Create object data & feed it from model.
		RenderableObjectData* objectData = new RenderableObjectData();
		ModelLoader::LoadModels(ResourceConstants::meshFolderPath + fileName, objectData->GetIndexedModels(), objectData->GetMaterialIndices(), objectData->GetMaterialSpecs());

		if (objectData->GetIndexedModels().size() == 0)
			LINA_CORE_ERR("Indexed model array is empty! The model with the name: {0} at path {1} could not be found or model scene does not contain any mesh! This will cause undefined behaviour or crashes if it is assigned to a ECS RenderableMeshComponent."
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

	void RenderEngine::RemoveTextureResource(Texture & textureResource)
	{
		// Feed into memory map & erase if exists.
		if (m_TextureResources.count(&textureResource) == 1)
		{
			m_PixelDump.push_back(m_TextureResources.at(&textureResource));
			m_TextureDump.push_back(&textureResource);
			m_TextureResources.erase(&textureResource);
		}
	}

	void RenderEngine::RemoveModelResource(RenderableObjectData & modelResource)
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

	void RenderEngine::DumpMemory()
	{
		// Free pixel dump
		for (uint32 i = 0; i < m_PixelDump.size(); i++)
			delete m_PixelDump[i];

		// Free renderable object dump
		for (uint32 i = 0; i < m_RenderableObjectDataDump.size(); i++)
			delete m_RenderableObjectDataDump[i];

		// Free texture dump
		for (uint32 i = 0; i < m_TextureDump.size(); i++)
			delete m_TextureDump[i];

		// Clear dumps.
		m_PixelDump.clear();
		m_RenderableObjectDataDump.clear();
		m_TextureDump.clear();
	}

}
