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

namespace LinaEngine::Graphics
{

	using namespace ECS;


	EntityComponentSystem ecs;
	EntityHandle entity;
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
	CubeChunkSystem cubeChunkSystem;
	CubeChunkRenderSystem* cubeChunkRenderSystem;
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
		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize()
	{
		m_RenderDevice->Initialize();

		target = new RenderTarget(*m_RenderDevice.get());
		context = new RenderContext(*m_RenderDevice.get(), *target);
	
	
		ModelLoader::LoadModels("Resources/Mesh/room.fbx", models, modelMaterialIndices, modelMaterials);
		ModelLoader::LoadModels("Resources/Mesh/tinycube.obj", models, modelMaterialIndices, modelMaterials);
		vertexArray = new VertexArray(*m_RenderDevice.get(), models[0], BufferUsage::USAGE_STATIC_DRAW);
		cubeArray = new VertexArray(*m_RenderDevice.get(), models[1], BufferUsage::USAGE_STATIC_DRAW);
		sampler = new Sampler(*m_RenderDevice.get(), SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

		/*if (!ddsTexture.Load("../res/textures/bricks.dds"))
		{
			LINA_CORE_ERR("Could not load texture!");
		}*/

		arrayBitmap.Load("Resources/Textures/seamless1.jpg");

		//texture = new Texture(*m_RenderDevice.get(), ddsTexture);
		texture = new Texture(*m_RenderDevice.get(), arrayBitmap, PixelFormat::FORMAT_RGB, true, false);

		/*if (!ddsTexture.Load("../res/textures/bricks2.dds")) 
		{
			LINA_CORE_ERR("Could not load texture! :(");
		}*/
		
		arrayBitmap.Load("Resources/Textures/seamless2.jpg");

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

		transformComponent.transform.SetTranslation(Vector3F(0.0f, 0, 50.0f));

		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(1.0f, 0.0f, 0.0f) * 3, Application::Get().GetInputDevice().GetHorizontalKeyAxis()));
		movementComponent.movementControls.push_back(LinaMakePair(Vector3F(0.0f, 1.0f, 0.0f) * 3, Application::Get().GetInputDevice().GetVerticalKeyAxis()));
		renderableMesh.vertexArray = vertexArray;
		renderableMesh.texture = &(*texture);

		entity = ecs.MakeEntity(transformComponent, movementComponent, renderableMesh);

		for (uint32 i = 0; i < 1000; i++)
		{
			transformComponent.transform.SetTranslation(Vector3F(Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f, Math::RandF()*10.0f - 5.0f + 40.0f));

			renderableMesh.vertexArray = &*cubeArray;
			renderableMesh.texture = Math::RandF() > 0.5f ? &*texture : &*textureNew;
			float vf = -4.0f;
			float af = 5.0f;
			motionComponent.acceleration = Vector3F(Math::RandF(-af, af), Math::RandF(-af, af), Math::RandF(-af, af));
			motionComponent.velocity = motionComponent.acceleration * vf;

			for (uint32 i = 0; i < 3; i++)
			{
				cubeChunkComponent.position[i] = transformComponent.transform.GetTranslation()[i];
				cubeChunkComponent.velocity[i] = motionComponent.velocity[i];
				cubeChunkComponent.acceleration[i] = motionComponent.acceleration[i];
				cubeChunkComponent.textureIndex = Math::RandF() > 0.5f ? 0 : 1;

			}
			ecs.MakeEntity(cubeChunkComponent);
			//ecs.MakeEntity(transformComponent,  motionComponent, renderableMesh);
		}

		renderingPipeline.AddSystem(*renderableMeshSystem);
		renderingPipeline.AddSystem(*cubeChunkRenderSystem);
		mainSystems.AddSystem(movementControlSystem);
		//mainSystems.AddSystem(motionSystem);
		mainSystems.AddSystem(cubeChunkSystem);
	}

	void RenderEngine::Tick()
	{
		

		ecs.UpdateSystems(mainSystems, 0.01f);
		
	}

	void RenderEngine::Render()
	{
		gameRenderContext->Clear(Color(0.2f, 0.35f, 0.42f, 1.0f), true);
		ecs.UpdateSystems(renderingPipeline, 0.01f);
		gameRenderContext->Flush();
		m_RenderDevice->TickWindow();

	}

}
