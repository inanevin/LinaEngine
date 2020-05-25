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

		// Construct screen quad.
		m_ScreenQuad = m_RenderDevice.CreateScreenQuadVertexArray();

		// Construct screen quad materials
		SetMaterialShader(m_ScreenQuadFinalMaterial, Shaders::SCREEN_QUAD_FINAL);
		SetMaterialShader(m_ScreenQuadBlurMaterial, Shaders::SCREEN_QUAD_FINAL);

		// Construct render targets
		ConstructRenderTargets();

		// Initialize built-in vertex array objects.
		m_SkyboxVAO = m_RenderDevice.CreateSkyboxVertexArray();

		// Create a default texture for render context.
		m_DefaultTexture.ConstructEmpty(m_RenderDevice);

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
			//(*it)->SetTexture(MC_TEXTURE2D_SHADOWMAP, &m_DepthMapRTTexture);

	//	for (std::set<Material*>::iterator it = m_ShadowMappedMaterials.begin(); it != m_ShadowMappedMaterials.end(); ++it)
		//	(*it)->SetTexture(MC_TEXTURE2D_SHADOWMAP, &m_PointLightsRTTexture, TextureBindMode::BINDTEXTURE_CUBEMAP);

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

	Texture& RenderEngine::CreateTexture(const std::string& filePath, SamplerParameters samplerParams, bool compress, bool useDefaultFormats)
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

	Texture& RenderEngine::CreateTexture(const std::string filePaths[6], SamplerParameters samplerParams, bool compress)
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
		material.sampler2Ds.clear();
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
			material.sampler2Ds[MC_TEXTURE2D_DIFFUSE] = { 0 };
			material.sampler2Ds[MC_TEXTURE2D_NORMALMAP] = { 1 };
			material.sampler2Ds[MC_TEXTURE2D_PARALLAXMAP] = { 2 };
			material.ints[MC_SURFACETYPE] = 0;
			material.ints[MC_SPECULAREXPONENT] = 32;
			material.vector2s[MC_TILING] = Vector2::One;
			material.receivesLighting = true;
			material.isShadowMapped = true;

			// Add to the shadow mapped materials.
			m_ShadowMappedMaterials.emplace(&material);
		}
		else if (shader == Shaders::STANDARD_UNLIT)
		{
			material.colors[MC_OBJECTCOLORPROPERTY] = Color::White;
			material.sampler2Ds[MC_TEXTURE2D_DIFFUSE] = { 0 };
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
			material.sampler2Ds[MC_TEXTURE2D_DIFFUSE] = { 0 };
		}
		else if (shader == Shaders::STENCIL_OUTLINE)
		{
			material.colors[MC_OBJECTCOLORPROPERTY] = Color::White;
			material.floats[MC_OUTLINETHICKNESS] = 0.1f;
			material.ints[MC_SURFACETYPE] = 0;
		}
		else if (shader == Shaders::SCREEN_QUAD_FINAL)
		{
			material.sampler2Ds[UF_SCREENTEXTURE] = { 0 };
		}
		else if (shader == Shaders::SCREEN_QUAD_BLUR)
		{
			material.sampler2Ds[UF_SCREENTEXTURE] = { 0 };
			material.booleans[UF_ISHORIZONTAL] = false;
		}
		else if (shader == Shaders::CUBEMAP_REFLECTIVE)
		{
			material.sampler2Ds[UF_SKYBOXTEXTURE] = { 0 };
		}
		else if (shader == Shaders::DEPTH_DIRECTIONAL_SHADOWS)
		{

		}
		else if (shader == Shaders::DEPTH_POINT_SHADOWS)
		{
			//material.matrices[UF_POINTSHADOWS_SHADOWMATRICES + std::string("[0]")] = Matrix();
			//material.matrices[UF_POINTSHADOWS_SHADOWMATRICES + std::string("[1]")] = Matrix();
			//material.matrices[UF_POINTSHADOWS_SHADOWMATRICES + std::string("[2]")] = Matrix();
			//material.matrices[UF_POINTSHADOWS_SHADOWMATRICES + std::string("[3]")] = Matrix();
			//material.matrices[UF_POINTSHADOWS_SHADOWMATRICES + std::string("[4]")] = Matrix();
			//material.matrices[UF_POINTSHADOWS_SHADOWMATRICES + std::string("[5]")] = Matrix();
			//material.vector3s[UF_POINTSHADOWS_LIGHTPOS] = Vector3(0, 4, 0);
			//material.floats[UF_POINTSHADOWS_FARPLANE] = 100;
		}
		else if (shader == Shaders::PBR_LIT)
		{
			material.sampler2Ds[MC_TEXTURE2D_ALBEDOMAP] = { 0 };
			material.sampler2Ds[MC_TEXTURE2D_NORMALMAP] = { 1 };
			material.sampler2Ds[MC_TEXTURE2D_METALLICMAP] = { 2 };
			material.sampler2Ds[MC_TEXTURE2D_ROUGHNESSMAP] = { 3 };
			material.sampler2Ds[MC_TEXTURE2D_AOMAP] = { 4 };
			material.vector2s[MC_TILING] = Vector2::One;
			material.receivesLighting = true;
			material.isShadowMapped = true;
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
		Shader& unlit = CreateShader(Shaders::STANDARD_UNLIT, "resources/shaders/basicStandardUnlit.glsl");
		unlit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		unlit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Lit
		Shader& lit = CreateShader(Shaders::STANDARD_LIT, "resources/shaders/basicStandardLit.glsl", false);
		lit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		lit.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
		lit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// PBR Lit
		Shader& pbrLit = CreateShader(Shaders::PBR_LIT, "resources/shaders/PBR/pbrLit.glsl", false);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Skies
		CreateShader(Shaders::SKYBOX_SINGLECOLOR, "resources/shaders/skyboxSingleColor.glsl");
		CreateShader(Shaders::SKYBOX_GRADIENT, "resources/shaders/skyboxVertexGradient.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_CUBEMAP, "resources/shaders/skyboxCubemap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SKYBOX_PROCEDURAL, "resources/shaders/skyboxProcedural.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Others
		Shader& singleColor = CreateShader(Shaders::STENCIL_OUTLINE, "resources/shaders/stencilOutline.glsl");
		singleColor.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		singleColor.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Screen Quad Shaders
		CreateShader(Shaders::SCREEN_QUAD_FINAL, "resources/shaders/screenQuadFinal.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		CreateShader(Shaders::SCREEN_QUAD_BLUR, "resources/shaders/screenQuadBlur.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Cubemap reflective
		CreateShader(Shaders::CUBEMAP_REFLECTIVE, "resources/shaders/cubemapReflective.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Depth Shader
		CreateShader(Shaders::DEPTH_DIRECTIONAL_SHADOWS, "resources/shaders/directionalDepthMap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Depth Shader for point lights
		CreateShader(Shaders::DEPTH_POINT_SHADOWS, "resources/shaders/pointLightDepthMap.glsl", true).BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
	
	}

	void RenderEngine::ConstructEngineMaterials()
	{
		// Stencil outline material.
		CreateMaterial(MAT_LINASTENCILOUTLINE, Shaders::STENCIL_OUTLINE);
		m_LoadedMaterials[MAT_LINASTENCILOUTLINE].floats[MC_OUTLINETHICKNESS] = 0.015f;
		m_LoadedMaterials[MAT_LINASTENCILOUTLINE].colors[MC_OBJECTCOLORPROPERTY] = Color::Red;

		// Create material for rendering directional shadows onto the depth buffer
		m_DepthBufferMaterial = &CreateMaterial("dirShadowsDepth", Shaders::DEPTH_DIRECTIONAL_SHADOWS);


		// Create material for rendering point shadows onto the depth buffer
		m_PointLightsDepthMaterial = &CreateMaterial("pointShadowsDepth", Shaders::DEPTH_POINT_SHADOWS);
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
		mainRTParams.textureParams.pixelFormat =  PixelFormat::FORMAT_RGB;
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

		SamplerParameters intRTParams;
		intRTParams.textureParams.pixelFormat = PixelFormat::FORMAT_RGB;
		intRTParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
		intRTParams.textureParams.minFilter = intRTParams.textureParams.magFilter = SamplerFilter::FILTER_LINEAR;
		intRTParams.textureParams.wrapS = intRTParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		SamplerParameters depthRTParams;
		depthRTParams.textureParams.pixelFormat = depthRTParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_DEPTH;
		depthRTParams.textureParams.minFilter = depthRTParams.textureParams.magFilter = SamplerFilter::FILTER_NEAREST;
		depthRTParams.textureParams.wrapS = depthRTParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		SamplerParameters pointLightRTParams;
		pointLightRTParams.textureParams.pixelFormat = pointLightRTParams.textureParams.internalPixelFormat = PixelFormat::FORMAT_DEPTH;
		pointLightRTParams.textureParams.minFilter = pointLightRTParams.textureParams.magFilter = SamplerFilter::FILTER_NEAREST;
		pointLightRTParams.textureParams.wrapS = pointLightRTParams.textureParams.wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		m_ShadowMapResolution = Vector2(2048, 2048);

		// Initialize frame buffer texture.
		m_MainRTTexture.ConstructRTTextureMSAA(m_RenderDevice, screenSize, mainRTParams, 4);

		// Initialize primary RT textures
		m_PrimaryRTTexture0.ConstructRTTexture(m_RenderDevice, screenSize, primaryRTParams, false);
		m_PrimaryRTTexture1.ConstructRTTexture(m_RenderDevice, screenSize, primaryRTParams, false);

		// Initialize ping pong rt texture
		m_PingPongRTTexture1.ConstructRTTexture(m_RenderDevice, screenSize, pingPongRTParams, false);
		m_PingPongRTTexture2.ConstructRTTexture(m_RenderDevice, screenSize, pingPongRTParams, false);

		// Initialize intermediate frame buffer texture
		m_IntermediateRTTexture.ConstructRTTexture(m_RenderDevice, screenSize, intRTParams, false);

		// Initialize depth map teture
		m_DepthMapRTTexture.ConstructRTTexture(m_RenderDevice, m_ShadowMapResolution, depthRTParams, true);

		// Initialize point light rt texture
		m_PointLightsRTTexture.ConstructRTCubemapTexture(m_RenderDevice, m_ShadowMapResolution, pointLightRTParams);
	
		// Initialize render buffer.
		m_RenderBuffer.Construct(m_RenderDevice, RenderBufferStorage::STORAGE_DEPTH24_STENCIL8, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), 4);

		// Initialize primary render buffer
		m_PrimaryRenderBuffer.Construct(m_RenderDevice, RenderBufferStorage::STORAGE_DEPTH24_STENCIL8, screenSize.x, screenSize.y);

		// Initialize intermediate render buffer
		m_IntermediateRenderBuffer.Construct(m_RenderDevice, RenderBufferStorage::STORAGE_DEPTH, screenSize.x, screenSize.y);

		// Initialize the render target w/ render buffer.
		m_MainRenderTarget.Construct(m_RenderDevice, m_MainRTTexture, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH_AND_STENCIL, m_RenderBuffer.GetID());

		// Initialize intermediate render target.
		//m_IntermediateRenderTarget.Construct(m_RenderDevice, m_IntermediateRTTexture, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_IntermediateRenderBuffer.GetID());
		m_IntermediateRenderTarget.Construct(m_RenderDevice, m_IntermediateRTTexture, m_MainWindow.GetWidth(), m_MainWindow.GetHeight(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

		// Initialize primary render target.
		m_PrimaryRenderTarget.Construct(m_RenderDevice, m_PrimaryRTTexture0, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH_AND_STENCIL, m_PrimaryRenderBuffer.GetID());

		// Bind the extre texture to primary render target, also tell open gl that we are running mrts.
		m_RenderDevice.BindTextureToRenderTarget(m_PrimaryRenderTarget.GetID(), m_PrimaryRTTexture1.GetID(), screenSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
		uint32 attachments[2] = { FrameBufferAttachment::ATTACHMENT_COLOR , (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1) };
		m_RenderDevice.MultipleDrawBuffersCommand(m_PrimaryRenderTarget.GetID(), 2, attachments);

		// Initialize ping pong render targets
		m_PingPongRenderTarget1.Construct(m_RenderDevice, m_PingPongRTTexture1, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		m_PingPongRenderTarget2.Construct(m_RenderDevice, m_PingPongRTTexture2, screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		
		// Bind the secondary texture to intermediate render target & tell open gl to draw 2 buffers.
		//m_RenderDevice.BindTextureToRenderTarget(m_IntermediateRenderTarget.GetID(), screenSize.x, screenSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1, 0);
		//uint32 attachments[2] = { FrameBufferAttachment::ATTACHMENT_COLOR , (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1) };
		//m_RenderDevice.MultipleDrawBuffersCommand(m_IntermediateRenderTarget.GetID(), 2, attachments);

		// Initialize shadow map target
		m_DepthMapRenderTarget.Construct(m_RenderDevice, m_DepthMapRTTexture, m_ShadowMapResolution.x, m_ShadowMapResolution.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

		// Initialize point light shadow map target
		m_PointLightsRenderTarget.Construct(m_RenderDevice, m_PointLightsRTTexture, m_ShadowMapResolution.x, m_ShadowMapResolution.y, TextureBindMode::BINDTEXTURE_NONE, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

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



		// Set depth map drawing parameters.
		m_DepthMapDrawParams.useScissorTest = false;
		m_DepthMapDrawParams.useDepthTest = true;
		m_DepthMapDrawParams.useStencilTest = false;
		m_DepthMapDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_DepthMapDrawParams.faceCulling = FaceCulling::FACE_CULL_NONE;
		m_DepthMapDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_NONE;
		m_DepthMapDrawParams.destBlend = BlendFunc::BLEND_FUNC_NONE;
		m_DepthMapDrawParams.shouldWriteDepth = true;
		m_DepthMapDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;
		m_DepthMapDrawParams.stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
		m_DepthMapDrawParams.stencilComparisonVal = 1;
		m_DepthMapDrawParams.stencilTestMask = 0xFF;
		m_DepthMapDrawParams.stencilWriteMask = 0xFF;
		m_DepthMapDrawParams.stencilFail = StencilOp::STENCIL_KEEP;
		m_DepthMapDrawParams.stencilPass = StencilOp::STENCIL_REPLACE;
		m_DepthMapDrawParams.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
		m_DepthMapDrawParams.scissorStartX = 0;
		m_DepthMapDrawParams.scissorStartY = 0;
		m_DepthMapDrawParams.scissorWidth = 0;
		m_DepthMapDrawParams.scissorHeight = 0;

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

		// Set stencil draw params first pass.		
		m_StencilOutlineDrawParams.useStencilTest = true;
		m_StencilOutlineDrawParams.useDepthTest = true;
		m_StencilOutlineDrawParams.useScissorTest = false;
		m_StencilOutlineDrawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_StencilOutlineDrawParams.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_StencilOutlineDrawParams.sourceBlend = BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_StencilOutlineDrawParams.destBlend = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		m_StencilOutlineDrawParams.depthFunc = DrawFunc::DRAW_FUNC_LESS;
		m_StencilOutlineDrawParams.shouldWriteDepth = true;
		m_StencilOutlineDrawParams.stencilComparisonVal = 1;
		m_StencilOutlineDrawParams.stencilTestMask = 0xFF;
		m_StencilOutlineDrawParams.stencilWriteMask = 0xFF;
		m_StencilOutlineDrawParams.stencilFunc = DrawFunc::DRAW_FUNC_ALWAYS;
		m_StencilOutlineDrawParams.stencilFail = StencilOp::STENCIL_KEEP;
		m_StencilOutlineDrawParams.stencilPass = StencilOp::STENCIL_REPLACE;
		m_StencilOutlineDrawParams.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
		m_StencilOutlineDrawParams.scissorStartX = 0;
		m_StencilOutlineDrawParams.scissorStartY = 0;
		m_StencilOutlineDrawParams.scissorWidth = 0;
		m_StencilOutlineDrawParams.scissorHeight = 0;

		// Set stencil draw params second first pass.		
		m_StencilOutlineDrawParams2.useStencilTest = true;
		m_StencilOutlineDrawParams2.useDepthTest = true;
		m_StencilOutlineDrawParams2.useScissorTest = false;
		m_StencilOutlineDrawParams2.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
		m_StencilOutlineDrawParams2.faceCulling = FaceCulling::FACE_CULL_BACK;
		m_StencilOutlineDrawParams2.sourceBlend = BlendFunc::BLEND_FUNC_SRC_ALPHA;
		m_StencilOutlineDrawParams2.destBlend = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
		m_StencilOutlineDrawParams2.depthFunc = DrawFunc::DRAW_FUNC_LESS;
		m_StencilOutlineDrawParams2.shouldWriteDepth = true;
		m_StencilOutlineDrawParams2.stencilComparisonVal = 1;
		m_StencilOutlineDrawParams2.stencilTestMask = 0xFF;
		m_StencilOutlineDrawParams2.stencilWriteMask = 0x00;
		m_StencilOutlineDrawParams2.stencilFunc = DrawFunc::DRAW_FUNC_NOT_EQUAL;
		m_StencilOutlineDrawParams2.stencilFail = StencilOp::STENCIL_KEEP;
		m_StencilOutlineDrawParams2.stencilPass = StencilOp::STENCIL_REPLACE;
		m_StencilOutlineDrawParams2.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
		m_StencilOutlineDrawParams2.scissorStartX = 0;
		m_StencilOutlineDrawParams2.scissorStartY = 0;
		m_StencilOutlineDrawParams2.scissorWidth = 0;
		m_StencilOutlineDrawParams2.scissorHeight = 0;
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

		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(false, m_DefaultDrawParams);

		// Back to default buffer
		m_RenderDevice.SetFBO(0);


		// Clear color bit.
		m_RenderDevice.Clear(true, false, false, Color::White, 0xFF);

		// Set frame buffer texture on the material.
		m_ScreenQuadFinalMaterial.SetTexture(UF_SCREENTEXTURE, &m_PrimaryRTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		// update shader w/ material data.
		UpdateShaderData(&m_ScreenQuadFinalMaterial);

		// Draw full screen quad.
		m_RenderDevice.Draw(m_ScreenQuad, m_FullscreenQuadDP, 0, 6, true);


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
		DrawSceneObjects(false, m_DefaultDrawParams, nullptr, true);
	}

	void RenderEngine::DrawOperationsPointLight(float delta, bool visualizeDepthMap)
	{
		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Set depth frame buffer
		m_RenderDevice.SetFBO(m_PointLightsRenderTarget.GetID());

		// Clear color.
		m_RenderDevice.Clear(false, true, false, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		m_RenderDevice.SetShader(m_PointLightsDepthMaterial->GetShaderID());

		std::vector<Matrix> matrices = m_LightingSystem.GetPointLightMatrices();


		for (unsigned int i = 0; i < 6; ++i)
		{
			std::string name = UF_POINTSHADOWS_SHADOWMATRICES + std::string("[") + std::to_string(i) + std::string("]");
			m_RenderDevice.UpdateShaderUniformMatrix(m_PointLightsDepthMaterial->GetShaderID(), name, matrices[i]);
			//m_PointLightsDepthMaterial->SetMatrix4( name, m_LightingSystem.GetPointLightMatrices()[i]);
		}

		// Draw scene into depth buffer.
		DrawSceneObjects(false, m_DepthMapDrawParams, m_PointLightsDepthMaterial, false);

		// Visaulize depth buffer
		if (visualizeDepthMap)
			DrawFullscreenQuad(m_PointLightsRTTexture, false);

	}

	void RenderEngine::DrawOperationsMSAA(float delta)
	{
		m_RenderDevice.SetFBO(m_MainRenderTarget.GetID());

		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(false, m_DefaultDrawParams);

		// Blit frame buffers
		int w = m_MainWindow.GetWidth();
		int h = m_MainWindow.GetHeight();
		m_RenderDevice.BlitFrameBuffers(m_MainRenderTarget.GetID(), w, h, m_IntermediateRenderTarget.GetID(), w, h, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST);

		// Back to default buffer
		m_RenderDevice.SetFBO(0);

		// Clear color bit.
		m_RenderDevice.Clear(true, false, false, Color::White, 0xFF);

		// Set frame buffer texture on the material.
		m_ScreenQuadFinalMaterial.SetTexture(UF_SCREENTEXTURE, &m_IntermediateRTTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		// update shader w/ material data.
		UpdateShaderData(&m_ScreenQuadFinalMaterial);

		// Draw full screen quad.
		m_RenderDevice.Draw(m_ScreenQuad, m_FullscreenQuadDP, 0, 6, true);
	}

	void RenderEngine::DrawOperationsShadows(float delta, bool visualizeDepthMap)
	{
		// Clear color.
		m_RenderDevice.Clear(true, true, true, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_RenderingPipeline.UpdateSystems(delta);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Set depth frame buffer
		m_RenderDevice.SetFBO(m_DepthMapRenderTarget.GetID());

		// Clear color.
		m_RenderDevice.Clear(false, true, false, m_CameraSystem.GetCurrentClearColor(), 0xFF);

		// Draw scene into depth buffer.
		DrawSceneObjects(false, m_DepthMapDrawParams, m_DepthBufferMaterial, false);

		// Visaulize depth buffer
		if (visualizeDepthMap)
			DrawFullscreenQuad(m_DepthMapRTTexture, false);
	}

	void RenderEngine::DrawSkybox()
	{
		if (m_SkyboxMaterial != nullptr)
		{
			UpdateShaderData(m_SkyboxMaterial);
			m_RenderDevice.Draw(m_SkyboxVAO, m_SkyboxDrawParams, 1, 36, true);
		}
	}

	void RenderEngine::DrawSceneObjects(bool useStencilOutlining, DrawParams& drawParams, Material* overrideMaterial, bool drawSkybox)
	{
		// Draw opaques.
		if (useStencilOutlining)
		{
			// Draw scene.
			m_MeshRendererSystem.FlushOpaque(m_StencilOutlineDrawParams, nullptr, false);
			m_MeshRendererSystem.FlushTransparent(m_StencilOutlineDrawParams, nullptr, false);

			// Enable depth test for drawing w/ new params.
			m_RenderDevice.SetDepthTestEnable(false);

			// Draw scene.
			m_MeshRendererSystem.FlushOpaque(m_StencilOutlineDrawParams2, &m_LoadedMaterials[MAT_LINASTENCILOUTLINE], true);
			m_MeshRendererSystem.FlushTransparent(m_StencilOutlineDrawParams2, &m_LoadedMaterials[MAT_LINASTENCILOUTLINE], true);

			// Reset stencil.
			m_RenderDevice.SetStencilWriteMask(0xFF);
			m_RenderDevice.SetDepthTestEnable(true);
		}
		else
		{
			m_MeshRendererSystem.FlushOpaque(drawParams, overrideMaterial, true);
			m_MeshRendererSystem.FlushTransparent(drawParams, overrideMaterial, true);
		}

		// Draw skybox.
		if (drawSkybox)
			DrawSkybox();
	}

	void RenderEngine::DrawFullscreenQuad(Texture& texture, bool blit)
	{
		int w = m_MainWindow.GetWidth();
		int h = m_MainWindow.GetHeight();

		// Blit read & write buffers.
		if (blit)
			m_RenderDevice.BlitFrameBuffers(m_MainRenderTarget.GetID(), w, h, m_IntermediateRenderTarget.GetID(), w, h, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST);

		// Back to default buffer
		m_RenderDevice.SetFBO(0);

		// Clear color bit.
		m_RenderDevice.Clear(true, blit, false, Color::White, 0xFF);

		// Set frame buffer texture on the material.
		m_ScreenQuadFinalMaterial.SetTexture(UF_SCREENTEXTURE, &texture, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		// update shader w/ material data.
		UpdateShaderData(&m_ScreenQuadFinalMaterial);

		// Draw
		m_RenderDevice.Draw(m_ScreenQuad, m_FullscreenQuadDP, 0, 6, true);
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
			m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first + MC_EXTENSION_ISACTIVE, isActive);

			if (d.second.isActive)
			{
				// Set the texture to corresponding active unit.
				m_RenderDevice.UpdateShaderUniformInt(data->shaderID, d.first + MC_EXTENSION_TEXTURE2D, d.second.unit);

				// Set texture
				m_RenderDevice.SetTexture(d.second.boundTexture->GetID(), d.second.boundTexture->GetSamplerID(), d.second.unit, d.second.bindMode, true);


			}
			else
			{
				m_RenderDevice.SetTexture(m_DefaultTexture.GetID(), 0, d.second.unit);
			}
		}


		if (data->receivesLighting)
			m_LightingSystem.SetLightingShaderData(data->GetShaderID());

	}

}