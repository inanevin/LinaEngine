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

	InputMouseButtonBinder* mouseButtonBinder;
	EntityHandle entity;
	EntityHandle cameraEntity;
	TransformComponent transformComponent;
	MovementControlComponent movementComponent;
	RenderableMeshComponent renderableMesh;
	ECSSystemList mainSystems;

	MovementControlSystem movementControlSystem;

	MotionSystem motionSystem;
	MotionComponent motionComponent;
	CubeChunkComponent cubeChunkComponent;
	ColliderComponent colliderComponent;
	CubeChunkSystem cubeChunkSystem;
	FreeLookSystem* freeLookSystem;
	FreeLookComponent freeLookComponent;
	CubeChunkRenderSystem* cubeChunkRenderSystem;
	CameraComponent cameraComponent;
	RenderContext* context;

	Texture* textures[2];
	VertexArray* vertexArray;
	VertexArray* cubeArray;
	AABB vertexArrayAABBCube;
	AABB vertexArrayAABBTinyCube;
	LinaArray<IndexedModel> models;
	LinaArray<uint32> modelMaterialIndices;
	LinaArray<Material> modelMaterials;
	DDSTexture ddsTexture;
	InputKeyAxisBinder* secondaryHorizontal;
	InputKeyAxisBinder* secondaryVertical;

	RenderEngine::RenderEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());

		m_RenderDevice = std::make_unique<PAMRenderDevice>();
	}

	RenderEngine::~RenderEngine()
	{
		delete freeLookSystem;
		delete context;

		delete textures[0];
		delete textures[1];
		delete vertexArray;
		delete cubeArray;

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(EntityComponentSystem* ecsIn)
	{
		// Set ECS reference
		m_ECS = ecsIn;

		// Initialize default sampler.
		m_DefaultSampler.Construct(*m_RenderDevice.get(), SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

		freeLookSystem = new FreeLookSystem(Application::Get().GetInputDevice());
		freeLookSystem->SetWindowCenter(m_RenderDevice->GetWindowSize() / 2.0f);
		m_RenderDevice->Initialize();

	
		
		m_DefaultRenderTarget.Construct(*m_RenderDevice.get());
		//context = new RenderContext(*m_RenderDevice.get(), *target);


		ModelLoader::LoadModels("Resources/Mesh/cube.obj", models, modelMaterialIndices, modelMaterials);
		ModelLoader::LoadModels("Resources/Mesh/tinycube.obj", models, modelMaterialIndices, modelMaterials);
		vertexArray = new VertexArray(*m_RenderDevice.get(), models[0], BufferUsage::USAGE_STATIC_DRAW);
		cubeArray = new VertexArray(*m_RenderDevice.get(), models[1], BufferUsage::USAGE_STATIC_DRAW);
		
		
		vertexArrayAABBCube = models[0].GetAABBForElementArray(0);
		vertexArrayAABBTinyCube = models[1].GetAABBForElementArray(0);

		/*if (!ddsTexture.Load("../res/textures/bricks.dds"))
		{
			LINA_CORE_ERR("Could not load texture!");
		}*/

		// Initialize default texture.
		m_DefaultTextureBitmap.Load("Resources/Textures/seamless1.jpg");

		//texture = new Texture(*m_RenderDevice.get(), ddsTexture);
		
		m_DefaultDiffuseTexture.Construct(*m_RenderDevice.get(), m_DefaultTextureBitmap, PixelFormat::FORMAT_RGB, true, false);

		/*if (!ddsTexture.Load("../res/textures/bricks2.dds"))
		{
			LINA_CORE_ERR("Could not load texture! :(");
		}*/

		// Initialize default texture.
		m_DefaultTextureBitmap.Load("Resources/Textures/seamless1.jpg");

		
		LinaString shaderText;
		LinaEngine::Internal::LoadTextFileWithIncludes(shaderText, "Resources/Shaders/basicShader.glsl", "#include");

		m_DefaultShader.Construct(*m_RenderDevice.get(), shaderText);
		m_DefaultShader.SetSampler("diffuse", m_DefaultDiffuseTexture, m_DefaultSampler, 0);

		m_DefaultDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_DefaultDrawParams.shouldWriteDepth = true;
		m_DefaultDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;

		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		m_DefaultPerspective = Matrix::perspective(Math::ToRadians(m_ActiveCameraComponent.fieldOfView / 2.0f), windowSize.GetX() / windowSize.GetY(), m_ActiveCameraComponent.zNear, m_ActiveCameraComponent.zFar);


		m_DefaultRenderContext.Construct(*m_RenderDevice.get(), m_DefaultRenderTarget, m_DefaultDrawParams, m_DefaultShader, m_DefaultSampler, m_DefaultPerspective);


		textures[0] = &m_DefaultDiffuseTexture;
		textures[1] = &m_DefaultDiffuseTexture;
		m_RenderableMeshSystem.Construct(m_DefaultRenderContext);
		cubeChunkRenderSystem = new CubeChunkRenderSystem(m_DefaultRenderContext, *cubeArray, textures, ARRAY_SIZE_IN_ELEMENTS(textures));
		m_CameraSystem.Construct(m_DefaultRenderContext);
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());
		colliderComponent.aabb = vertexArrayAABBCube;


		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 0.0f));

		transformComponent.transform.SetScale(Vector3F(1.0f));
		cameraComponent.fieldOfView = 35;
		cameraComponent.zNear = 0.1f;
		cameraComponent.zFar = 1000.0f;



		secondaryHorizontal = new InputKeyAxisBinder();
		secondaryVertical = new InputKeyAxisBinder();
	
		secondaryHorizontal->SetActionDispatcher(&Application::Get().GetInputDevice());
		secondaryVertical->SetActionDispatcher(&Application::Get().GetInputDevice());
		secondaryHorizontal->Initialize(InputCode::Key::L, InputCode::Key::J);
		secondaryVertical->Initialize(InputCode::Key::I, InputCode::Key::K);

		movementComponent.movementControls.push_back(MovementControl(Vector3F(1.0f, 0.0f, 0.0f) * 15, secondaryHorizontal));
		movementComponent.movementControls.push_back(MovementControl(Vector3F(0.0f, 1.0f, 0.0f) * 15, secondaryVertical));
		renderableMesh.vertexArray = vertexArray;
		renderableMesh.texture = &(m_DefaultDiffuseTexture);


		freeLookComponent.movementSpeedX = 13.0f;
		freeLookComponent.movementSpeedZ = 13.0f;
		freeLookComponent.rotationSpeedX = 0.25f;
		freeLookComponent.rotationSpeedY = 0.25f;

		cameraEntity = m_ECS->MakeEntity(transformComponent, cameraComponent, freeLookComponent);

		

		transformComponent.transform.SetLocation(Vector3F(0.0f, 0.0f, 20.0f));

		transformComponent.transform.SetScale(1);
		entity = m_ECS->MakeEntity(transformComponent, renderableMesh, colliderComponent, movementComponent, motionComponent);
		//entity = ECS->MakeEntity(transformComponent, movementComponent, renderableMesh);

		for (uint32 i = 0; i < 1; i++)
		{
			transformComponent.transform.SetLocation(Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f, 20.0f));
			transformComponent.transform.SetScale(1.0f);
			renderableMesh.vertexArray = &*cubeArray;
			renderableMesh.texture = Math::RandF() > 0.5f ? &m_DefaultDiffuseTexture : &m_DefaultDiffuseTexture;
			float vf = -4.0f;
			float af = 5.0f;
			//motionComponent.acceleration = Vector3F(Math::RandF(-af, af), Math::RandF(-af, af), Math::RandF(-af, af));
			//motionComponent.velocity = motionComponent.acceleration * vf;

			/*for (uint32 i = 0; i < 3; i++)
			{
				cubeChunkComponent.position[i] = transformComponent.transform.GetLocation()[i];
				cubeChunkComponent.velocity[i] = motionComponent.velocity[i];
				cubeChunkComponent.acceleration[i] = motionComponent.acceleration[i];
				cubeChunkComponent.textureIndex = Math::RandF() > 0.5f ? 0 : 1;

			}*/
			//ECS->MakeEntity(cubeChunkComponent);
			//ECS->MakeEntity(transformComponent,  motionComponent, renderableMesh, colliderComponent);
			colliderComponent.aabb = vertexArrayAABBTinyCube;

			m_ECS->MakeEntity(transformComponent, renderableMesh, motionComponent, colliderComponent);

		}

		m_RenderingPipeline.AddSystem(m_RenderableMeshSystem);
		m_RenderingPipeline.AddSystem(m_CameraSystem);
		//renderingPipeline.AddSystem(*cubeChunkRenderSystem);
		mainSystems.AddSystem(movementControlSystem);
		mainSystems.AddSystem(motionSystem);
		mainSystems.AddSystem(*freeLookSystem);
		//mainSystems.AddSystem(cubeChunkSystem);
	}


	void RenderEngine::Tick(float delta)
	{




		m_ECS->UpdateSystems(mainSystems, 0.01f);

		m_DefaultRenderContext.Clear(Color(0.2f, 0.225f, 0.12f, 1.0f), true);
		m_ECS->UpdateSystems(m_RenderingPipeline, 0.01f);
		m_DefaultRenderContext.Flush();
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

}
