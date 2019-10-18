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
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Systems/MeshRendererSystem.hpp"
#include "PackageManager/PAMInputEngine.hpp"
#include "Core/Application.hpp"
#include "ECS/Systems/CameraSystem.hpp"

#include "glad/glad.h"

namespace LinaEngine::Graphics
{

	using namespace ECS;
	using namespace Physics;

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
		m_DefaultDiffuseTexture = LoadTextureResource(ResourceConstants::textureFolderPath + "defaultDiffuse.png", PixelFormat::FORMAT_RGB, true, false);

		// Initialize basic unlit shader.
		LinaString basicShaderText;
		LinaEngine::Internal::LoadTextFileWithIncludes(basicShaderText, ResourceConstants::shaderFolderPath + "basicStandardUnlit.glsl", "#include");
		m_StandardUnlitShader.Construct(*m_RenderDevice.get(), basicShaderText);
		m_StandardUnlitShader.SetSampler(m_DefaultSampler.GetSamplerName(), m_DefaultDiffuseTexture, m_DefaultSampler, 0);

		// Initialize the correct skybox shader depending on the type of sykbox to draw.
		LinaString skyboxShaderText;

		switch (m_SkyboxType)
		{
		case SkyboxType::SingleColor:
			LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, ResourceConstants::shaderFolderPath + "skyboxSingleColor.glsl", "#include");
			ConstructShader(m_SkyboxSingleColorShader, m_DefaultDiffuseTexture, m_SkyboxSampler, skyboxShaderText, 0);
			break;

