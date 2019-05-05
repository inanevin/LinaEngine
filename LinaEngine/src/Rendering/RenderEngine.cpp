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
#include "Core/Internal.hpp"
#include "Rendering/ModelLoader.hpp"
#include "Utility/ResourceConstants.hpp"

/*#include "Rendering/Material.hpp"
#include "Rendering/ModelLoader.hpp"

#include "ECS/EntityComponentSystem.hpp"
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
#include "Rendering/ArrayBitmap.hpp"
#include "Physics/PhysicsInteractionWorld.hpp"
#include "ECS/Components/ColliderComponent.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/FreeLookSystem.hpp"
#include "ECS/Components/FreeLookComponent.hpp"*/

using namespace LinaEngine::Internal;

namespace LinaEngine::Graphics
{


	/*InputMouseButtonBinder* mouseButtonBinder;
	EntityHandle entity;
	EntityHandle cameraEntity;
	TransformComponent transformComponent;
	MovementControlComponent movementComponent;
	RenderableMeshComponent renderableMesh;
	ECSSystemList mainSystems;
	ECSSystemList renderingPipeline;
	MovementControlSystem movementControlSystem;
	RenderableMeshSystem* renderableMeshSystem;
	MotionSystem motionSystem;
	MotionComponent motionComponent;
	CubeChunkComponent cubeChunkComponent;
	ColliderComponent colliderComponent;
	CubeChunkSystem cubeChunkSystem;
	FreeLookSystem* freeLookSystem;
	FreeLookComponent freeLookComponent;
	CubeChunkRenderSystem* cubeChunkRenderSystem;
	CameraComponent cameraComponent;
	CameraSystem* cameraSystem;
	RenderTarget* target;
	RenderContext* context;
	GameRenderContext* gameRenderContext;
	Shader* shader;
	Texture* texture;
	Texture* textureNew;
	Texture* textures[2];
	VertexArray* vertexArray;
	VertexArray* cubeArray;
	AABB vertexArrayAABBCube;
	AABB vertexArrayAABBTinyCube;
	LinaArray<IndexedModel> models;
	LinaArray<uint32> modelMaterialIndices;
	LinaArray<Material> modelMaterials;
	DDSTexture ddsTexture;
	ArrayBitmap arrayBitmap;
	DrawParams drawParams;
	Matrix perspective;
	InputKeyAxisBinder* secondaryHorizontal;
	InputKeyAxisBinder* secondaryVertical;*/


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
		// Initialize default sampler.
		m_DefaultSampler.Construct(*m_RenderDevice.get(), SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

		// Initialize default texture.
		m_DefaultTextureBitmap.Load("Resources/Textures/checker.png");
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
		m_DefaultPerspective = Matrix::perspective(m_ActiveCameraComponent.fieldOfView / 2.0f, windowSize.GetX() / windowSize.GetY(), m_ActiveCameraComponent.zNear, m_ActiveCameraComponent.zFar);

		// Initialize the render context.
		m_DefaultRenderContext.Construct(*m_RenderDevice.get(), m_DefaultRenderTarget, m_DefaultDrawParams, m_DefaultShader, m_DefaultSampler, m_DefaultPerspective);

		// Initialize ECS Camera System.
		m_CameraSystem.Construct(m_DefaultRenderContext);
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());

		// Initialize ECS Mesh Render System.
		m_RenderableMeshSystem.Construct(m_DefaultRenderContext);

		// Set ECS reference
		m_ECS = ecsIn;

		// Add ECS systems to pipeline.
		m_RenderingPipeline.AddSystem(m_CameraSystem);
		m_RenderingPipeline.AddSystem(m_RenderableMeshSystem);

		// Set clear color.
		m_ActiveCameraComponent.clearColor = Color(0.15f, 0.22f, 0.38f, 1.0f);

		// Initialize the render device.
		m_RenderDevice->Initialize();

