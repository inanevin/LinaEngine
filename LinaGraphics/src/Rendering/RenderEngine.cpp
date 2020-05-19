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
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/ECS.hpp"
#include "Utility/Math/Color.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "PackageManager/OpenGL/GLRenderDevice.hpp"
#include "Core/Layer.hpp"


namespace LinaEngine::Graphics
{

	constexpr size_t UNIFORMBUFFER_VIEWDATA_SIZE = sizeof(Matrix) * 2 + (sizeof(Vector4) * 4) + (sizeof(float) * 2);
	constexpr int UNIFORMBUFFER_VIEWDATA_BINDPOINT = 0;
	constexpr auto UNIFORMBUFFER_VIEWDATA_NAME = "ViewData";

	constexpr size_t UNIFORMBUFFER_LIGHTDATA_SIZE = (sizeof(int) * 8);
	constexpr int UNIFORMBUFFER_LIGHTDATA_BINDPOINT = 1;
	constexpr auto UNIFORMBUFFER_LIGHTDATA_NAME = "LightData";

	constexpr size_t UNIFORMBUFFER_DEBUGDATA_SIZE = (sizeof(bool) * 1);
	constexpr int UNIFORMBUFFER_DEBUGDATA_BINDPOINT = 2;
	constexpr auto UNIFORMBUFFER_DEBUGDATA_NAME = "DebugData";

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
		m_ScreenQuad = m_RenderDevice.ReleaseVertexArray(m_ScreenQuad);

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(LinaEngine::ECS::ECSRegistry& ecsReg)
	{
		// Initialize the render device.
		m_RenderDevice.Initialize(m_LightingSystem, m_MainWindow.GetWidth(), m_MainWindow.GetHeight());

		// Construct the uniform buffer for global matrices.
		m_GlobalDataBuffer.Construct(m_RenderDevice, UNIFORMBUFFER_VIEWDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_GlobalDataBuffer.Bind(UNIFORMBUFFER_VIEWDATA_BINDPOINT);

		// Construct the uniform buffer for lights.
		m_GlobalLightBuffer.Construct(m_RenderDevice, UNIFORMBUFFER_LIGHTDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_GlobalLightBuffer.Bind(UNIFORMBUFFER_LIGHTDATA_BINDPOINT);

		// Construct the uniform buffer for debugging.
		m_GlobalDebugBuffer.Construct(m_RenderDevice, UNIFORMBUFFER_DEBUGDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_GlobalDebugBuffer.Bind(UNIFORMBUFFER_DEBUGDATA_BINDPOINT);

		// Initialize the engine shaders.
		ConstructEngineShaders();

		// Initialize engine materials
		ConstructEngineMaterials();

		// Initialize engine vertex arrays.
		ConstructEnginePrimitives();

		// Initialize frame buffer texture.
		m_FrameBufferTexture.Construct(m_RenderDevice, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), PixelFormat::FORMAT_RGB, false, false, { FILTER_LINEAR, FILTER_LINEAR, WRAP_REPEAT, WRAP_REPEAT });

		// Construct screen quad.
		m_ScreenQuad = m_RenderDevice.CreateScreenQuadVertexArray();

		// Construct screen quad material.
		SetMaterialShader(m_ScreenQuadMaterial, Shaders::SCREEN_QUAD);

		// Initialize render buffer.
		m_RenderBuffer.Construct(m_RenderDevice, RenderBufferStorage::STORAGE_DEPTH24_STENCIL8, m_MainWindow.GetWidth(), m_MainWindow.GetHeight());

		// Initialize the render target.
		m_RenderTarget.Construct(m_RenderDevice, m_FrameBufferTexture, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH_AND_STENCIL, m_RenderBuffer.GetID());;

		// Setup draw parameters.
		SetupDrawParameters();

		// Create a default texture for render context.
		m_DefaultTexture = &CreateTexture("resources/textures/defaultDiffuse.png", PixelFormat::FORMAT_RGB, true, false, SamplerData());

		// Initialize built-in vertex array objects.
		m_SkyboxVAO = m_RenderDevice.CreateSkyboxVertexArray();

		// Initialize ECS Camera System.
		Vector2 windowSize = Vector2(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		m_CameraSystem.Construct(ecsReg);
		m_CameraSystem.SetAspectRatio(windowSize.x / windowSize.y);

		// Initialize ECS Mesh Renderer System
		m_MeshRendererSystem.Construct(ecsReg, *this, m_RenderDevice);

		// Initialize ECS Lighting system.
		m_LightingSystem.Construct(ecsReg, m_RenderDevice, *this);

		// Add the ECS systems into the pipeline.
		m_RenderingPipeline.AddSystem(m_CameraSystem);
		m_RenderingPipeline.AddSystem(m_MeshRendererSystem);
		m_RenderingPipeline.AddSystem(m_LightingSystem);

		// Set debug values.
		m_DebugData.visualizeDepth = false;
	}

	void RenderEngine::Tick(float delta)
	{

		//m_RenderDevice.SetFBO(0);
		m_DefaultDrawParams.useDepthTest = true;
		m_DefaultDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_DefaultDrawParams.destBlend = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		m_RenderDevice.SetFBO(m_RenderTarget.GetID());
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;

		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Draw skybox.
		DrawSkybox();

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(true, m_RenderTarget.GetID());

		// Draw screen quad.
		m_RenderDevice.SetFBO(0);
		
		m_DefaultDrawParams.useDepthTest = false;
		m_DefaultDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_NONE;
		m_DefaultDrawParams.destBlend = BlendFunc::BLEND_FUNC_NONE;
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_NONE;
		m_RenderDevice.Clear(true, false, false, Color::White, 0xFF);
		m_ScreenQuadMaterial.SetTexture(UF_SCREENTEXTURE, &m_FrameBufferTexture, 0);
		UpdateShaderData(&m_ScreenQuadMaterial);
		m_RenderDevice.Draw(m_ScreenQuad, m_DefaultDrawParams, 0, 6, true);

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
		Vector2 windowSize = Vector2(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		m_CameraSystem.SetAspectRatio(windowSize.x / windowSize.y);
	}

	Material& RenderEngine::CreateMaterial(const std::string& materialName, Shaders shader)
	{
		if (!MaterialExists(materialName))
		{
			// Create material & set it's shader.
			return SetMaterialShader(m_LoadedMaterials[materialName], shader);
		}
		else
		{
			// Abort if material exists.
			LINA_CORE_ERR("Material with the name {0} already exists, returning that...", materialName);
			return m_LoadedMaterials[materialName];
		}
	}

	Texture& RenderEngine::CreateTexture(const std::string& filePath, PixelFormat pixelFormat, bool generateMipmaps, bool compress, SamplerData samplerData)
	{
		if (!TextureExists(filePath))
		{
			// Create pixel data.
			ArrayBitmap* textureBitmap = new ArrayBitmap();
			textureBitmap->Load(filePath);

			// Create texture & construct.
			m_LoadedTextures[filePath].Construct(m_RenderDevice, *textureBitmap, pixelFormat, generateMipmaps, compress, samplerData);

			// Delete pixel data.
			delete textureBitmap;

			// Return
			return m_LoadedTextures[filePath];
		}
		else
		{
			// Texture with this name already exists!
			LINA_CORE_ERR("Texture with the path {0} already exists, returning that...", filePath);
			return m_LoadedTextures[filePath];
		}
	}

	Texture& RenderEngine::CreateTexture(const std::string filePaths[6], PixelFormat pixelFormat, bool generateMipmaps, bool compress, SamplerData samplerData)
	{
		if (!TextureExists(filePaths[0]))
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
			Texture& texture = m_LoadedTextures[filePaths[0]].Construct(m_RenderDevice, bitmaps, pixelFormat, generateMipmaps, compress, samplerData);

			// Delete pixel data.
			for (uint32 i = 0; i < bitmaps.size(); i++)
				delete bitmaps[i];

			// Clear list.
			bitmaps.clear();

			// Return
			return texture;
		}
		else
		{
			// Texture with this name already exists!
			LINA_CORE_ERR("Texture with the path {0} already exists, returning that...", filePaths[0]);
			return m_LoadedTextures[filePaths[0]];
		}
	}

	Mesh& RenderEngine::CreateMesh(const std::string& filePath)
	{
		if (!MeshExists(filePath))
		{

			// Create object data & feed it from model.
			Mesh& mesh = m_LoadedMeshes[filePath];
			m_ModelLoader.LoadModels(filePath, mesh.GetIndexedModels(), mesh.GetMaterialIndices(), mesh.GetMaterialSpecs());

			if (mesh.GetIndexedModels().size() == 0)
			{
				LINA_CORE_ERR("Indexed model array is empty! The model with the name: {0} could not be found or model scene does not contain any mesh! Returning plane quad...", filePath);
				UnloadMeshResource(filePath);
				return GetPrimitive(Primitives::PLANE);
			}

			// Create vertex array for each mesh.
			for (uint32 i = 0; i < mesh.GetIndexedModels().size(); i++)
			{
				VertexArray* vertexArray = new VertexArray();
				vertexArray->Construct(m_RenderDevice, mesh.GetIndexedModels()[i], BufferUsage::USAGE_STATIC_COPY);
				mesh.GetVertexArrays().push_back(vertexArray);
			}

			// Return
			return m_LoadedMeshes[filePath];
		}
		else
		{
			// Mesh with this name already exists!
			LINA_CORE_ERR("Mesh with the name {0} already exists, returning that...", filePath);
			return m_LoadedMeshes[filePath];
		}

	}

	Mesh& RenderEngine::CreatePrimitive(Primitives primitive, const std::string& path)
	{
		if (!PrimitiveExists(primitive))
		{

			// Create object data & feed it from model.
			Mesh& mesh = m_LoadedPrimitives[primitive];

			m_ModelLoader.LoadModels(path, mesh.GetIndexedModels(), mesh.GetMaterialIndices(), mesh.GetMaterialSpecs());

			if (mesh.GetIndexedModels().size() == 0)
			{
				LINA_CORE_ERR("Indexed model array is empty! Primitive {0} could not be loaded, returning empty mesh", primitive);
				return Mesh();
			}

			// Create vertex array for each mesh.
			for (uint32 i = 0; i < mesh.GetIndexedModels().size(); i++)
			{
				VertexArray* vertexArray = new VertexArray();
				vertexArray->Construct(m_RenderDevice, mesh.GetIndexedModels()[i], BufferUsage::USAGE_STATIC_COPY);
				mesh.GetVertexArrays().push_back(vertexArray);
			}

			// Return
			return m_LoadedPrimitives[primitive];
		}
		else
		{
			// Mesh with this name already exists!
			LINA_CORE_ERR("Primitive with the ID{0} already exists, returning that...", primitive);
			return m_LoadedPrimitives[primitive];
		}
	}

	Shader& RenderEngine::CreateShader(Shaders shader, const std::string& path)
	{
		// Create shader
		if (!ShaderExists(shader))
		{
			std::string shaderText;
			Utility::LoadTextFileWithIncludes(shaderText, path, "#include");
			return m_LoadedShaders[shader].Construct(m_RenderDevice, shaderText);
		}
		else
		{
			// Shader with this name already exists!
			LINA_CORE_WARN("Shader with the id {0} already exists, returning that...", shader);
			return m_LoadedShaders[shader];
		}
	}


	Material& RenderEngine::GetMaterial(const std::string& materialName)
	{
		if (!MaterialExists(materialName))
		{
			// Mesh not found.
			LINA_CORE_ERR("Material with the name {0} was not found, returning un-constructed material...", materialName);
			return Material();
		}

		return m_LoadedMaterials[materialName];
	}

	Texture& RenderEngine::GetTexture(const std::string& textureName)
	{
		if (!TextureExists(textureName))
		{
			// Mesh not found.
			LINA_CORE_ERR("Texture with the name {0} was not found, returning un-constructed texture...", textureName);
			return Texture();
		}

		return m_LoadedTextures[textureName];
	}

	Mesh& RenderEngine::GetMesh(const std::string& meshName)
	{
		if (!MeshExists(meshName))
		{
			// Mesh not found.
			LINA_CORE_ERR("Mesh with the name {0} was not found, returning un-constructed mesh...", meshName);
			return Mesh();
		}

		return m_LoadedMeshes[meshName];
	}

	Shader& RenderEngine::GetShader(Shaders shader)
	{
		if (!ShaderExists(shader))
		{
			// Shader not found.
			LINA_CORE_ERR("Shader with the ID {0} was not found, returning standardLitShader", shader);
			return GetShader(Shaders::STANDARD_LIT);
		}

		return m_LoadedShaders[shader];
	}

	Mesh& RenderEngine::GetPrimitive(Primitives primitive)
	{
		if (!PrimitiveExists(primitive))
		{
			// VA not found.
			LINA_CORE_ERR("Primitive with the ID {0} was not found, returning plane...", primitive);
			return GetPrimitive(Primitives::PLANE);
		}
		else
			return m_LoadedPrimitives[primitive];
	}

	void RenderEngine::UnloadTextureResource(const std::string& textureName)
	{
		if (!TextureExists(textureName))
		{
			LINA_CORE_ERR("Texture not found! Aborting... ");
			return;
		}

		m_LoadedTextures.erase(textureName);
	}

	void RenderEngine::UnloadMeshResource(const std::string& meshName)
	{
		if (!MeshExists(meshName))
		{
			LINA_CORE_ERR("Mesh not found! Aborting... ");
			return;
		}

		m_LoadedMeshes.erase(meshName);
	}

	void RenderEngine::UnloadMaterialResource(const std::string& materialName)
	{
		if (!MaterialExists(materialName))
		{
			LINA_CORE_ERR("Material not found! Aborting... ");
			return;
		}

		m_LoadedMaterials.erase(materialName);
	}

	bool RenderEngine::MaterialExists(const std::string& materialName)
	{
		return !(m_LoadedMaterials.find(materialName) == m_LoadedMaterials.end());
	}

	bool RenderEngine::TextureExists(const std::string& textureName)
	{
		return !(m_LoadedTextures.find(textureName) == m_LoadedTextures.end());
	}

	bool RenderEngine::MeshExists(const std::string& meshName)
	{
		return !(m_LoadedMeshes.find(meshName) == m_LoadedMeshes.end());
	}

	bool RenderEngine::ShaderExists(Shaders shader)
	{
		return !(m_LoadedShaders.find(shader) == m_LoadedShaders.end());
	}

	bool RenderEngine::PrimitiveExists(Primitives primitive)
	{
		return !(m_LoadedPrimitives.find(primitive) == m_LoadedPrimitives.end());
	}


	void RenderEngine::ConstructEngineShaders()
	{
		// Unlit.
		Shader& unlit = CreateShader(Shaders::STANDARD_UNLIT, "resources/shaders/basicStandardUnlit.glsl");
		unlit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		unlit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		Shader& lit = CreateShader(Shaders::STANDARD_LIT, "resources/shaders/basicStandardLit.glsl");
		lit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		lit.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
		lit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Skies
		CreateShader(Shaders::SKYBOX_SINGLECOLOR, "resources/shaders/skyboxSingleColor.glsl");
		CreateShader(Shaders::SKYBOX_GRADIENT, "resources/shaders/skyboxVertexGradient.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_CUBEMAP, "resources/shaders/skyboxCubemap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_PROCEDURAL, "resources/shaders/skyboxProcedural.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Others
		Shader& singleColor = CreateShader(Shaders::STENCIL_OUTLINE, "resources/shaders/stencilOutline.glsl");
		singleColor.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		singleColor.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Screen Quad
		CreateShader(Shaders::SCREEN_QUAD, "resources/shaders/screenQuad.glsl");

	}

	void RenderEngine::ConstructEngineMaterials()
	{
		// Stencil outline material.
		CreateMaterial(MAT_LINASTENCILOUTLINE, Shaders::STENCIL_OUTLINE);
		m_LoadedMaterials[MAT_LINASTENCILOUTLINE].floats[MC_OUTLINETHICKNESS] = 0.015f;
		m_LoadedMaterials[MAT_LINASTENCILOUTLINE].colors[MC_OBJECTCOLORPROPERTY] = Color::Red;
	}

	void RenderEngine::ConstructEnginePrimitives()
	{
		// Primitives
		CreatePrimitive(Primitives::CUBE, "resources/meshes/primitives/cube.obj");
		CreatePrimitive(Primitives::CYLINDER, "resources/meshes/primitives/cylinder.obj");
		CreatePrimitive(Primitives::PLANE, "resources/meshes/primitives/plane.obj");
		CreatePrimitive(Primitives::SPHERE, "resources/meshes/primitives/sphere.obj");
		CreatePrimitive(Primitives::ICOSPHERE, "resources/meshes/primitives/icosphere.obj");
		CreatePrimitive(Primitives::CONE, "resources/meshes/primitives/cone.obj");
	}

	void RenderEngine::SetupDrawParameters()
	{
		// Set default drawing parameters.
		m_DefaultDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_DefaultDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_DefaultDrawParams.destBlend = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;		
		m_DefaultDrawParams.useDepthTest = true;
		m_DefaultDrawParams.shouldWriteDepth = true;
		m_DefaultDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;

		// Set render to fbo target draw parameters.	
		m_FBOTextureDrawParameters.useDepthTest = false;
		m_FBOTextureDrawParameters.sourceBlend = BlendFunc::BLEND_FUNC_NONE;
		m_FBOTextureDrawParameters.destBlend = BlendFunc::BLEND_FUNC_NONE;
		m_FBOTextureDrawParameters.faceCulling = FaceCulling::FACE_CULL_NONE;

		// Set skybox draw params.
		m_SkyboxDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_SkyboxDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_SkyboxDrawParams.useDepthTest = true;
		m_SkyboxDrawParams.shouldWriteDepth = true;
		m_SkyboxDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LEQUAL;
		m_SkyboxDrawParams.useStencilTest = true;
		m_SkyboxDrawParams.stencilWriteMask = 0xFF;
		m_SkyboxDrawParams.stencilTestMask = 0XFF;

		// Set stencil draw params first pass.		
		m_StencilOutlineDrawParams.useStencilTest = true;
		m_StencilOutlineDrawParams.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
		m_StencilOutlineDrawParams.stencilComparisonVal = 1;
		m_StencilOutlineDrawParams.stencilTestMask = 0xFF;
		m_StencilOutlineDrawParams.stencilWriteMask = 0xFF;
		
		// Set stencil draw params second first pass.		
		m_StencilOutlineDrawParams2.useStencilTest = true;
		m_StencilOutlineDrawParams2.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_NOT_EQUAL;
		m_StencilOutlineDrawParams2.stencilComparisonVal = 1;
		m_StencilOutlineDrawParams2.stencilTestMask = 0xFF;
		m_StencilOutlineDrawParams2.stencilWriteMask = 0x00;

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
			UpdateShaderData(m_SkyboxMaterial);
			m_RenderDevice.Draw(m_SkyboxVAO, m_SkyboxDrawParams, 1, 36, true);
		}
	}

	void RenderEngine::DrawSceneObjects(bool useStencilOutlining, uint32 fbo)
	{
		// Draw opaques.
		if (useStencilOutlining)
		{
			m_DefaultDrawParams.useStencilTest = true;
			m_DefaultDrawParams.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
			m_DefaultDrawParams.stencilComparisonVal = 1;
			m_DefaultDrawParams.stencilTestMask = 0xFF;
			m_DefaultDrawParams.stencilWriteMask = 0xFF;


			// Draw scene.
			m_MeshRendererSystem.FlushOpaque(m_DefaultDrawParams, nullptr, false);
			m_MeshRendererSystem.FlushTransparent( m_DefaultDrawParams, nullptr, false);

			// Set stencil draw params.
			m_DefaultDrawParams.stencilFunc = Graphics::DrawFunc::DRAW_FUNC_NOT_EQUAL;
			m_DefaultDrawParams.stencilComparisonVal = 1;
			m_DefaultDrawParams.stencilTestMask = 0xFF;
			m_DefaultDrawParams.stencilWriteMask = 0x00;
			m_RenderDevice.SetDepthTestEnable(false);

			// Draw scene.
			m_MeshRendererSystem.FlushOpaque(m_DefaultDrawParams, &m_LoadedMaterials[MAT_LINASTENCILOUTLINE], true);
			m_MeshRendererSystem.FlushTransparent(m_DefaultDrawParams, &m_LoadedMaterials[MAT_LINASTENCILOUTLINE], true);

			// Reset stencil.
			m_RenderDevice.SetStencilWriteMask(0xFF);
			m_RenderDevice.SetDepthTestEnable(true);
		}
		else
		{
			m_MeshRendererSystem.FlushOpaque(m_DefaultDrawParams, nullptr, true);
			m_MeshRendererSystem.FlushTransparent(m_DefaultDrawParams, nullptr, true);
		}



	}


	void RenderEngine::UpdateUniformBuffers()
	{
		Vector3 cameraLocation = m_CameraSystem.GetCameraLocation();
		Vector4 viewPos = Vector4(cameraLocation.x, cameraLocation.y, cameraLocation.z, 1.0f);

		// Update global matrix buffer
		uintptr currentGlobalDataOffset = 0;
		m_GlobalDataBuffer.Update(&m_CameraSystem.GetProjectionMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_GlobalDataBuffer.Update(&m_CameraSystem.GetViewMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_GlobalDataBuffer.Update(&viewPos, currentGlobalDataOffset, sizeof(Vector4));
		currentGlobalDataOffset += sizeof(Vector4);

		m_GlobalDataBuffer.Update(&m_CameraSystem.GetCurrentCameraComponent().zNear, currentGlobalDataOffset, sizeof(float));
		currentGlobalDataOffset += sizeof(float);

		m_GlobalDataBuffer.Update(&m_CameraSystem.GetCurrentCameraComponent().zFar, currentGlobalDataOffset, sizeof(float));
		currentGlobalDataOffset += sizeof(float);

		// Update lights buffer.
		m_GlobalLightBuffer.Update(&m_CurrentPointLightCount, 0, sizeof(int));
		m_GlobalLightBuffer.Update(&m_CurrentSpotLightCount, sizeof(int), sizeof(int));

		// Update debug fufer.
		m_GlobalDebugBuffer.Update(&m_DebugData.visualizeDepth, 0, sizeof(bool));
	}

	Material& RenderEngine::SetMaterialShader(Material& material, Shaders shader)
	{

		// If no shader found, fall back to standardLit
		if (m_LoadedShaders.find(shader) == m_LoadedShaders.end()) {
			LINA_CORE_ERR("Shader with engine ID {0} was not found. Setting material's shader to standardLit.", shader);
			material.shaderID = m_LoadedShaders[Shaders::STANDARD_LIT].GetID();
		}
		else
			material.shaderID = m_LoadedShaders[shader].GetID();

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
		if (shader == Shaders::STANDARD_LIT)
		{
			material.colors[MC_OBJECTCOLORPROPERTY] = Color::White;
			material.floats[MC_SPECULARINTENSITYPROPERTY] = 1.0f;
			material.samplers[MC_DIFFUSETEXTUREPROPERTY] = 0;
			material.samplers[MC_SPECULARTEXTUREPROPERTY] = 1;
			material.ints[MC_SPECULAREXPONENTPROPERTY] = 32;
			material.ints[MC_SURFACETYPE] = 0;
			material.receivesLighting = true;

		}
		else if (shader == Shaders::STANDARD_UNLIT)
		{
			material.colors[MC_OBJECTCOLORPROPERTY] = Color::White;
			material.samplers[MC_DIFFUSETEXTUREPROPERTY] = 0;
			material.ints[MC_SURFACETYPE] = 0;
		}
		else if (shader == Shaders::SKYBOX_SINGLECOLOR)
		{
			material.colors[MC_COLORPROPERTY] = Color::White;
		}
		else if (shader == Shaders::SKYBOX_GRADIENT)
		{
			material.colors[MC_STARTCOLORPROPERTY] = Color::Black;
			material.colors[MC_ENDCOLORPROPERTY] = Color::White;
		}
		else if (shader == Shaders::SKYBOX_PROCEDURAL)
		{
			material.colors[MC_STARTCOLORPROPERTY] = Color::Black;
			material.colors[MC_ENDCOLORPROPERTY] = Color::White;
			material.vector3s[MC_SUNDIRECTIONPROPERTY] = Vector3(0, -1, 0);
		}
		else if (shader == Shaders::SKYBOX_CUBEMAP)
		{
			material.samplers[MC_DIFFUSETEXTUREPROPERTY] = 0;
		}
		else if (shader == Shaders::STENCIL_OUTLINE)
		{
			material.colors[MC_OBJECTCOLORPROPERTY] = Color::White;
			material.floats[MC_OUTLINETHICKNESS] = 0.1f;
			material.ints[MC_SURFACETYPE] = 0;
		}
		else if (shader == Shaders::SCREEN_QUAD)
		{
			material.samplers[UF_SCREENTEXTURE] = 0;
		}


		return material;
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

	void RenderEngine::UpdateShaderData(Material* data)
	{

		m_RenderDevice.SetShader(data->GetShaderID());

		for (auto const& d : (*data).floats)
			m_RenderDevice.UpdateShaderUniformFloat(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).colors)
			m_RenderDevice.UpdateShaderUniformColor(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).ints)
			m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).vector2s)
			m_RenderDevice.UpdateShaderUniformVector2(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).vector3s)
			m_RenderDevice.UpdateShaderUniformVector3(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).vector4s)
			m_RenderDevice.UpdateShaderUniformVector4F(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).matrices)
			m_RenderDevice.UpdateShaderUniformMatrix(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).samplers)
		{

			if ((*data).textures.find(d.first) != (*data).textures.end())
			{
				m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first, d.second);

				MaterialTextureData* textureData = &(*data).textures[d.first];
				m_RenderDevice.SetTexture(textureData->texture->GetID(), textureData->texture->GetSamplerID(), textureData->unit, textureData->bindMode, true);
			}
			else
				m_RenderDevice.SetTexture(m_DefaultTexture->GetID(), 0, d.second);


		}


		if (data->receivesLighting)
			m_LightingSystem.SetLightingShaderData(data->GetShaderID());

	}

}