		case SkyboxType::Gradient:
			LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, ResourceConstants::shaderFolderPath + "skyboxVertexGradient.glsl", "#include");
			ConstructShader(m_SkyboxGradientShader, m_DefaultDiffuseTexture, m_SkyboxSampler, skyboxShaderText, 0);
			break;

		case SkyboxType::Cubemap:
			LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, ResourceConstants::shaderFolderPath + "skyboxCubemap.glsl", "#include");

			m_SkyboxTexture = LoadCubemapTextureResource(
				ResourceConstants::textureFolderPath + "defaultSkybox/right.png",
				ResourceConstants::textureFolderPath + "defaultSkybox/left.png",
				ResourceConstants::textureFolderPath + "defaultSkybox/up.png",
				ResourceConstants::textureFolderPath + "defaultSkybox/down.png",
				ResourceConstants::textureFolderPath + "defaultSkybox/front.png",
				ResourceConstants::textureFolderPath + "defaultSkybox/back.png");

			ConstructShader(m_SkyboxCubemapShader, m_SkyboxTexture, m_SkyboxSampler, skyboxShaderText, 0);
			break;
		}


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
		m_DefaultCamera = m_ECS->MakeEntity(m_DefaultCameraTransform, m_DefaultCameraComponent);
		DefaultSceneCameraActivation(true);		

		// Initialize the render context.
		m_DefaultRenderContext.Construct(*m_RenderDevice.get(), m_RenderTarget, m_DefaultDrawParams, m_StandardUnlitShader, m_DefaultSampler, Matrix::identity());

		// Initialize skybox vertex array object.
		m_SkyboxVAO = m_RenderDevice->CreateSkyboxVertexArray();
	
		// Initialize ECS Camera System.
		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		m_CameraSystem.Construct(m_DefaultRenderContext);
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());

		// Add the ECS systems into the pipeline.
		m_RenderingPipeline.AddSystem(m_CameraSystem);

	}

	void RenderEngine::Tick(float delta)
	{
		// Clear color.
		m_DefaultRenderContext.Clear(m_CameraSystem.GetActiveClearColor(), true);

		// Update view & proj matrices.
		m_DefaultRenderContext.UpdateViewMatrix(m_CameraSystem.GetViewMatrix());
		m_DefaultRenderContext.UpdateProjectionMatrix(m_CameraSystem.GetProjectionMatrix());

		// Update pipeline.
		m_ECS->UpdateSystems(m_RenderingPipeline, delta);
		
		// Draw scene.
		m_DefaultRenderContext.Flush();

		// Draw skybox.
		RenderSkybox();

		// Update window.
		m_RenderDevice->TickWindow();

	}

	void RenderEngine::OnWindowResized(float width, float height)
	{
		// Propogate to render device.
		m_RenderDevice->OnWindowResized(width, height);

		// Update camera system's projection matrix.
		Vector2F windowSize = m_RenderDevice->GetWindowSize();
		CameraComponent* activeCameraComponent = m_CameraSystem.GetActiveCameraComponent();
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());

	}

	Texture & RenderEngine::LoadTextureResource(const LinaString & fileName, PixelFormat internalPixelFormat, bool generateMipMaps, bool compress)
	{
		// Create pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();
		textureBitmap->Load(fileName);

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
			faceBitmap->Load(fileNames[i]);
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


	LINA_API void RenderEngine::DefaultSceneCameraActivation(bool activation)
	{
		m_ECS->GetComponent<CameraComponent>(m_DefaultCamera)->isActive = activation;
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

	void RenderEngine::RenderSkybox()
	{
		// Draw skybox based on its type.
		switch (m_SkyboxType)
		{
		case SkyboxType::SingleColor:
			m_RenderDevice->DrawSkybox(m_RenderTarget.GetID(), m_SkyboxSingleColorShader.GetID(), m_SkyboxVAO, m_DefaultDiffuseTexture.GetID(), m_SkyboxDrawParams, m_SingleColorSkyboxColor);
			break;

		case SkyboxType::Gradient:
			m_RenderDevice->DrawSkybox(m_RenderTarget.GetID(), m_SkyboxGradientShader.GetID(), m_SkyboxVAO, m_DefaultDiffuseTexture.GetID(), m_SkyboxDrawParams, LinaEngine::Colors::Gray, LinaEngine::Colors::Gray, m_CameraSystem.GetProjectionMatrix(), m_CameraSystem.GetSkyboxViewTransformation());
			break;
			
		case SkyboxType::Cubemap:
			m_RenderDevice->DrawSkybox(m_RenderTarget.GetID(), m_SkyboxCubemapShader.GetID(), m_SkyboxVAO, m_SkyboxTexture.GetID(), m_SkyboxDrawParams, m_CameraSystem.GetProjectionMatrix(), m_CameraSystem.GetSkyboxViewTransformation());
			break;
		}
	}

	void RenderEngine::ConstructShader(Shader& shader, const Texture& texture, const Sampler& sampler, const LinaString& text, uint32 samplerUnit)
	{
		shader.Construct(*m_RenderDevice.get(), text);
		shader.SetSampler(sampler.GetSamplerName(), texture, sampler, samplerUnit);
	}

	void RenderEngine::ChangeSkyboxRenderType(SkyboxType type)
	{

		// Construct the shader of the target type if not done before.
		switch (type)
		{
		case SkyboxType::SingleColor:
			
			if (!m_SkyboxSingleColorShader.GetIsConstructed())
			{
				LinaString skyboxShaderText;
				LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, ResourceConstants::shaderFolderPath + "skyboxSingleColor.glsl", "#include");
				ConstructShader(m_SkyboxSingleColorShader, m_DefaultDiffuseTexture, m_SkyboxSampler, skyboxShaderText, 0);
			}
			break;

		case SkyboxType::Gradient:
			
			if (!m_SkyboxGradientShader.GetIsConstructed())
			{
				LinaString skyboxShaderText;
				LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, ResourceConstants::shaderFolderPath + "skyboxVertexGradient.glsl", "#include");
				ConstructShader(m_SkyboxGradientShader, m_DefaultDiffuseTexture, m_SkyboxSampler, skyboxShaderText, 0);
			}

			break;

		case SkyboxType::Cubemap:
			
			if (!m_SkyboxCubemapShader.GetIsConstructed())
			{
				LinaString skyboxShaderText;
				LinaEngine::Internal::LoadTextFileWithIncludes(skyboxShaderText, ResourceConstants::shaderFolderPath + "skyboxCubemap.glsl", "#include");

				m_SkyboxTexture = LoadCubemapTextureResource(
					ResourceConstants::textureFolderPath + "defaultSkybox/right.png",
					ResourceConstants::textureFolderPath + "defaultSkybox/left.png",
					ResourceConstants::textureFolderPath + "defaultSkybox/up.png",
					ResourceConstants::textureFolderPath + "defaultSkybox/down.png",
					ResourceConstants::textureFolderPath + "defaultSkybox/front.png",
					ResourceConstants::textureFolderPath + "defaultSkybox/back.png");

				ConstructShader(m_SkyboxCubemapShader, m_SkyboxTexture, m_SkyboxSampler, skyboxShaderText, 0);

			}

			break;
		}
		
		// Set the type.
		m_SkyboxType = type;


	}
}