		//freeLookSystem = new FreeLookSystem(Application::Get().GetInputDevice());
		//freeLookSystem->SetWindowCenter(m_RenderDevice->GetWindowSize() / 2.0f);
		//m_RenderDevice->Initialize();
		//
		////target = new RenderTarget(*m_RenderDevice.get());
		////context = new RenderContext(*m_RenderDevice.get(), *target);
		//
		//
		//ModelLoader::LoadModels("Resources/Mesh/cube.obj", models, modelMaterialIndices, modelMaterials);
		//ModelLoader::LoadModels("Resources/Mesh/tinycube.obj", models, modelMaterialIndices, modelMaterials);
		//vertexArray = new VertexArray(*m_RenderDevice.get(), models[0], BufferUsage::USAGE_STATIC_DRAW);
		//cubeArray = new VertexArray(*m_RenderDevice.get(), models[1], BufferUsage::USAGE_STATIC_DRAW);
		//
		//vertexArrayAABBCube = models[0].GetAABBForElementArray(0);
		//vertexArrayAABBTinyCube = models[1].GetAABBForElementArray(0);
		//
		///*if (!ddsTexture.Load("../res/textures/bricks.dds"))
		//{
		//	LINA_CORE_ERR("Could not load texture!");
		//}*/
		//
		//arrayBitmap.Load("Resources/Textures/grid.png");
		//
		////texture = new Texture(*m_RenderDevice.get(), ddsTexture);
		////texture = new Texture(*m_RenderDevice.get(), arrayBitmap, PixelFormat::FORMAT_RGB, true, false);
		//
		///*if (!ddsTexture.Load("../res/textures/bricks2.dds"))
		//{
		//	LINA_CORE_ERR("Could not load texture! :(");
		//}*/
		//
		//arrayBitmap.Load("Resources/Textures/checker.png");
		//
		////textureNew = new Texture(*m_RenderDevice.get(), ddsTexture);
		////textureNew = new Texture(*m_RenderDevice.get(), arrayBitmap, PixelFormat::FORMAT_RGB, true, false);
		//LinaString shaderText;
	//	//LinaEngine::Internal::loadTextFileWithIncludes(shaderText, "Resources/Shaders/basicShader.glsl", "#include");
		////shader = new Shader(*m_RenderDevice.get(), shaderText);
		//
		////shader->SetSampler("diffuse", *texture, m_DefaultSampler, 0);
		//
		//drawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		//drawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		//drawParams.shouldWriteDepth = true;
		//drawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;
		//
		//
		//perspective = Matrix::perspective(Math::ToRadians(70.0f / 2.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
		//
		//
		////gameRenderContext = new GameRenderContext(*m_RenderDevice.get(), *target, drawParams, *shader, m_DefaultSampler, perspective);
		//
		//textures[0] = texture;
		//textures[1] = textureNew;
		////renderableMeshSystem = new RenderableMeshSystem(*gameRenderContext);
		//cubeChunkRenderSystem = new CubeChunkRenderSystem(*gameRenderContext, *cubeArray, textures, ARRAY_SIZE_IN_ELEMENTS(textures));
		////cameraSystem = new CameraSystem(*gameRenderContext);
		//Vector2F windowSize = m_RenderDevice->GetWindowSize();
		//cameraSystem->SetAspectRatio(windowSize.GetX() / windowSize.GetY());
		//colliderComponent.aabb = vertexArrayAABBCube;
		//
		//
		//transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 0.0f));
		//
		//transformComponent.transform.SetScale(Vector3F(1.0f));
		//cameraComponent.fieldOfView = 35;
		//cameraComponent.zNear = 0.1f;
		//cameraComponent.zFar = 1000.0f;
		//
		//
		//
		//secondaryHorizontal = new InputKeyAxisBinder();
		//secondaryVertical = new InputKeyAxisBinder();
		//
		//secondaryHorizontal->SetActionDispatcher(&Application::Get().GetInputDevice());
		//secondaryVertical->SetActionDispatcher(&Application::Get().GetInputDevice());
		//secondaryHorizontal->Initialize(InputCode::Key::L, InputCode::Key::J);
		//secondaryVertical->Initialize(InputCode::Key::I, InputCode::Key::K);
		//
		//movementComponent.movementControls.push_back(MovementControl(Vector3F(1.0f, 0.0f, 0.0f) * 15, secondaryHorizontal));
		//movementComponent.movementControls.push_back(MovementControl(Vector3F(0.0f, 1.0f, 0.0f) * 15, secondaryVertical));
		//renderableMesh.vertexArray = vertexArray;
		//renderableMesh.texture = &(*texture);
		//
		//
		//freeLookComponent.movementSpeedX = 13.0f;
		//freeLookComponent.movementSpeedZ = 13.0f;
		//freeLookComponent.rotationSpeedX = 0.25f;
		//freeLookComponent.rotationSpeedY = 0.25f;
		//
		////cameraEntity = ECS->MakeEntity(transformComponent, cameraComponent, freeLookComponent);
		//
		//
		//
		//transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 20.0f));
		//
		//transformComponent.transform.SetScale(1);
	//	//entity = ECS->MakeEntity(transformComponent, renderableMesh, colliderComponent, movementComponent, motionComponent);
		////entity = ECS->MakeEntity(transformComponent, movementComponent, renderableMesh);
		//
		//for (uint32 i = 0; i < 1; i++)
		//{
		//	transformComponent.transform.SetLocation(Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f, 20.0f));
		//	transformComponent.transform.SetScale(1.0f);
		//	renderableMesh.vertexArray = &*cubeArray;
		//	renderableMesh.texture = Math::RandF() > 0.5f ? &*texture : &*textureNew;
		//	float vf = -4.0f;
		//	float af = 5.0f;
		//	//motionComponent.acceleration = Vector3F(Math::RandF(-af, af), Math::RandF(-af, af), Math::RandF(-af, af));
		//	//motionComponent.velocity = motionComponent.acceleration * vf;
		//
		//	/*for (uint32 i = 0; i < 3; i++)
		//	{
		//		cubeChunkComponent.position[i] = transformComponent.transform.GetLocation()[i];
		//		cubeChunkComponent.velocity[i] = motionComponent.velocity[i];
		//		cubeChunkComponent.acceleration[i] = motionComponent.acceleration[i];
		//		cubeChunkComponent.textureIndex = Math::RandF() > 0.5f ? 0 : 1;
		//
		//	}*/
		//	//ECS->MakeEntity(cubeChunkComponent);
		//	//ECS->MakeEntity(transformComponent,  motionComponent, renderableMesh, colliderComponent);
		//	colliderComponent.aabb = vertexArrayAABBTinyCube;
		//
		////	ECS->MakeEntity(transformComponent, renderableMesh, motionComponent, colliderComponent);
		//
		//}
		//
		//renderingPipeline.AddSystem(*renderableMeshSystem);
		//renderingPipeline.AddSystem(*cameraSystem);
		////renderingPipeline.AddSystem(*cubeChunkRenderSystem);
		//mainSystems.AddSystem(movementControlSystem);
		//mainSystems.AddSystem(motionSystem);
		//mainSystems.AddSystem(*freeLookSystem);
		//mainSystems.AddSystem(cubeChunkSystem);
	}


	void RenderEngine::Tick(float delta)
	{
		// Clear color.
		m_DefaultRenderContext.Clear(m_ActiveCameraComponent.clearColor, false);

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

	Texture & RenderEngine::LoadTextureResource(const LinaString & texturePath, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
	{
		// Create pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();
		textureBitmap->Load(ResourceConstants::textureFolderPath + texturePath);

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
