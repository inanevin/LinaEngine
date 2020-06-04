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

	constexpr size_t UNIFORMBUFFER_VIEWDATA_SIZE = (sizeof(Matrix) * 3) + (sizeof(Vector4)) + (sizeof(float) * 2);
	constexpr int UNIFORMBUFFER_VIEWDATA_BINDPOINT = 0;
	constexpr auto UNIFORMBUFFER_VIEWDATA_NAME = "ViewData";

	constexpr size_t UNIFORMBUFFER_LIGHTDATA_SIZE = (sizeof(int) * 2);
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
		m_ScreenQuadVAO = m_RenderDevice.ReleaseVertexArray(m_ScreenQuadVAO);

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(LinaEngine::ECS::ECSRegistry& ecsReg)
	{
		// Flip loaded images.
		ArrayBitmap::SetImageFlip(true);

		// Setup draw parameters.
		SetupDrawParameters();

		// Initialize the render device.
		m_RenderDevice.Initialize(m_LightingSystem, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), m_DefaultDrawParams);

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

		// Initialize built-in vertex array objects.
		m_SkyboxVAO = m_RenderDevice.CreateSkyboxVertexArray();
		m_HDRICubeVAO = m_RenderDevice.CreateHDRICubeVertexArray();
		m_ScreenQuadVAO = m_RenderDevice.CreateScreenQuadVertexArray();

		// Construct render targets
		ConstructRenderTargets();

		// Create default textures.
		m_DefaultTexture.ConstructEmpty(m_RenderDevice);
		m_DefaultCubemapTexture.ConstructRTCubemapTexture(m_RenderDevice, m_MainWindow.GetSize(), SamplerParameters());

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

		//DrawOperationsDefault(delta);
		//DrawOperationsMSAA(delta);
		//DrawOperationsPointLight(delta, false);


		//DrawOperationsShadows(delta,false);
		//for (std::set<Material*>::iterator it = m_ShadowMappedMaterials.begin(); it != m_ShadowMappedMaterials.end(); ++it)
			//(*it)->SetTexture(MAT_TEXTURE2D_SHADOWMAP, &m_DepthMapRTTexture);

	//	for (std::set<Material*>::iterator it = m_ShadowMappedMaterials.begin(); it != m_ShadowMappedMaterials.end(); ++it)
		//	(*it)->SetTexture(MAT_TEXTURE2D_SHADOWMAP, &m_PointLightsRTTexture, TextureBindMode::BINDTEXTURE_CUBEMAP);

		//DrawOperationsPrimaryRT(delta);
		//DrawOperationsDefault(delta);
		Render(delta);

		//DrawOperationsMSAA(delta);
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

	Texture& RenderEngine::CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams, bool compress, bool useDefaultFormats)
	{
		if (!TextureExists(filePath))
		{
			// Create pixel data.
			ArrayBitmap* textureBitmap = new ArrayBitmap();

			int nrComponents = textureBitmap->Load(filePath);
			if (nrComponents == -1)
			{
				LINA_CORE_ERR("Texture with the path {0} doesn't exist, returning empty texture", filePath);
				delete textureBitmap;
				return m_DefaultTexture;
			}

			if (useDefaultFormats)
			{
				if (nrComponents == 1)
					samplerParams.textureParams.internalPixelFormat = samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_R;
				if (nrComponents == 2)
					samplerParams.textureParams.internalPixelFormat = samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_RG;
				else if (nrComponents == 3)
					samplerParams.textureParams.internalPixelFormat = samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;
				else if (nrComponents == 4)
					samplerParams.textureParams.internalPixelFormat = samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGBA;

			}
			// Create texture & construct.
			m_LoadedTextures[filePath].Construct(m_RenderDevice, *textureBitmap, samplerParams, compress);

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

	Texture& RenderEngine::CreateTextureCubemap(const std::string filePaths[6], SamplerParameters samplerParams, bool compress)
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
			Texture& texture = m_LoadedTextures[filePaths[0]].ConstructCubemap(m_RenderDevice, samplerParams, bitmaps, compress);

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

	Texture& RenderEngine::CreateTextureHDRI(const std::string filePath)
	{
		if (!TextureExists(filePath))
		{
			// Create pixel data.
			int w, h, nrComponents;
			float* data = ArrayBitmap::LoadImmediateHDRI(filePath.c_str(), w, h, nrComponents);

			if (!data)
			{
				LINA_CORE_ERR("Texture with the path {0} doesn't exist, returning empty texture", filePath);
				return m_DefaultTexture;
			}


			// Create texture & construct.
			SamplerParameters samplerParams;
			samplerParams.textureParams.wrapR = samplerParams.textureParams.wrapS = samplerParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
			samplerParams.textureParams.minFilter = samplerParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
			samplerParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
			samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;
			m_LoadedTextures[filePath].ConstructHDRI(m_RenderDevice, samplerParams, Vector2(w, h), data);

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

	Mesh& RenderEngine::CreateMesh(const std::string& filePath)
	{
		if (!MeshExists(filePath))
		{

			// Create object data & feed it from model.
			Mesh& mesh = m_LoadedMeshes[filePath];
			m_ModelLoader.LoadModel(filePath, mesh.GetIndexedModels(), mesh.GetMaterialIndices(), mesh.GetMaterialSpecs());

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

			m_ModelLoader.LoadModel(path, mesh.GetIndexedModels(), mesh.GetMaterialIndices(), mesh.GetMaterialSpecs());

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

	Shader& RenderEngine::CreateShader(Shaders shader, const std::string& path, bool usesGeometryShader)
	{
		// Create shader
		if (!ShaderExists(shader))
		{
			std::string shaderText;
			Utility::LoadTextFileWithIncludes(shaderText, path, "#include");
			return m_LoadedShaders[shader].Construct(m_RenderDevice, shaderText, usesGeometryShader);
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
			LINA_CORE_ERR("Shader with the ID {0} was not found, returning standardUnlit Shader", shader);
			return GetShader(Shaders::STANDARD_UNLIT);
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

	Material& RenderEngine::SetMaterialShader(Material& material, Shaders shader)
	{

		// If no shader found, fall back to standardLit
		if (m_LoadedShaders.find(shader) == m_LoadedShaders.end()) {
			LINA_CORE_ERR("Shader with engine ID {0} was not found. Setting material's shader to standardUnlit.", shader);
			material.shaderID = m_LoadedShaders[Shaders::STANDARD_UNLIT].GetID();
		}
		else
			material.shaderID = m_LoadedShaders[shader].GetID();

		// Clear all shader related material data.
		material.sampler2Ds.clear();
		material.colors.clear();
		material.floats.clear();
		material.ints.clear();
		material.vector3s.clear();
		material.vector2s.clear();
		material.matrices.clear();
		material.vector4s.clear();


		 if (shader == Shaders::STANDARD_UNLIT)
		{
			material.colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.sampler2Ds[MAT_TEXTURE2D_DIFFUSE] = { 0 };
			material.ints[MAT_SURFACETYPE] = 0;
		}
		else if (shader == Shaders::SKYBOX_SINGLECOLOR)
		{
			material.colors[MAT_COLOR] = Color::White;
		}
		else if (shader == Shaders::SKYBOX_GRADIENT)
		{
			material.colors[MAT_STARTCOLOR] = Color::Black;
			material.colors[MAT_ENDCOLOR] = Color::White;
		}
		else if (shader == Shaders::SKYBOX_PROCEDURAL)
		{
			material.colors[MAT_STARTCOLOR] = Color::Black;
			material.colors[MAT_ENDCOLOR] = Color::White;
			material.vector3s[MAT_SUNDIRECTION] = Vector3(0, -1, 0);
		}
		else if (shader == Shaders::SKYBOX_CUBEMAP)
		{
			material.sampler2Ds[MAT_MAP_ENVIRONMENT] = { 0 };
		}
		else if (shader == Shaders::SKYBOX_HDRI)
		{
			material.sampler2Ds[MAT_MAP_ENVIRONMENT] = { 0 };
		}

		else if (shader == Shaders::SCREEN_QUAD_FINAL)
		{

			material.sampler2Ds[MAT_MAP_SCREEN] = { 0 };
			material.sampler2Ds[MAT_MAP_BLOOM] = { 1 };
			material.sampler2Ds[MAT_MAP_OUTLINE] = { 2 };
			material.floats[MAT_EXPOSURE] = 1.0f;
			material.floats[MAT_FXAAREDUCEMIN] = 1.0f / 128.0f;
			material.floats[MAT_FXAAREDUCEMUL] = 1.0f / 8.0f;
			material.floats[MAT_FXAASPANMAX] = 8.0f;
			material.booleans[MAT_BLOOMENABLED] = false;
			material.booleans[MAT_FXAAENABLED] = false;
			material.vector3s[MAT_INVERSESCREENMAPSIZE] = Vector3();
		}
		else if (shader == Shaders::SCREEN_QUAD_BLUR)
		{
			material.sampler2Ds[MAT_MAP_SCREEN] = { 0 };
			material.booleans[MAT_ISHORIZONTAL] = false;
		}
		else if (shader == Shaders::SCREEN_QUAD_OUTLINE)
		{
			material.sampler2Ds[MAT_MAP_SCREEN] = { 0 };
		}

		else if (shader == Shaders::PBR_LIT)
		{
			material.sampler2Ds[MAT_TEXTURE2D_ALBEDOMAP] = { 0 };
			material.sampler2Ds[MAT_TEXTURE2D_NORMALMAP] = { 1 };
			material.sampler2Ds[MAT_TEXTURE2D_ROUGHNESSMAP] = { 2 };
			material.sampler2Ds[MAT_TEXTURE2D_METALLICMAP] = { 3 };
			material.sampler2Ds[MAT_TEXTURE2D_AOMAP] = { 4 };
			material.sampler2Ds[MAT_TEXTURE2D_IRRADIANCEMAP] = { 5, nullptr, TextureBindMode::BINDTEXTURE_CUBEMAP, false };
			material.sampler2Ds[MAT_TEXTURE2D_PREFILTERMAP] = { 6,nullptr, TextureBindMode::BINDTEXTURE_CUBEMAP, false };
			material.sampler2Ds[MAT_TEXTURE2D_BRDFLUTMAP] = { 7 };
			material.floats[MAT_METALLICMULTIPLIER] = 1.0f;
			material.floats[MAT_ROUGHNESSMULTIPLIER] = 1.0f;
			material.ints[MAT_WORKFLOW] = 0;
			material.vector2s[MAT_TILING] = Vector2::One;
			material.receivesLighting = true;
			material.isShadowMapped = true;
			material.usesHDRI = true;
		}
		else if (shader == Shaders::EQUIRECTANGULAR_HDRI)
		{
			material.sampler2Ds[MAT_MAP_EQUIRECTANGULAR] = { 0 };
			material.matrices[UF_MATRIX_VIEW] = Matrix();
			material.matrices[UF_MATRIX_PROJECTION] = Matrix();
		}


		return material;
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

		// If its in the internal list, remove first.
		if (m_ShadowMappedMaterials.find(&m_LoadedMaterials[materialName]) != m_ShadowMappedMaterials.end())
			m_ShadowMappedMaterials.erase(&m_LoadedMaterials[materialName]);

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
		Shader& unlit = CreateShader(Shaders::STANDARD_UNLIT, "resources/shaders/Unlit/Unlit.glsl");
		unlit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		unlit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// PBR Lit
		Shader& pbrLit = CreateShader(Shaders::PBR_LIT, "resources/shaders/PBR/PBRLit.glsl", false);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Skies
		CreateShader(Shaders::SKYBOX_SINGLECOLOR, "resources/shaders/Skybox/SkyboxColor.glsl");
		CreateShader(Shaders::SKYBOX_GRADIENT, "resources/shaders/Skybox/SkyboxGradient.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_CUBEMAP, "resources/shaders/Skybox/SkyboxCubemap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_PROCEDURAL, "resources/shaders/Skybox/SkyboxProcedural.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_HDRI, "resources/shaders/Skybox/SkyboxHDRI.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);


		// Equirectangular cube & irradiance for HDRI skbox
		CreateShader(Shaders::EQUIRECTANGULAR_HDRI, "resources/shaders/HDRI/HDRIEquirectangular.glsl");
		CreateShader(Shaders::IRRADIANCE_HDRI, "resources/shaders/HDRI/HDRIIrradiance.glsl");
		CreateShader(Shaders::PREFILTER_HDRI, "resources/shaders/HDRI/HDRIPrefilter.glsl");
		CreateShader(Shaders::BRDF_HDRI, "resources/shaders/HDRI/HDRIBRDF.glsl");


		// Screen Quad Shaders
		CreateShader(Shaders::SCREEN_QUAD_FINAL, "resources/shaders/ScreenQuads/SQFinal.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SCREEN_QUAD_BLUR, "resources/shaders/ScreenQuads/SQBlur.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SCREEN_QUAD_OUTLINE, "resources/shaders/ScreenQuads/SQOutline.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

	}

	void RenderEngine::ConstructEngineMaterials()
	{
		SetMaterialShader(m_ScreenQuadFinalMaterial, Shaders::SCREEN_QUAD_FINAL);
		SetMaterialShader(m_ScreenQuadBlurMaterial, Shaders::SCREEN_QUAD_BLUR);
		SetMaterialShader(m_ScreenQuadOutlineMaterial, Shaders::SCREEN_QUAD_OUTLINE);
		SetMaterialShader(m_HDRIMaterial, Shaders::EQUIRECTANGULAR_HDRI);
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

	void RenderEngine::ConstructRenderTargets()
	{
		Vector2 screenSize = Vector2(m_MainWindow.GetWidth(), m_MainWindow.GetHeight());
		SamplerParameters mainRTParams;
		mainRTParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;
		mainRTParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
		mainRTParams.textureParams.minFilter = mainRTParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		mainRTParams.textureParams.wrapS = mainRTParams.textureParams.wrapT = SamplerWrapMode::WRAP_REPEAT;

		SamplerParameters primaryRTParams;
		primaryRTParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;
		primaryRTParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		primaryRTParams.textureParams.minFilter = primaryRTParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		primaryRTParams.textureParams.wrapS = primaryRTParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;


		SamplerParameters pingPongRTParams;
		pingPongRTParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;
		pingPongRTParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		pingPongRTParams.textureParams.minFilter = pingPongRTParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		pingPongRTParams.textureParams.wrapS = pingPongRTParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;


		// Initialize primary RT textures
		m_PrimaryRTTexture0.ConstructRTTexture(m_RenderDevice, screenSize, primaryRTParams, false);
		m_PrimaryRTTexture1.ConstructRTTexture(m_RenderDevice, screenSize, primaryRTParams, false);
		m_PrimaryRTTexture2.ConstructRTTexture(m_RenderDevice, screenSize, primaryRTParams, false);

		// Initialize ping pong rt texture
		m_PingPongRTTexture1.ConstructRTTexture(m_RenderDevice, screenSize, pingPongRTParams, false);
		m_PingPongRTTexture2.ConstructRTTexture(m_RenderDevice, screenSize, pingPongRTParams, false);

		// Initialize outilne RT texture
		m_OutlineRTTexture.ConstructRTTexture(m_RenderDevice, screenSize, primaryRTParams, false);

		// Initialize primary render buffer
		m_PrimaryRenderBuffer.Construct(m_RenderDevice, RenderBufferStorage::STORAGE_DEPTH, screenSize.x, screenSize.y);

		// Initialize hdri render buffer
		m_HDRICaptureRenderBuffer.Construct(m_RenderDevice, RenderBufferStorage::STORAGE_DEPTH_COMP24, m_HDRIResolution.x, m_HDRIResolution.y);

		// Initialize primary render target.
		m_PrimaryRenderTarget.Construct(m_RenderDevice, m_PrimaryRTTexture0, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_PrimaryRenderBuffer.GetID());

		// Bind the extre texture to primary render target, also tell open gl that we are running mrts.
		m_RenderDevice.BindTextureToRenderTarget(m_PrimaryRenderTarget.GetID(), m_PrimaryRTTexture1.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
		m_RenderDevice.BindTextureToRenderTarget(m_PrimaryRenderTarget.GetID(), m_PrimaryRTTexture2.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 2);
		uint32 attachments[3] = { FrameBufferAttachment::ATTACHMENT_COLOR , (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1),(FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)2) };
		m_RenderDevice.MultipleDrawBuffersCommand(m_PrimaryRenderTarget.GetID(), 3, attachments);

		// Initialize ping pong render targets
		m_PingPongRenderTarget1.Construct(m_RenderDevice, m_PingPongRTTexture1, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		m_PingPongRenderTarget2.Construct(m_RenderDevice, m_PingPongRTTexture2, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

		// Initialize outline render target
		m_OutlineRenderTarget.Construct(m_RenderDevice, m_OutlineRTTexture, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

		// Initialize HDRI render target
		m_HDRICaptureRenderTarget.Construct(m_RenderDevice, m_HDRIResolution, FrameBufferAttachment::ATTACHMENT_DEPTH, m_HDRICaptureRenderBuffer.GetID());

	}

	void RenderEngine::SetupDrawParameters()
	{
		// Set default drawing parameters.
		m_DefaultDrawParams.useScissorTest = false;
		m_DefaultDrawParams.useDepthTest = true;
		m_DefaultDrawParams.useStencilTest = true;
		m_DefaultDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_DefaultDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_DefaultDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_DefaultDrawParams.destBlend = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		m_DefaultDrawParams.shouldWriteDepth = true;
		m_DefaultDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;
		m_DefaultDrawParams.stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
		m_DefaultDrawParams.stencilComparisonVal = 1;
		m_DefaultDrawParams.stencilTestMask = 0xFF;
		m_DefaultDrawParams.stencilWriteMask = 0x00;
		m_DefaultDrawParams.stencilFail = StencilOp::STENCIL_KEEP;
		m_DefaultDrawParams.stencilPass = StencilOp::STENCIL_REPLACE;
		m_DefaultDrawParams.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
		m_DefaultDrawParams.scissorStartX = 0;
		m_DefaultDrawParams.scissorStartY = 0;
		m_DefaultDrawParams.scissorWidth = 0;
		m_DefaultDrawParams.scissorHeight = 0;


		// Set render to fbo target draw parameters.	
		m_FullscreenQuadDP.useScissorTest = false;
		m_FullscreenQuadDP.useDepthTest = false;
		m_FullscreenQuadDP.useStencilTest = true;
		m_FullscreenQuadDP.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_FullscreenQuadDP.faceCulling = FaceCulling::FACE_CULL_NONE;
		m_FullscreenQuadDP.sourceBlend = BlendFunc::BLEND_FUNC_NONE;
		m_FullscreenQuadDP.destBlend = BlendFunc::BLEND_FUNC_NONE;
		m_FullscreenQuadDP.shouldWriteDepth = true;
		m_FullscreenQuadDP.depthFunc = DrawFunc::DRAW_FUNC_LESS;
		m_FullscreenQuadDP.stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
		m_FullscreenQuadDP.stencilComparisonVal = 1;
		m_FullscreenQuadDP.stencilTestMask = 0xFF;
		m_FullscreenQuadDP.stencilWriteMask = 0xFF;
		m_FullscreenQuadDP.stencilFail = StencilOp::STENCIL_KEEP;
		m_FullscreenQuadDP.stencilPass = StencilOp::STENCIL_REPLACE;
		m_FullscreenQuadDP.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
		m_FullscreenQuadDP.scissorStartX = 0;
		m_FullscreenQuadDP.scissorStartY = 0;
		m_FullscreenQuadDP.scissorWidth = 0;
		m_FullscreenQuadDP.scissorHeight = 0;

		// Set skybox draw params.	
		m_SkyboxDrawParams.useScissorTest = false;
		m_SkyboxDrawParams.useDepthTest = true;
		m_SkyboxDrawParams.useStencilTest = true;
		m_SkyboxDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_SkyboxDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_SkyboxDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_SkyboxDrawParams.destBlend = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		m_SkyboxDrawParams.shouldWriteDepth = true;
		m_SkyboxDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LEQUAL;
		m_SkyboxDrawParams.stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
		m_SkyboxDrawParams.stencilComparisonVal = 0;
		m_SkyboxDrawParams.stencilTestMask = 0xFF;
		m_SkyboxDrawParams.stencilWriteMask = 0xFF;
		m_SkyboxDrawParams.stencilFail = StencilOp::STENCIL_KEEP;
		m_SkyboxDrawParams.stencilPass = StencilOp::STENCIL_REPLACE;
		m_SkyboxDrawParams.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
		m_SkyboxDrawParams.scissorStartX = 0;
		m_SkyboxDrawParams.scissorStartY = 0;
		m_SkyboxDrawParams.scissorWidth = 0;
		m_SkyboxDrawParams.scissorHeight = 0;
	}

	void RenderEngine::DumpMemory()
	{
		// Clear dumps.
		m_LoadedMeshes.clear();
		m_LoadedTextures.clear();
		m_LoadedMaterials.clear();
	}

	void RenderEngine::Render(float delta)
	{
		// Set render target
		m_RenderDevice.SetFBO(m_PrimaryRenderTarget.GetID());
		m_RenderDevice.SetViewport(Vector2::Zero, m_MainWindow.GetSize());

		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(m_DefaultDrawParams);


		// Write to the pingpong buffers to apply 2 pass gaussian blur.
		bool horizontal = true;
		bool firstIteration = true;
		unsigned int amount = 4;
		for (unsigned int i = 0; i < amount; i++)
		{
			// Select FBO
			m_RenderDevice.SetFBO(horizontal ? m_PingPongRenderTarget1.GetID() : m_PingPongRenderTarget2.GetID());

			// Setup material & use.
			m_ScreenQuadBlurMaterial.SetBool(MAT_ISHORIZONTAL, horizontal);
			if (firstIteration)
				m_ScreenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_PrimaryRTTexture1);
			else
			{
				if (horizontal)
					m_ScreenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_PingPongRTTexture2);
				else
					m_ScreenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_PingPongRTTexture1);
			}

			// Update shader data & draw.
			UpdateShaderData(&m_ScreenQuadBlurMaterial);
			m_RenderDevice.Draw(m_ScreenQuadVAO, m_FullscreenQuadDP, 0, 6, true);
			horizontal = !horizontal;
			if (firstIteration) firstIteration = false;
		}


		// Back to outline buffer
		//m_RenderDevice.SetFBO(m_OutlineRenderTarget.GetID());	
		//m_ScreenQuadOutlineMaterial.SetTexture(UF_SCREENTEXTURE, &m_PrimaryRTTexture2);
		//UpdateShaderData(&m_ScreenQuadOutlineMaterial);
		//m_RenderDevice.Draw(m_ScreenQuadVAO, m_FullscreenQuadDP, 0, 6, true);


		// Back to default buffer
		m_RenderDevice.SetFBO(0);

		// Clear color bit.
		m_RenderDevice.Clear(true, true, false, Color::White, 0xFF);

		// Set frame buffer texture on the material.
		m_ScreenQuadFinalMaterial.SetTexture(MAT_MAP_SCREEN, &m_PrimaryRTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D);
		m_ScreenQuadFinalMaterial.SetTexture(MAT_MAP_BLOOM, horizontal ? &m_PingPongRTTexture1 : &m_PingPongRTTexture2, TextureBindMode::BINDTEXTURE_TEXTURE2D);
		m_ScreenQuadFinalMaterial.SetTexture(MAT_MAP_OUTLINE, &m_OutlineRTTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D);
		
		Vector2 inverseMapSize = 1.0f / m_PrimaryRTTexture0.GetSize();
		m_ScreenQuadFinalMaterial.SetVector3(MAT_INVERSESCREENMAPSIZE, Vector3(inverseMapSize.x, inverseMapSize.y, 0.0));

		// update shader w/ material data.
		UpdateShaderData(&m_ScreenQuadFinalMaterial);

		// Draw full screen quad.
		m_RenderDevice.Draw(m_ScreenQuadVAO, m_FullscreenQuadDP, 0, 6, true);

		// Draw the final texture into primary fbo for storing purposes.
		//m_RenderDevice.SetFBO(m_PrimaryRenderTarget.GetID());
		//m_RenderDevice.Draw(m_ScreenQuadVAO, m_FullscreenQuadDP, 0, 6, true);
		//m_RenderDevice.SetFBO(0);
	}

	void RenderEngine::DrawOperationsDefault(float delta)
	{
		m_RenderDevice.SetFBO(0);

		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(m_DefaultDrawParams, nullptr, true);
	}

	void RenderEngine::DrawSkybox()
	{
		if (m_SkyboxMaterial != nullptr)
		{
			UpdateShaderData(m_SkyboxMaterial);
			m_RenderDevice.Draw(m_SkyboxVAO, m_SkyboxDrawParams, 1, 36, true);
		}

	}

	void RenderEngine::DrawSceneObjects(DrawParams& drawParams, Material* overrideMaterial, bool drawSkybox)
	{
		m_MeshRendererSystem.FlushOpaque(drawParams, overrideMaterial, true);
		m_MeshRendererSystem.FlushTransparent(drawParams, overrideMaterial, true);

		// Draw skybox.
		if (drawSkybox)
			DrawSkybox();
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

		m_GlobalDataBuffer.Update(&m_LightingSystem.GetDirectionalLightMatrix(), currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_GlobalDataBuffer.Update(&viewPos, currentGlobalDataOffset, sizeof(Vector4));
		currentGlobalDataOffset += sizeof(Vector4);

		ECS::CameraComponent* cameraComponent = &m_CameraSystem.GetCurrentCameraComponent();

		if (cameraComponent != nullptr)
		{
			// Update only if changed.
			if (m_BufferValueRecord.zNear != cameraComponent->zNear)
			{
				m_BufferValueRecord.zNear = cameraComponent->zNear;
				m_GlobalDataBuffer.Update(&cameraComponent->zNear, currentGlobalDataOffset, sizeof(float));
			}
			currentGlobalDataOffset += sizeof(float);



			// Update only if changed.
			if (m_BufferValueRecord.zFar != cameraComponent->zFar)
			{
				m_BufferValueRecord.zFar = cameraComponent->zFar;
				m_GlobalDataBuffer.Update(&cameraComponent->zNear, currentGlobalDataOffset, sizeof(float));
			}
			currentGlobalDataOffset += sizeof(float);
		}

		// Update lights buffer.
		m_GlobalLightBuffer.Update(&m_CurrentPointLightCount, 0, sizeof(int));
		m_GlobalLightBuffer.Update(&m_CurrentSpotLightCount, sizeof(int), sizeof(int));

		// Update debug fufer.
		m_GlobalDebugBuffer.Update(&m_DebugData.visualizeDepth, 0, sizeof(bool));
	}

	void RenderEngine::UpdateShaderData(Material* data)
	{

		m_RenderDevice.SetShader(data->GetShaderID());

		for (auto const& d : (*data).floats)
			m_RenderDevice.UpdateShaderUniformFloat(data->shaderID, d.first, d.second);

		for (auto const& d : (*data).booleans)
			m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first, d.second);

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

		for (auto const& d : (*data).sampler2Ds)
		{
			// Set whether the texture is active or not.
			bool isActive = (d.second.isActive && !d.second.boundTexture->GetIsEmpty()) ? true : false;
			m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first + MAT_EXTENSION_ISACTIVE, isActive);

			// Set the texture to corresponding active unit.
			m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first + MAT_EXTENSION_TEXTURE2D, d.second.unit);

			// Set texture
			if (d.second.isActive)
				m_RenderDevice.SetTexture(d.second.boundTexture->GetID(), d.second.boundTexture->GetSamplerID(), d.second.unit, d.second.bindMode, true);
			else
			{

				if (d.second.bindMode == TextureBindMode::BINDTEXTURE_TEXTURE2D)
					m_RenderDevice.SetTexture(m_DefaultTexture.GetID(), m_DefaultTexture.GetSamplerID(), d.second.unit, BINDTEXTURE_TEXTURE2D);
				else
					m_RenderDevice.SetTexture(m_DefaultCubemapTexture.GetID(), m_DefaultCubemapTexture.GetSamplerID(), d.second.unit, BINDTEXTURE_CUBEMAP);
			}
		}


		if (data->receivesLighting)
			m_LightingSystem.SetLightingShaderData(data->GetShaderID());

	}

	void RenderEngine::CaptureCalculateHDRI(Texture& hdriTexture)
	{
		// Create projection & view matrices for capturing HDRI data.
		Matrix captureProjection = Matrix::PerspectiveRH(90.0f, 1.0f, 0.1f, 10.0f);
		Matrix captureViews[] =
		{
			Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			Matrix::InitLookAtRH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		// Calculate HDRI, Irradiance, Prefilter and BRDF
		CalculateHDRICubemap(hdriTexture, captureProjection, captureViews);
		CalculateHDRIIrradiance(captureProjection, captureViews);
		CalculateHDRIPrefilter(captureProjection, captureViews);
		CalculateHDRIBRDF(captureProjection, captureViews);
		m_RenderDevice.SetFBO(0);

		// Set flag
		m_HDRIDataCaptured = true;
	}

	void RenderEngine::CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6])
	{
		// Generate sampler.
		SamplerParameters samplerParams;
		samplerParams.textureParams.wrapR = samplerParams.textureParams.wrapS = samplerParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		samplerParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution.
		m_HDRIResolution = Vector2(512, 512);

		// Construct Cubemap texture.
		m_HDRICubemap.ConstructRTCubemapTexture(m_RenderDevice, m_HDRIResolution, samplerParams);

		// Setup shader data.
		uint32 equirectangularShader = GetShader(Shaders::EQUIRECTANGULAR_HDRI).GetID();
		m_RenderDevice.SetShader(equirectangularShader);
		m_RenderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		m_RenderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_ISACTIVE), 1);
		m_RenderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_PROJECTION, captureProjection);
		m_RenderDevice.SetTexture(hdriTexture.GetID(), hdriTexture.GetSamplerID(), 0);
		m_RenderDevice.SetFBO(m_HDRICaptureRenderTarget.GetID());
		m_RenderDevice.SetViewport(Vector2::Zero, m_HDRIResolution);

		// Draw the cubemap.
		for (uint32 i = 0; i < 6; ++i)
		{
			m_RenderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_VIEW, views[i]);
			m_RenderDevice.BindTextureToRenderTarget(m_HDRICaptureRenderTarget.GetID(), m_HDRICubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false);
			m_RenderDevice.SetFBO(m_HDRICaptureRenderTarget.GetID());
			m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);
			m_RenderDevice.Draw(m_HDRICubeVAO, m_DefaultDrawParams, 0, 36, true);
		}

		// Generate mipmaps & check errors.
		m_RenderDevice.GenerateTextureMipmaps(m_HDRICubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP);
		m_RenderDevice.IsRenderTargetComplete(m_HDRICaptureRenderTarget.GetID());
	}

	void RenderEngine::CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6])
	{
		// Generate sampler.
		SamplerParameters irradianceParams;
		irradianceParams.textureParams.wrapR = irradianceParams.textureParams.wrapS = irradianceParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		irradianceParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		irradianceParams.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		irradianceParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		irradianceParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution
		Vector2 irradianceMapResolsution = Vector2(32, 32);

		// Create irradiance texture & scale render buffer according to the resolution.
		m_HDRIIrradianceMap.ConstructRTCubemapTexture(m_RenderDevice, irradianceMapResolsution, irradianceParams);
		m_RenderDevice.SetFBO(m_HDRICaptureRenderTarget.GetID());
		m_RenderDevice.ScaleRenderBuffer(m_HDRICaptureRenderTarget.GetID(), m_HDRICaptureRenderBuffer.GetID(), irradianceMapResolsution, RenderBufferStorage::STORAGE_DEPTH_COMP24);

		// Create & setup shader info.
		uint32 irradianceShader = GetShader(Shaders::IRRADIANCE_HDRI).GetID();
		m_RenderDevice.SetShader(irradianceShader);
		m_RenderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		m_RenderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
		m_RenderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_PROJECTION, captureProjection);
		m_RenderDevice.SetTexture(m_HDRICubemap.GetID(), m_HDRICubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);
		m_RenderDevice.SetViewport(Vector2::Zero, irradianceMapResolsution);

		// Draw cubemap.
		for (uint32 i = 0; i < 6; ++i)
		{
			m_RenderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_VIEW, views[i]);
			m_RenderDevice.BindTextureToRenderTarget(m_HDRICaptureRenderTarget.GetID(), m_HDRIIrradianceMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false, false);
			m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);
			m_RenderDevice.Draw(m_HDRICubeVAO, m_DefaultDrawParams, 0, 36, true);
		}
	}

	void RenderEngine::CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6])
	{
		// Generate sampler.
		SamplerParameters prefilterParams;
		prefilterParams.textureParams.generateMipMaps = true;
		prefilterParams.textureParams.wrapR = prefilterParams.textureParams.wrapS = prefilterParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		prefilterParams.textureParams.minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		prefilterParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		prefilterParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		prefilterParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution
		Vector2 prefilterResolution = Vector2(128, 128);

		// Construct prefilter texture.
		m_HDRIPrefilterMap.ConstructRTCubemapTexture(m_RenderDevice, prefilterResolution, prefilterParams);

		// Setup shader data.
		uint32 prefilterShader = GetShader(Shaders::PREFILTER_HDRI).GetID();
		m_RenderDevice.SetShader(prefilterShader);
		m_RenderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		m_RenderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
		m_RenderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ENVIRONMENTRESOLUTION, 512.0f);
		m_RenderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_PROJECTION, captureProjection);
		m_RenderDevice.SetTexture(m_HDRICubemap.GetID(), m_HDRICubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);

		// Setup mip levels & switch fbo.
		uint32 maxMipLevels = 5;
		m_RenderDevice.SetFBO(m_HDRICaptureRenderTarget.GetID());

		for (uint32 mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			m_RenderDevice.ScaleRenderBuffer(m_HDRICaptureRenderTarget.GetID(), m_HDRICaptureRenderBuffer.GetID(), Vector2(mipWidth, mipHeight), RenderBufferStorage::STORAGE_DEPTH_COMP24);
			m_RenderDevice.SetViewport(Vector2::Zero, Vector2(mipWidth, mipHeight));

			// Draw prefiltered map
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			m_RenderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ROUGHNESSMULTIPLIER, roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				m_RenderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_VIEW, views[i]);
				m_RenderDevice.BindTextureToRenderTarget(m_HDRICaptureRenderTarget.GetID(), m_HDRIPrefilterMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, mip, false, false);
				m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);
				m_RenderDevice.Draw(m_HDRICubeVAO, m_DefaultDrawParams, 0, 36, true);
			}
		}
	}

	void RenderEngine::CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6])
	{
		// Generate sampler.
		SamplerParameters samplerParams;
		samplerParams.textureParams.wrapR = samplerParams.textureParams.wrapS = samplerParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.textureParams.minFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution.
		Vector2 brdfLutSize = Vector2(512, 512);

		// Create BRDF texture.
		m_HDRILutMap.ConstructHDRI(m_RenderDevice, samplerParams, brdfLutSize, NULL);

		// Scale render buffer according to the resolution & bind lut map to frame buffer.
		m_RenderDevice.ScaleRenderBuffer(m_HDRICaptureRenderTarget.GetID(), m_HDRICaptureRenderBuffer.GetID(), brdfLutSize, RenderBufferStorage::STORAGE_DEPTH_COMP24);
		m_RenderDevice.BindTextureToRenderTarget(m_HDRICaptureRenderTarget.GetID(), m_HDRILutMap.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, 0, true, false);
		
		// Setup shader.
		uint32 brdfShader = GetShader(Shaders::BRDF_HDRI).GetID();
		m_RenderDevice.SetShader(brdfShader);
		
		// Switch framebuffer & draw.
		m_RenderDevice.SetFBO(m_HDRICaptureRenderTarget.GetID());
		m_RenderDevice.SetViewport(Vector2::Zero, brdfLutSize);
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);
		m_RenderDevice.Draw(m_ScreenQuadVAO, m_FullscreenQuadDP, 0, 6, true);
	}

	void RenderEngine::SetHDRIData(Material* mat)
	{
		if (mat == nullptr)
		{
			LINA_CORE_ERR("Materialto set HDRI data for is null, returning...");
			return;
		}

		if (!mat->usesHDRI)
		{
			LINA_CORE_ERR("This material's shader does not use HDRI calculations, returning...");
			return;
		}

		if (!m_HDRIDataCaptured)
		{
			LINA_CORE_ERR("HDRI data is not captured, please capture it first then set the material's data.");
			return;
		}

		mat->SetTexture(MAT_TEXTURE2D_IRRADIANCEMAP, &m_HDRIIrradianceMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
		mat->SetTexture(MAT_TEXTURE2D_BRDFLUTMAP, &m_HDRILutMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);
		mat->SetTexture(MAT_TEXTURE2D_PREFILTERMAP, &m_HDRIPrefilterMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
	}

	void RenderEngine::RemoveHDRIData(Material* mat)
	{
		if (mat == nullptr)
		{
			LINA_CORE_ERR("Materialto set HDRI data for is null, returning...");
			return;
		}

		if (!mat->usesHDRI)
		{
			LINA_CORE_ERR("This material's shader does not use HDRI calculations, returning...");
			return;
		}

		mat->RemoveTexture(MAT_TEXTURE2D_IRRADIANCEMAP);
		mat->RemoveTexture(MAT_TEXTURE2D_BRDFLUTMAP);
		mat->RemoveTexture(MAT_TEXTURE2D_PREFILTERMAP);

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

	void* RenderEngine::GetFinalImage()
	{
		return (void*)m_PrimaryRTTexture0.GetID();
	}

}