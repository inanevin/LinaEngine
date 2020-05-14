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

#include "Rendering/RenderEngine.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/ECS.hpp"
#include "Utility/Math/Color.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Core/Layer.hpp"



namespace LinaEngine::Graphics
{

	constexpr size_t UNIFORMBUFFER_GLOBALMATRIX_SIZE = sizeof(Matrix) * 2 + (sizeof(Vector4F) * 4);
	constexpr int UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT = 0;
	constexpr auto UNIFORMBUFFER_GLOBALMATRIX_NAME = "GlobalData";

	constexpr size_t UNIFORMBUFFER_LIGHTS_SIZE = (sizeof(int) * 8);
	constexpr int UNIFORMBUFFER_LIGHTS_BINDPOINT = 1;
	constexpr auto UNIFORMBUFFER_LIGHTS_NAME = "GlobalLightData";

	RenderEngine::RenderEngine()
	{
		LINA_CORE_TRACE("[Constructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	RenderEngine::~RenderEngine()
	{

		// Dump the remaining memory.
		DumpMemory();

		// Release Vertex Array Objects
		m_SkyboxVAO = m_RenderDevice.ReleaseVertexArray(m_SkyboxVAO);

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(LinaEngine::ECS::ECSRegistry& ecsReg)
	{

		// Initialize the render device.
		m_RenderDevice.Initialize(m_LightingSystem, m_MainWindow.GetWidth(), m_MainWindow.GetHeight());

		// Construct the uniform buffer for global matrices.
		m_GlobalDataBuffer.Construct(m_RenderDevice, UNIFORMBUFFER_GLOBALMATRIX_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_GlobalDataBuffer.Bind(UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT);

		// Construct the uniform buffer for lights.
		m_GlobalLightBuffer.Construct(m_RenderDevice, UNIFORMBUFFER_LIGHTS_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_GlobalLightBuffer.Bind(UNIFORMBUFFER_LIGHTS_BINDPOINT);

		// Initialize the engine shaders.
		ConstructEngineShaders();

		// Initialize the render target.
		m_RenderTarget.Construct(m_RenderDevice);

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

		// Create a default texture for render context.
		Texture* text;
		CreateTexture("defaultDiffuse", "resources/textures/defaultDiffuse.png", PixelFormat::FORMAT_RGB, true, false, SamplerData(), &text);

		// Initialize the render context.
		m_DefaultRenderContext.Construct(m_RenderDevice, m_RenderTarget, m_DefaultDrawParams, *text, m_LightingSystem);

		// Initialize skybox vertex array object.
		m_SkyboxVAO = m_RenderDevice.CreateSkyboxVertexArray();

		// Initialize ECS Camera System.
		Vector2F windowSize = Vector2F(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		m_CameraSystem.Construct(ecsReg, m_DefaultRenderContext);
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());

		// Initialize ECS Mesh Renderer System
		m_MeshRendererSystem.Construct(ecsReg, m_DefaultRenderContext);

		// Add the ECS systems into the pipeline.
		m_RenderingPipeline.AddSystem(m_CameraSystem);
		m_RenderingPipeline.AddSystem(m_MeshRendererSystem);
	
	}


	void RenderEngine::Tick(float delta)
	{
		// Clear color.
		m_DefaultRenderContext.Clear(m_CameraSystem.GetCurrentClearColor(), true);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene.
		m_DefaultRenderContext.Flush();

		// Draw skybox.
		DrawSkybox();

		// Draw GUI Layers
		for (Layer* layer : m_GUILayerStack)
			layer->OnUpdate();

		// Update window.
		m_MainWindow.Tick();

	}

	void RenderEngine::OnWindowResized(float width, float height)
	{
		// Propogate to render device.
		m_RenderDevice.OnWindowResized(width, height);

		// Update camera system's projection matrix.
		Vector2F windowSize = Vector2F(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		m_CameraSystem.SetAspectRatio(windowSize.GetX() / windowSize.GetY());
	}

	void RenderEngine::CreateMaterial(const std::string & materialName, const std::string & shaderName, Material * *refPointer)
	{
		if (!MaterialExists(materialName))
		{
			// Create material & set it's shader.
			SetMaterialShader(m_LoadedMaterials[materialName], shaderName);

			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedMaterials[materialName];
		}
		else
		{
			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedMaterials[materialName];

			// Abort if material exists.
			LINA_CORE_ERR("Material with the name {0} already exists, aborting...", materialName);
			return;
		}
	}

	void RenderEngine::CreateTexture(const std::string & textureName, const std::string & filePath, PixelFormat pixelFormat, bool generateMipmaps, bool compress, SamplerData samplerData, Texture** refPointer)
	{
		if (!TextureExists(textureName))
		{
			// Create pixel data.
			ArrayBitmap* textureBitmap = new ArrayBitmap();
			textureBitmap->Load(filePath);

			// Create texture & construct.
			m_LoadedTextures[textureName].Construct(m_RenderDevice, *textureBitmap, pixelFormat, generateMipmaps, compress, samplerData);

			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedTextures[textureName];

			// Delete pixel data.
			delete textureBitmap;
		}
		else
		{
			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedTextures[textureName];

			// Texture with this name already exists!
			LINA_CORE_ERR("Texture with the name {0} already exists, aborting...", textureName);
			return;
		}
	}

	void RenderEngine::CreateTexture(const std::string & textureName, const std::string filePaths[6], PixelFormat pixelFormat, bool generateMipmaps, bool compress,SamplerData samplerData, Texture** refPointer)
	{
		if (!TextureExists(textureName))
		{
			LinaArray<ArrayBitmap*> bitmaps;

			// Load bitmap for each face.
			for (uint32 i = 0; i < 6; i++)
			{
				ArrayBitmap* faceBitmap = new ArrayBitmap();
				faceBitmap->Load(filePaths[i]);
				bitmaps.push_back(faceBitmap);
			}

			// Create texture & construct.
			m_LoadedTextures[textureName].Construct(m_RenderDevice, bitmaps, pixelFormat, generateMipmaps, compress, samplerData);

			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedTextures[textureName];

			// Delete pixel data.
			for (uint32 i = 0; i < bitmaps.size(); i++)
				delete bitmaps[i];

			bitmaps.clear();
		}
		else
		{
			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedTextures[textureName];

			// Texture with this name already exists!
			LINA_CORE_ERR("Texture with the name {0} already exists, aborting...", textureName);
			return;
		}
	}

	void RenderEngine::CreateMesh(const std::string & meshName, const std::string & filePath, Mesh * *refPointer)
	{
		if (!MeshExists(meshName))
		{

			// Create object data & feed it from model.
			m_ModelLoader.LoadModels(filePath, m_LoadedMeshes[meshName].GetIndexedModels(), m_LoadedMeshes[meshName].GetMaterialIndices(), m_LoadedMeshes[meshName].GetMaterialSpecs());

			if (m_LoadedMeshes[meshName].GetIndexedModels().size() == 0)
				LINA_CORE_ERR("Indexed model array is empty! The model with the name: {0} could not be found or model scene does not contain any mesh! This will cause undefined behaviour or crashes if it is assigned to a ECS MeshRendererComponent."
					, filePath);

			// Create vertex array for each mesh.
			for (uint32 i = 0; i < m_LoadedMeshes[meshName].GetIndexedModels().size(); i++)
			{
				VertexArray* vertexArray = new VertexArray();
				vertexArray->Construct(m_RenderDevice, m_LoadedMeshes[meshName].GetIndexedModels()[i], BufferUsage::USAGE_STATIC_DRAW);
				m_LoadedMeshes[meshName].GetVertexArrays().push_back(vertexArray);
			}


			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedMeshes[meshName];

		}
		else
		{
			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedMeshes[meshName];

			// Mesh with this name already exists!
			LINA_CORE_ERR("Mesh with the name {0} already exists, aborting...", meshName);
			return;
		}

	}

	void RenderEngine::CreateShader(const std::string & shaderName, const std::string & shaderText, Shader * *refPointer)
	{
		if (!ShaderExists(shaderName))
		{
			// Create shader
			m_LoadedShaders[shaderName].Construct(m_RenderDevice, shaderText);
			
			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedShaders[shaderName];
		}
		else
		{

			// Set pointer that was sent in.
			if (refPointer != nullptr)
				* refPointer = &m_LoadedShaders[shaderName];

			// Shader with this name already exists!
			LINA_CORE_ERR("Shader with the name {0} already exists, aborting...", shaderName);
			return;
		}
	}


	Material& RenderEngine::GetMaterial(const std::string & materialName)
	{
		if (!MaterialExists(materialName))
		{
			// Mesh not found.
			LINA_CORE_ERR("Material with the name {0} was not found, returning dummy object", materialName);
			return m_DummyMaterial;
		}

		return m_LoadedMaterials.at(materialName);
	}

	Texture& RenderEngine::GetTexture(const std::string & textureName)
	{
		if (!TextureExists(textureName))
		{
			// Mesh not found.
			LINA_CORE_ERR("Texture with the name {0} was not found, returning dummy object", textureName);
			return m_DummyTexture;
		}

		return m_LoadedTextures[textureName];
	}

	Mesh& RenderEngine::GetMesh(const std::string & meshName)
	{
		if (!MeshExists(meshName))
		{
			// Mesh not found.
			LINA_CORE_ERR("Mesh with the name {0} was not found, returning dummy object", meshName);
			return m_DummyMesh;
		}

		return m_LoadedMeshes[meshName];
	}

	Shader& RenderEngine::GetShader(const std::string & shaderName)
	{
		if (!ShaderExists(shaderName))
		{
			// Shader not found.
			LINA_CORE_ERR("Shader with the name {0} was not found, returning standardLitShader", shaderName);
			return GetShader(ShaderConstants::standardLitShader);
		}

		return m_LoadedShaders[shaderName];
	}

	void RenderEngine::UnloadTextureResource(const std::string & textureName)
	{
		if (!TextureExists(textureName))
		{
			LINA_CORE_ERR("Texture not found! Aborting... ");
			return;
		}

		m_LoadedTextures.erase(textureName);
	}

	void RenderEngine::UnloadMeshResource(const std::string & meshName)
	{
		if (!MeshExists(meshName))
		{
			LINA_CORE_ERR("Mesh not found! Aborting... ");
			return;
		}

		m_LoadedMeshes.erase(meshName);
	}

	void RenderEngine::UnloadMaterialResource(const std::string & materialName)
	{
		if (!MaterialExists(materialName))
		{
			LINA_CORE_ERR("Material not found! Aborting... ");
			return;
		}

		m_LoadedMaterials.erase(materialName);
	}

	bool RenderEngine::MaterialExists(const std::string & materialName)
	{
		return !(m_LoadedMaterials.find(materialName) == m_LoadedMaterials.end());
	}

	bool RenderEngine::TextureExists(const std::string & textureName)
	{
		return !(m_LoadedTextures.find(textureName) == m_LoadedTextures.end());
	}

	bool RenderEngine::MeshExists(const std::string & meshName)
	{
		return !(m_LoadedMeshes.find(meshName) == m_LoadedMeshes.end());
	}

	bool RenderEngine::ShaderExists(const std::string & shaderName)
	{
		return !(m_LoadedShaders.find(shaderName) == m_LoadedShaders.end());
	}


	void RenderEngine::ConstructEngineShaders()
	{
		std::string shaderText;

		// Unlit.
		Utility::LoadTextFileWithIncludes(shaderText, "resources/shaders/basicStandardUnlit.glsl", "#include");
		CreateShader(ShaderConstants::standardUnlitShader, shaderText);
		GetShader(ShaderConstants::standardUnlitShader).BindBlockToBuffer(UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT, UNIFORMBUFFER_GLOBALMATRIX_NAME);

		// Lit.
		Utility::LoadTextFileWithIncludes(shaderText, "resources/shaders/basicStandardLit.glsl", "#include");
		CreateShader(ShaderConstants::standardLitShader, shaderText);
		GetShader(ShaderConstants::standardLitShader).BindBlockToBuffer(UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT, UNIFORMBUFFER_GLOBALMATRIX_NAME);
		GetShader(ShaderConstants::standardLitShader).BindBlockToBuffer(UNIFORMBUFFER_LIGHTS_BINDPOINT, UNIFORMBUFFER_LIGHTS_NAME);

		// Skybox single color.
		Utility::LoadTextFileWithIncludes(shaderText, "resources/shaders/skyboxSingleColor.glsl", "#include");
		CreateShader(ShaderConstants::skyboxSingleColorShader, shaderText);

		// Skybox vertex gradient color.
		Utility::LoadTextFileWithIncludes(shaderText, "resources/shaders/skyboxVertexGradient.glsl", "#include");
		CreateShader(ShaderConstants::skyboxGradientShader, shaderText);
		GetShader(ShaderConstants::skyboxGradientShader).BindBlockToBuffer(UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT, UNIFORMBUFFER_GLOBALMATRIX_NAME);

		// Skybox cubemap.
		Utility::LoadTextFileWithIncludes(shaderText, "resources/shaders/skyboxCubemap.glsl", "#include");
		CreateShader(ShaderConstants::skyboxCubemapShader, shaderText);
		GetShader(ShaderConstants::skyboxCubemapShader).BindBlockToBuffer(UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT, UNIFORMBUFFER_GLOBALMATRIX_NAME);

		// Skybox procedural.
		Utility::LoadTextFileWithIncludes(shaderText, "resources/shaders/skyboxProcedural.glsl", "#include");
		CreateShader(ShaderConstants::skyboxProceduralShader, shaderText);
		GetShader(ShaderConstants::skyboxProceduralShader).BindBlockToBuffer(UNIFORMBUFFER_GLOBALMATRIX_BINDPOINT, UNIFORMBUFFER_GLOBALMATRIX_NAME);


	}

	void RenderEngine::DumpMemory()
	{
		// Clear dumps.
		m_LoadedMeshes.clear();
		m_LoadedTextures.clear();
		m_LoadedMaterials.clear();
	}

	void RenderEngine::DrawSkybox()
	{
		if (m_SkyboxMaterial != nullptr)
		{
			m_DefaultRenderContext.UpdateShaderData(m_SkyboxMaterial);
			m_DefaultRenderContext.Draw(m_SkyboxVAO, m_SkyboxDrawParams, 1, 36, true);
		}
	}


	void RenderEngine::UpdateUniformBuffers()
	{
		Vector3F cameraLocation = m_CameraSystem.GetCameraLocation();
		Vector4F viewPos = Vector4F(cameraLocation.GetX(), cameraLocation.GetY(), cameraLocation.GetZ(), 1.0f);

		// Update global matrix buffer
		m_GlobalDataBuffer.Update(&m_CameraSystem.GetProjectionMatrix(), 0, sizeof(Matrix));
		m_GlobalDataBuffer.Update(&m_CameraSystem.GetViewMatrix(), sizeof(Matrix), sizeof(Matrix));
		m_GlobalDataBuffer.Update(&viewPos, sizeof(Matrix) * 2, sizeof(Vector4F));

		// Update lights buffer.
	
		int lightCount = 1;
		int spotLightCount = 1;
		m_GlobalLightBuffer.Update(&lightCount, 0, sizeof(int));
		m_GlobalLightBuffer.Update(&spotLightCount, sizeof(int), sizeof(int));


		//m_LightsBuffer.Update(&color, sizeof(float) * 12, sizeof(float) * 4);
		//m_LightsBuffer.Update(&direction, sizeof(float) * 16, sizeof(float) * 4);
	}

	void RenderEngine::SetMaterialShader(Material & material, const std::string & shaderName)
	{
		std::string usedName = shaderName;

		// If no shader found, fall back to standardLit
		if (m_LoadedShaders.find(shaderName) == m_LoadedShaders.end()) {
			LINA_CORE_ERR("Shader with the name {0} was not found. Setting material's shader to standardLit.", shaderName);
			material.shaderID = m_LoadedShaders[ShaderConstants::standardLitShader].GetID();
			usedName = ShaderConstants::standardLitShader;
		}
		else
			material.shaderID = m_LoadedShaders[shaderName].GetID();

		// Clear all shader related material data.
		material.textures.clear();
		material.colors.clear();
		material.floats.clear();
		material.ints.clear();
		material.vector3s.clear();
		material.vector2s.clear();
		material.matrices.clear();
		material.vector4s.clear();

		// Set shader data for material based on it's shader.
		if (usedName == ShaderConstants::standardLitShader)
		{
			material.colors[MaterialConstants::objectColorProperty] = Colors::White;
			material.floats[MaterialConstants::specularIntensityProperty] = 2.0f;
			material.samplers[MaterialConstants::diffuseTextureProperty] = 0;
			material.samplers[MaterialConstants::specularTextureProperty] = 1;
			material.ints[MaterialConstants::specularExponentProperty] = 32;
			material.receivesLighting = true;

		}
		else if (usedName == ShaderConstants::standardUnlitShader)
		{
			material.colors[MaterialConstants::objectColorProperty] = Colors::White;
			material.samplers[MaterialConstants::diffuseTextureProperty] = 0;
		}
		else if (usedName == ShaderConstants::skyboxSingleColorShader)
		{
			material.colors[MaterialConstants::colorProperty] = Colors::White;
		}
		else if (usedName == ShaderConstants::skyboxGradientShader)
		{
			material.colors[MaterialConstants::startColorProperty] = Colors::Black;
			material.colors[MaterialConstants::endColorProperty] = Colors::White;
		}
		else if (usedName == ShaderConstants::skyboxProceduralShader)
		{
			material.colors[MaterialConstants::startColorProperty] = Colors::Black;
			material.colors[MaterialConstants::endColorProperty] = Colors::White;
			material.vector3s[MaterialConstants::sunDirectionProperty] = Vector3F(0, -1, 0);
		}
		else if (usedName == ShaderConstants::skyboxCubemapShader)
		{
			material.samplers[MaterialConstants::diffuseTextureProperty] = 0;
		}
	}

	void RenderEngine::SetSkyboxMaterial(const std::string & materialName)
	{
		if (MaterialExists(materialName))
			SetSkyboxMaterial(m_LoadedMaterials[materialName]);
		else
			LINA_CORE_ERR("Material with the name {0} does not exists! Aborting...", materialName);
	}

	void RenderEngine::PushLayer(Layer* layer)
	{
		m_GUILayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	void RenderEngine::PushOverlay(Layer* layer)
	{
		m_GUILayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

}
