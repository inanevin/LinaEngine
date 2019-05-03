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
#include "ECS/Components/FreeLookComponent.hpp"


namespace LinaEngine::Graphics
{

	using namespace ECS;
	using namespace Physics;

	InputMouseButtonBinder* mouseButtonBinder;
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
	Sampler* sampler;
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

	RenderEngine::RenderEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());

		m_RenderDevice = std::make_unique<PAMRenderDevice>();
	}

	RenderEngine::~RenderEngine()
	{
		delete renderableMeshSystem;
		delete freeLookSystem;
		delete cameraSystem;
		delete target;
		delete context;
		delete gameRenderContext;
		delete shader;
		delete texture;
		delete textureNew;
		delete textures[0];
		delete textures[1];
		delete sampler;
		delete vertexArray;
		delete cubeArray;

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(EntityComponentSystem* ecsIn)
	{
		// Set ECS reference
		ECS = ecsIn;

		mouseButtonBinder = new InputMouseButtonBinder();
		mouseButtonBinder->SetActionDispatcher(&Application::Get().GetInputDevice());
		mouseButtonBinder->Initialize(InputCode::Mouse::Mouse1);
		freeLookSystem = new FreeLookSystem(Application::Get().GetInputDevice());
		freeLookSystem->SetWindowCenter(m_RenderDevice->GetWindowSize());
		m_RenderDevice->Initialize();

		target = new RenderTarget(*m_RenderDevice.get());
		context = new RenderContext(*m_RenderDevice.get(), *target);


		ModelLoader::LoadModels("Resources/Mesh/cube.obj", models, modelMaterialIndices, modelMaterials);
		ModelLoader::LoadModels("Resources/Mesh/tinycube.obj", models, modelMaterialIndices, modelMaterials);
		vertexArray = new VertexArray(*m_RenderDevice.get(), models[0], BufferUsage::USAGE_STATIC_DRAW);
		cubeArray = new VertexArray(*m_RenderDevice.get(), models[1], BufferUsage::USAGE_STATIC_DRAW);
		sampler = new Sampler(*m_RenderDevice.get(), SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);
		vertexArrayAABBCube = models[0].GetAABBForElementArray(0);
		vertexArrayAABBTinyCube = models[1].GetAABBForElementArray(0);

		/*if (!ddsTexture.Load("../res/textures/bricks.dds"))
		{
			LINA_CORE_ERR("Could not load texture!");
		}*/

		arrayBitmap.Load("Resources/Textures/grid.png");

		//texture = new Texture(*m_RenderDevice.get(), ddsTexture);
		texture = new Texture(*m_RenderDevice.get(), arrayBitmap, PixelFormat::FORMAT_RGB, true, false);

		/*if (!ddsTexture.Load("../res/textures/bricks2.dds"))
		{
			LINA_CORE_ERR("Could not load texture! :(");
		}*/

		arrayBitmap.Load("Resources/Textures/checker.png");

		//textureNew = new Texture(*m_RenderDevice.get(), ddsTexture);
		textureNew = new Texture(*m_RenderDevice.get(), arrayBitmap, PixelFormat::FORMAT_RGB, true, false);
		LinaString shaderText;
		LinaEngine::Internal::loadTextFileWithIncludes(shaderText, "Resources/Shaders/basicShader.glsl", "#include");
		shader = new Shader(*m_RenderDevice.get(), shaderText);

		shader->SetSampler("diffuse", *texture, *sampler, 0);

		drawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		drawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		drawParams.shouldWriteDepth = true;
		drawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;


		perspective = Matrix::perspective(Math::ToRadians(70.0f / 2.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);


		gameRenderContext = new GameRenderContext(*m_RenderDevice.get(), *target, drawParams, *shader, *sampler, perspective);

		textures[0] = texture;
		textures[1] = textureNew;
		renderableMeshSystem = new RenderableMeshSystem(*gameRenderContext);
		cubeChunkRenderSystem = new CubeChunkRenderSystem(*gameRenderContext, *cubeArray, textures, ARRAY_SIZE_IN_ELEMENTS(textures));
		cameraSystem = new CameraSystem(*gameRenderContext);
		colliderComponent.aabb = vertexArrayAABBCube;


		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 0.0f));

		transformComponent.transform.SetScale(Vector3F(1.0f));
		cameraComponent.fieldOfView = 35;
		cameraComponent.zNear = 0.1f;
		cameraComponent.zFar = 1000.0f;





		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(1.0f, 0.0f, 0.0f) * 7, Application::Get().GetInputDevice().GetHorizontalKeyAxis()));
		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(0.0f, 1.0f, 0.0f) * 7, Application::Get().GetInputDevice().GetVerticalKeyAxis()));
		renderableMesh.vertexArray = vertexArray;
		renderableMesh.texture = &(*texture);


		freeLookComponent.movementSpeedX = 13.0f;
		freeLookComponent.movementSpeedZ = 13.0f;
		freeLookComponent.rotationSpeedX = 0.25f;
		freeLookComponent.rotationSpeedY = 0.25f;

		cameraEntity = ECS->MakeEntity(transformComponent, cameraComponent, freeLookComponent);


		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 20.0f));

		transformComponent.transform.SetScale(1);
		entity = ECS->MakeEntity(transformComponent, renderableMesh, colliderComponent);
		//entity = ECS->MakeEntity(transformComponent, movementComponent, renderableMesh);

		for (uint32 i = 0; i < 1; i++)
		{
			transformComponent.transform.SetLocation(Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f, 20.0f));
			transformComponent.transform.SetScale(1.0f);
			renderableMesh.vertexArray = &*cubeArray;
			renderableMesh.texture = Math::RandF() > 0.5f ? &*texture : &*textureNew;
			float vf = -4.0f;
			float af = 5.0f;
			motionComponent.acceleration = Vector3F(Math::RandF(-af, af), Math::RandF(-af, af), Math::RandF(-af, af));
			motionComponent.velocity = motionComponent.acceleration * vf;

			for (uint32 i = 0; i < 3; i++)
			{
				cubeChunkComponent.position[i] = transformComponent.transform.GetLocation()[i];
				cubeChunkComponent.velocity[i] = motionComponent.velocity[i];
				cubeChunkComponent.acceleration[i] = motionComponent.acceleration[i];
				cubeChunkComponent.textureIndex = Math::RandF() > 0.5f ? 0 : 1;

			}
			//ECS->MakeEntity(cubeChunkComponent);
			//ECS->MakeEntity(transformComponent,  motionComponent, renderableMesh, colliderComponent);
			colliderComponent.aabb = vertexArrayAABBTinyCube;

			ECS->MakeEntity(transformComponent, renderableMesh, colliderComponent);

		}

		renderingPipeline.AddSystem(*renderableMeshSystem);
		renderingPipeline.AddSystem(*cameraSystem);
		//renderingPipeline.AddSystem(*cubeChunkRenderSystem);
		mainSystems.AddSystem(movementControlSystem);
		//mainSystems.AddSystem(motionSystem);
		mainSystems.AddSystem(*freeLookSystem);
		//mainSystems.AddSystem(cubeChunkSystem);
	}


	void RenderEngine::Tick()
	{




		ECS->UpdateSystems(mainSystems, 0.01f);


	}

	void RenderEngine::Render()
	{
		gameRenderContext->Clear(Color(0.2f, 0.225f, 0.12f, 1.0f), true);
		ECS->UpdateSystems(renderingPipeline, 0.01f);
		gameRenderContext->Flush();
		m_RenderDevice->TickWindow();

	}

}
