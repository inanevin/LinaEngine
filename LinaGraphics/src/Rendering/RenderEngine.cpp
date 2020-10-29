/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Core/Layer.hpp"
#include "Rendering/RenderEngine.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/RenderConstants.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ArrayBitmap.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/ECS.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "PackageManager/OpenGL/GLRenderDevice.hpp"
#include "Helpers/DrawParameterHelper.hpp"

namespace LinaEngine::Graphics
{
	RenderDevice RenderEngine::m_renderDevice;

	constexpr size_t UNIFORMBUFFER_VIEWDATA_SIZE = (sizeof(Matrix) * 3) + (sizeof(Vector4)) + (sizeof(float) * 2);
	constexpr int UNIFORMBUFFER_VIEWDATA_BINDPOINT = 0;
	constexpr auto UNIFORMBUFFER_VIEWDATA_NAME = "ViewData";

	constexpr size_t UNIFORMBUFFER_LIGHTDATA_SIZE = (sizeof(int) * 2) + sizeof(Vector4) + sizeof(Vector4);
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
		// Delete textures.
		for (std::map<int, Texture*>::iterator it = m_loadedTextures.begin(); it != m_loadedTextures.end(); it++)
			delete it->second;

		// Dump the remaining memory.
		DumpMemory();

		// Release Vertex Array Objects
		m_skyboxVAO = m_renderDevice.ReleaseVertexArray(m_skyboxVAO);
		m_screenQuadVAO = m_renderDevice.ReleaseVertexArray(m_screenQuadVAO);
		m_hdriCubeVAO = m_renderDevice.ReleaseVertexArray(m_hdriCubeVAO);
		m_lineVAO = m_renderDevice.ReleaseVertexArray(m_lineVAO);

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(LinaEngine::ECS::ECSRegistry& ecsReg, Window& appWindow)
	{

		// Register ECS components
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::CameraComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::PointLightComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::SpotLightComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::DirectionalLightComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::MeshRendererComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::SpriteRendererComponent>();

		// Set references.
		m_appWindow = &appWindow;

		// Flip loaded images.
		ArrayBitmap::SetImageFlip(true);

		// Setup draw parameters.
		m_defaultDrawParams= DrawParameterHelper::GetDefault();
		m_skyboxDrawParams = DrawParameterHelper::GetSkybox();
		m_fullscreenQuadDP = DrawParameterHelper::GetFullScreenQuad();
		m_shadowMapDrawParams = DrawParameterHelper::GetShadowMap();


		// Initialize the render device.
		m_renderDevice.Initialize(m_appWindow->GetWidth(), m_appWindow->GetHeight(), m_defaultDrawParams);

		// Construct the uniform buffer for global matrices.
		m_globalDataBuffer.Construct(m_renderDevice, UNIFORMBUFFER_VIEWDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_globalDataBuffer.Bind(UNIFORMBUFFER_VIEWDATA_BINDPOINT);

		// Construct the uniform buffer for lights.
		m_globalLightBuffer.Construct(m_renderDevice, UNIFORMBUFFER_LIGHTDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_globalLightBuffer.Bind(UNIFORMBUFFER_LIGHTDATA_BINDPOINT);

		// Construct the uniform buffer for debugging.
		m_globalDebugBuffer.Construct(m_renderDevice, UNIFORMBUFFER_DEBUGDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_globalDebugBuffer.Bind(UNIFORMBUFFER_DEBUGDATA_BINDPOINT);

		// Initialize the engine shaders.
		ConstructEngineShaders();

		// Initialize engine materials
		ConstructEngineMaterials();

		// Initialize engine vertex arrays.
		ConstructEnginePrimitives();

		// Initialize built-in vertex array objects.
		m_skyboxVAO = m_renderDevice.CreateSkyboxVertexArray();
		m_hdriCubeVAO = m_renderDevice.CreateHDRICubeVertexArray();
		m_screenQuadVAO = m_renderDevice.CreateScreenQuadVertexArray();
		m_lineVAO = m_renderDevice.CreateLineVertexArray();

		// Construct render targets
		ConstructRenderTargets();

		// Create default textures.
		m_defaultTexture.ConstructEmpty(m_renderDevice);
		m_defaultCubemapTexture.ConstructRTCubemapTexture(m_renderDevice, m_viewportSize, SamplerParameters());

		// Initialize ECS Camera System.
		m_cameraSystem.Construct(ecsReg);
		m_cameraSystem.SetAspectRatio((float)m_viewportSize.x / (float)m_viewportSize.y);

		// Initialize ECS Mesh Renderer System
		m_meshRendererSystem.Construct(ecsReg, *this, m_renderDevice);
		m_spriteRendererSystem.Construct(ecsReg, *this, m_renderDevice);

		// Initialize ECS Lighting system.
		m_lightingSystem.Construct(ecsReg, m_renderDevice, *this);

		// Add the ECS systems into the pipeline.
		m_renderingPipeline.AddSystem(m_cameraSystem);
		m_renderingPipeline.AddSystem(m_meshRendererSystem);
		m_renderingPipeline.AddSystem(m_spriteRendererSystem);
		m_renderingPipeline.AddSystem(m_lightingSystem);

		// Set debug values.
		m_debugData.visualizeDepth = false;

	}

	void RenderEngine::Render()
	{
		// DrawShadows();

		Draw();

		if (!m_firstFrameDrawn)
		{
			ValidateEngineShaders();
			m_firstFrameDrawn = true;
		}

		//DrawOperationsDefault();

	}

	void RenderEngine::RenderLayers()
	{
		// Draw GUI Layers
		for (Layer* layer : m_guiLayerStack)
			layer->Render();
	}

	void RenderEngine::Swap()
	{

		// Update window.
		m_appWindow->Tick();
	}

	void RenderEngine::SetViewportDisplay(Vector2 pos, Vector2 size)
	{
		m_renderDevice.SetViewport(pos, size);
		m_viewportPos = pos;
		m_viewportSize = size;

		m_cameraSystem.SetAspectRatio((float)m_viewportSize.x / (float)m_viewportSize.y);

		// Resize render buffers & frame buffer textures
		m_renderDevice.ResizeRTTexture(m_primaryRTTexture0.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
		m_renderDevice.ResizeRTTexture(m_primaryRTTexture1.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
		//m_renderDevice.ResizeRTTexture(m_OutlineRTTexture.GetID(), windowSize, primaryRTParams.m_textureParams.m_internalPixelFormat, primaryRTParams.m_textureParams.m_pixelFormat);
		m_renderDevice.ResizeRTTexture(m_pingPongRTTexture1.GetID(), m_viewportSize, m_pingPongRTParams.m_textureParams.m_internalPixelFormat, m_pingPongRTParams.m_textureParams.m_pixelFormat);
		m_renderDevice.ResizeRTTexture(m_pingPongRTTexture1.GetID(), m_viewportSize, m_pingPongRTParams.m_textureParams.m_internalPixelFormat, m_pingPongRTParams.m_textureParams.m_pixelFormat);
		m_renderDevice.ResizeRenderBuffer(m_primaryRenderTarget.GetID(), m_primaryRenderBuffer.GetID(), m_viewportSize, RenderBufferStorage::STORAGE_DEPTH);
	}


	Material& RenderEngine::CreateMaterial(Shaders shader, const std::string& path)
	{
		// Create material & set it's shader.
		int id = Utility::GetUniqueID();
		Material& mat = m_loadedMaterials[id];
		SetMaterialShader(mat, shader);
		SetMaterialContainers(mat);
		mat.m_materialID = id;
		mat.m_path = path.compare("") == 0 ? INTERNAL_MAT_PATH : path;
		return m_loadedMaterials[id];
	}


	Material& RenderEngine::LoadMaterialFromFile(const std::string& path)
	{
		// Create material & set it's shader.
		int id = Utility::GetUniqueID();
		Material& mat = m_loadedMaterials[id];
		Material::LoadMaterialData(mat, path);
		SetMaterialContainers(mat);
		mat.m_materialID = id;
		mat.m_path = path;
		return m_loadedMaterials[id];
	}

	void RenderEngine::MaterialUpdated(Material& mat)
	{
		if (!mat.m_usesHDRI && m_hdriMaterials.find(&mat) != m_hdriMaterials.end())
		{
			m_hdriMaterials.erase(&mat);
			RemoveHDRIData(&mat);
		}
		else if (mat.m_usesHDRI && m_hdriMaterials.find(&mat) == m_hdriMaterials.end())
		{
			m_hdriMaterials.emplace(&mat);
			SetHDRIData(&mat);
		}
	}

	Texture& RenderEngine::CreateTexture2D(const std::string& filePath, SamplerParameters samplerParams, bool compress, bool useDefaultFormats, const std::string& paramsPath)
	{
		// Create pixel data.
		ArrayBitmap* textureBitmap = new ArrayBitmap();

		int nrComponents = textureBitmap->Load(filePath);
		if (nrComponents == -1)
		{
			LINA_CORE_WARN("Texture with the path {0} doesn't exist, returning empty texture", filePath);
			delete textureBitmap;
			return m_defaultTexture;
		}

		if (useDefaultFormats)
		{
			if (nrComponents == 1)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_R;
			if (nrComponents == 2)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RG;
			else if (nrComponents == 3)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
			else if (nrComponents == 4)
				samplerParams.m_textureParams.m_internalPixelFormat = samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGBA;

		}

		// Create texture & construct.
		Texture* texture = new Texture();
		texture->Construct(m_renderDevice, *textureBitmap, samplerParams, compress, filePath);
		m_loadedTextures[texture->GetID()] = texture;
		texture->m_paramsPath = paramsPath;

		// Delete pixel data.
		delete textureBitmap;

		LINA_CORE_TRACE("Texture created. {0}", filePath);

		// Return
		return *m_loadedTextures[texture->GetID()];
	}

	Texture& RenderEngine::CreateTextureHDRI(const std::string filePath)
	{
		// Create pixel data.
		int w, h, nrComponents;
		float* data = ArrayBitmap::LoadImmediateHDRI(filePath.c_str(), w, h, nrComponents);

		if (!data)
		{
			LINA_CORE_WARN("Texture with the path {0} doesn't exist, returning empty texture", filePath);
			return m_defaultTexture;
		}

		// Create texture & construct.
		SamplerParameters samplerParams;
		samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.m_textureParams.m_minFilter = samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		Texture* texture = new Texture();
		texture->ConstructHDRI(m_renderDevice, samplerParams, Vector2(w, h), data, filePath);
		m_loadedTextures[texture->GetID()] = texture;

		// Return
		return *m_loadedTextures[texture->GetID()];
	}

	Material& RenderEngine::GetMaterial(int id)
	{
		if (!MaterialExists(id))
		{
			// Mesh not found.
			LINA_CORE_WARN("Material with the id {0} was not found, returning default material...", id);
			return m_defaultUnlit;
		}

		return m_loadedMaterials[id];
	}

	Material& RenderEngine::GetMaterial(const std::string& path)
	{
		const auto it = std::find_if(m_loadedMaterials.begin(), m_loadedMaterials.end(), [path]
		(const auto& item) -> bool { return item.second.GetPath().compare(path) == 0; });

		if (it == m_loadedMaterials.end())
		{
			// Mesh not found.
			LINA_CORE_WARN("Material with the path {0} was not found, returning un-constructed material...", path);
			return Material();
		}

		return it->second;
	}

	Texture& RenderEngine::GetTexture(int id)
	{
		if (!TextureExists(id))
		{
			// Mesh not found.
			LINA_CORE_WARN("Texture with the id {0} was not found, returning un-constructed texture...", id);
			return Texture();
		}

		return *m_loadedTextures[id];
	}

	Texture& RenderEngine::GetTexture(const std::string& path)
	{
		const auto it = std::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), [path]
		(const auto& item) -> bool { return item.second->GetPath().compare(path) == 0; });

		if (it == m_loadedTextures.end())
		{
			// Mesh not found.
			LINA_CORE_WARN("Texture with the path {0} was not found, returning un-constructed texture...", path);
			return Texture();
		}

		return *it->second;
	}


	Material& RenderEngine::SetMaterialShader(Material& material, Shaders shader)
	{
		// If no shader found, fall back to standardLit
		std::map<int, Shader>& shaders = Shader::GetLoadedShaders();
		if (shaders.find(shader) == shaders.end()) {
			LINA_CORE_WARN("Shader with engine ID {0} was not found. Setting material's shader to standardUnlit.", shader);
			material.m_shaderID = shaders[Shaders::Standard_Unlit].GetID();
		}
		else
			material.m_shaderID = shaders[shader].GetID();

		// Clear all shader related material data.
		material.m_sampler2Ds.clear();
		material.m_colors.clear();
		material.m_floats.clear();
		material.m_ints.clear();
		material.m_vector3s.clear();
		material.m_vector2s.clear();
		material.m_matrices.clear();
		material.m_vector4s.clear();
		material.m_shaderType = shader;
		material.m_isShadowMapped = false;
		material.m_receivesLighting = false;
		material.m_usesHDRI = false;

		if (shader == Shaders::Standard_Unlit)
		{
			material.m_colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.m_sampler2Ds[MAT_TEXTURE2D_DIFFUSE] = { 0 };
			material.m_ints[MAT_SURFACETYPE] = 0;
			// m_shadowMappedMaterials.emplace(&material);

		}
		else if (shader == Shaders::Skybox_SingleColor)
		{
			material.m_colors[MAT_COLOR] = Color::Gray;
		}
		else if (shader == Shaders::Skybox_Gradient)
		{
			material.m_colors[MAT_STARTCOLOR] = Color::Black;
			material.m_colors[MAT_ENDCOLOR] = Color::White;
		}
		else if (shader == Shaders::Skybox_Procedural)
		{
			material.m_colors[MAT_STARTCOLOR] = Color::Black;
			material.m_colors[MAT_ENDCOLOR] = Color::White;
			material.m_vector3s[MAT_SUNDIRECTION] = Vector3(0, -1, 0);
		}
		else if (shader == Shaders::Skybox_Cubemap)
		{
			material.m_sampler2Ds[MAT_MAP_ENVIRONMENT] = { 0 };
		}
		else if (shader == Shaders::Skybox_HDRI)
		{
			material.m_sampler2Ds[MAT_MAP_ENVIRONMENT] = { 0 };
		}
		else if (shader == Shaders::Skybox_Atmospheric)
		{
			material.m_floats[MAT_TIME] = 0.0f;
			material.m_floats[MAT_CIRRUS] = 0.4f;
			material.m_floats[MAT_CUMULUS] = 0.8f;
			material.m_floats[UF_FLOAT_TIME] = 0.0f;
		}

		else if (shader == Shaders::ScreenQuad_Final)
		{
			material.m_sampler2Ds[MAT_MAP_SCREEN] = { 0 };
			material.m_sampler2Ds[MAT_MAP_BLOOM] = { 1 };
			material.m_sampler2Ds[MAT_MAP_OUTLINE] = { 2 };
			material.m_floats[MAT_EXPOSURE] = 1.0f;
			material.m_floats[MAT_FXAAREDUCEMIN] = 1.0f / 128.0f;
			material.m_floats[MAT_FXAAREDUCEMUL] = 1.0f / 8.0f;
			material.m_floats[MAT_FXAASPANMAX] = 8.0f;
			material.m_bools[MAT_BLOOMENABLED] = false;
			material.m_bools[MAT_FXAAENABLED] = false;
			material.m_vector3s[MAT_INVERSESCREENMAPSIZE] = Vector3();
		}
		else if (shader == Shaders::ScreenQuad_Blur)
		{
			material.m_sampler2Ds[MAT_MAP_SCREEN] = { 0 };
			material.m_bools[MAT_ISHORIZONTAL] = false;
		}
		else if (shader == Shaders::ScreenQuad_Outline)
		{
			material.m_sampler2Ds[MAT_MAP_SCREEN] = { 0 };
		}
		else if (shader == Shaders::ScreenQuad_Shadowmap)
		{

		}
		else if (shader == Shaders::PBR_Lit)
		{
			material.m_sampler2Ds[MAT_TEXTURE2D_ALBEDOMAP] = { 0 };
			material.m_sampler2Ds[MAT_TEXTURE2D_NORMALMAP] = { 1 };
			material.m_sampler2Ds[MAT_TEXTURE2D_ROUGHNESSMAP] = { 2 };
			material.m_sampler2Ds[MAT_TEXTURE2D_METALLICMAP] = { 3 };
			material.m_sampler2Ds[MAT_TEXTURE2D_AOMAP] = { 4 };
			material.m_sampler2Ds[MAT_TEXTURE2D_BRDFLUTMAP] = { 5 };
			// material.m_sampler2Ds[MAT_TEXTURE2D_SHADOWMAP] = { 6 };
			material.m_sampler2Ds[MAT_TEXTURE2D_IRRADIANCEMAP] = { 6, nullptr, "", TextureBindMode::BINDTEXTURE_CUBEMAP, false };
			material.m_sampler2Ds[MAT_TEXTURE2D_PREFILTERMAP] = { 7,nullptr, "", TextureBindMode::BINDTEXTURE_CUBEMAP, false };
			material.m_floats[MAT_METALLICMULTIPLIER] = 1.0f;
			material.m_floats[MAT_ROUGHNESSMULTIPLIER] = 1.0f;
			material.m_ints[MAT_WORKFLOW] = 0;
			material.m_vector2s[MAT_TILING] = Vector2::One;
			material.m_colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.m_receivesLighting = true;
			material.m_isShadowMapped = true;
			material.m_usesHDRI = true;
			// m_shadowMappedMaterials.emplace(&material);
		}
		else if (shader == Shaders::HDRI_Equirectangular)
		{
			material.m_sampler2Ds[MAT_MAP_EQUIRECTANGULAR] = { 0 };
			material.m_matrices[UF_MATRIX_VIEW] = Matrix();
			material.m_matrices[UF_MATRIX_PROJECTION] = Matrix();
		}
		else if (shader == Shaders::Debug_Line)
		{
			material.m_colors[MAT_COLOR] = Color::White;
		}
		else if (shader == Shaders::Standard_Sprite)
		{
			material.m_colors[MAT_OBJECTCOLORPROPERTY] = Color::White;
			material.m_sampler2Ds[MAT_TEXTURE2D_DIFFUSE] = { 0 };
		}


		return material;
	}

	void RenderEngine::SetMaterialContainers(Material& material)
	{
		if(material.m_usesHDRI)
			m_hdriMaterials.emplace(&material);

	}

	void RenderEngine::UnloadTextureResource(int id)
	{
		if (!TextureExists(id))
		{
			LINA_CORE_WARN("Texture not found! Aborting... ");
			return;
		}

		delete m_loadedTextures[id];
		m_loadedTextures.erase(id);
	}

	void RenderEngine::UnloadMaterialResource(int id)
	{
		if (!MaterialExists(id))
		{
			LINA_CORE_WARN("Material not found! Aborting... ");
			return;
		}

		// If its in the internal list, remove first.
		if (m_shadowMappedMaterials.find(&m_loadedMaterials[id]) != m_shadowMappedMaterials.end())
			m_shadowMappedMaterials.erase(&m_loadedMaterials[id]);

		m_loadedMaterials.erase(id);
	}

	bool RenderEngine::MaterialExists(int id)
	{
		if (id < 0) return false;
		return !(m_loadedMaterials.find(id) == m_loadedMaterials.end());
	}

	bool RenderEngine::MaterialExists(const std::string& path)
	{
		const auto it = std::find_if(m_loadedMaterials.begin(), m_loadedMaterials.end(), [path]
		(const auto& it) -> bool { 	return it.second.GetPath().compare(path) == 0; 	});
		return it != m_loadedMaterials.end();
	}

	bool RenderEngine::TextureExists(int id)
	{
		if (id < 0) return false;
		return !(m_loadedTextures.find(id) == m_loadedTextures.end());
	}

	bool RenderEngine::TextureExists(const std::string& path)
	{
		const auto it = std::find_if(m_loadedTextures.begin(), m_loadedTextures.end(), [path]
		(const auto& it) -> bool { 	return it.second->GetPath().compare(path) == 0; 	});
		return it != m_loadedTextures.end();
	}

	void RenderEngine::ConstructEngineShaders()
	{
		// Unlit.
		Shader& unlit = Shader::CreateShader(Shaders::Standard_Unlit, "resources/engine/shaders/Unlit/Unlit.glsl");
		unlit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		unlit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// PBR Lit
		Shader& pbrLit = Shader::CreateShader(Shaders::PBR_Lit, "resources/engine/shaders/PBR/PBRLit.glsl", false);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Skies
		Shader::CreateShader(Shaders::Skybox_SingleColor, "resources/engine/shaders/Skybox/SkyboxColor.glsl");
		Shader::CreateShader(Shaders::Skybox_Gradient, "resources/engine/shaders/Skybox/SkyboxGradient.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::Skybox_Cubemap, "resources/engine/shaders/Skybox/SkyboxCubemap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::Skybox_Procedural, "resources/engine/shaders/Skybox/SkyboxProcedural.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::Skybox_HDRI, "resources/engine/shaders/Skybox/SkyboxHDRI.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::Skybox_Atmospheric, "resources/engine/shaders/Skybox/SkyboxAtmospheric.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);


		// Equirectangular cube & irradiance for HDRI skbox
		Shader::CreateShader(Shaders::HDRI_Equirectangular, "resources/engine/shaders/HDRI/HDRIEquirectangular.glsl");
		Shader::CreateShader(Shaders::HDRI_Irradiance, "resources/engine/shaders/HDRI/HDRIIrradiance.glsl");
		Shader::CreateShader(Shaders::HDRI_Prefilter, "resources/engine/shaders/HDRI/HDRIPrefilter.glsl");
		Shader::CreateShader(Shaders::HDRI_BRDF, "resources/engine/shaders/HDRI/HDRIBRDF.glsl");


		// Screen Quad Shaders
		Shader::CreateShader(Shaders::ScreenQuad_Final, "resources/engine/shaders/ScreenQuads/SQFinal.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::ScreenQuad_Blur, "resources/engine/shaders/ScreenQuads/SQBlur.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::ScreenQuad_Outline, "resources/engine/shaders/ScreenQuads/SQOutline.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader(Shaders::ScreenQuad_Shadowmap, "resources/engine/shaders/ScreenQuads/SQShadowMap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Line
		Shader::CreateShader(Shaders::Debug_Line, "resources/engine/shaders/Misc/DebugLine.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// 2D
		Shader::CreateShader(Shaders::Standard_Sprite, "resources/engine/shaders/2D/Sprite.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
	}

	bool RenderEngine::ValidateEngineShaders()
	{
		int validation = 0;
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Standard_Unlit).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::PBR_Lit).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Skybox_SingleColor).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Skybox_Gradient).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Skybox_Cubemap).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Skybox_Procedural).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Skybox_HDRI).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::HDRI_Equirectangular).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::HDRI_Irradiance).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::HDRI_Prefilter).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::HDRI_BRDF).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::ScreenQuad_Final).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::ScreenQuad_Blur).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::ScreenQuad_Outline).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::ScreenQuad_Shadowmap).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Debug_Line).GetID());
		validation += m_renderDevice.ValidateShaderProgram(Shader::GetShader(Shaders::Standard_Sprite).GetID());

		return !validation;
	}

	void RenderEngine::ConstructEngineMaterials()
	{
		SetMaterialShader(m_screenQuadFinalMaterial, Shaders::ScreenQuad_Final);
		SetMaterialShader(m_screenQuadBlurMaterial, Shaders::ScreenQuad_Blur);
		SetMaterialShader(m_screenQuadOutlineMaterial, Shaders::ScreenQuad_Outline);
		SetMaterialShader(m_hdriMaterial, Shaders::HDRI_Equirectangular);
		SetMaterialShader(m_debugDrawMaterial, Shaders::Debug_Line);
		SetMaterialShader(m_shadowMapMaterial, Shaders::ScreenQuad_Shadowmap);
		SetMaterialShader(m_defaultSkyboxMaterial, Shaders::Skybox_SingleColor);
		SetMaterialShader(m_defaultUnlit, Shaders::Standard_Unlit);
	}

	void RenderEngine::ConstructEnginePrimitives()
	{
		// Primitives
		Mesh::CreateMesh("resources/engine/meshes/primitives/cube.obj", MeshParameters(), Primitives::Cube);
		Mesh::CreateMesh("resources/engine/meshes/primitives/cylinder.obj", MeshParameters(), Primitives::Cylinder);
		Mesh::CreateMesh("resources/engine/meshes/primitives/plane.obj", MeshParameters(), Primitives::Plane);
		Mesh::CreateMesh("resources/engine/meshes/primitives/sphere.obj", MeshParameters(), Primitives::Sphere);
		Mesh::CreateMesh("resources/engine/meshes/primitives/icosphere.obj", MeshParameters(), Primitives::Icosphere);
		Mesh::CreateMesh("resources/engine/meshes/primitives/cone.obj", MeshParameters(), Primitives::Cone);
	}

	void RenderEngine::ConstructRenderTargets()
	{

		// Main
		m_mainRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
		m_mainRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
		m_mainRTParams.m_textureParams.m_minFilter = m_mainRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		m_mainRTParams.m_textureParams.m_wrapS = m_mainRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_REPEAT;

		// Primary
		m_primaryRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
		m_primaryRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		m_primaryRTParams.m_textureParams.m_minFilter = m_primaryRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		m_primaryRTParams.m_textureParams.m_wrapS = m_primaryRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;


		// Ping pong
		m_pingPongRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
		m_pingPongRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		m_pingPongRTParams.m_textureParams.m_minFilter = m_pingPongRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		m_pingPongRTParams.m_textureParams.m_wrapS = m_pingPongRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		// Shadows depth.
		m_shadowsRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_DEPTH;
		m_shadowsRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_DEPTH16;
		m_shadowsRTParams.m_textureParams.m_minFilter = m_shadowsRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_NEAREST;
		m_shadowsRTParams.m_textureParams.m_wrapS = m_shadowsRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_BORDER;

		// Initialize primary RT textures
		m_primaryRTTexture0.ConstructRTTexture(m_renderDevice, m_viewportSize, m_primaryRTParams, false);
		m_primaryRTTexture1.ConstructRTTexture(m_renderDevice, m_viewportSize, m_primaryRTParams, false);

		// Initialize ping pong rt texture
		m_pingPongRTTexture1.ConstructRTTexture(m_renderDevice, m_viewportSize, m_pingPongRTParams, false);
		m_pingPongRTTexture2.ConstructRTTexture(m_renderDevice, m_viewportSize, m_pingPongRTParams, false);

		// Initialize outilne RT texture
		//m_OutlineRTTexture.ConstructRTTexture(m_renderDevice, screenSize, primaryRTParams, false);

		// Shadow map RT texture
		m_shadowMapRTTexture.ConstructRTTexture(m_renderDevice, m_shadowMapResolution, m_shadowsRTParams, true);

		// Initialize primary render buffer
		m_primaryRenderBuffer.Construct(m_renderDevice, RenderBufferStorage::STORAGE_DEPTH, m_viewportSize);

		// Initialize hdri render buffer
		m_hdriCaptureRenderBuffer.Construct(m_renderDevice, RenderBufferStorage::STORAGE_DEPTH_COMP24, m_hdriResolution);

		// Initialize primary render target.
		m_primaryRenderTarget.Construct(m_renderDevice, m_primaryRTTexture0, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_primaryRenderBuffer.GetID());

		// Bind the extre texture to primary render target, also tell open gl that we are running mrts.
		m_renderDevice.BindTextureToRenderTarget(m_primaryRenderTarget.GetID(), m_primaryRTTexture1.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
		uint32 attachments[2] = { FrameBufferAttachment::ATTACHMENT_COLOR , (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1) };
		m_renderDevice.MultipleDrawBuffersCommand(m_primaryRenderTarget.GetID(), 2, attachments);

		// Initialize ping pong render targets
		m_pingPongRenderTarget1.Construct(m_renderDevice, m_pingPongRTTexture1, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		m_pingPongRenderTarget2.Construct(m_renderDevice, m_pingPongRTTexture2, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

		// Initialize outline render target
		//m_OutlineRenderTarget.Construct(m_renderDevice, m_OutlineRTTexture, m_viewportSize.x, m_viewportSize.y, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

		// Initialize HDRI render target
		m_hdriCaptureRenderTarget.Construct(m_renderDevice, m_hdriResolution, FrameBufferAttachment::ATTACHMENT_DEPTH, m_hdriCaptureRenderBuffer.GetID());

		// Initialize depth map for shadows
		m_shadowMapTarget.Construct(m_renderDevice, m_shadowMapRTTexture, m_shadowMapResolution, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

	}

	void RenderEngine::DumpMemory()
	{
		// Clear dumps.
		Mesh::UnloadAll();
		m_loadedTextures.clear();
		m_loadedMaterials.clear();
	}

	void RenderEngine::DrawShadows()
	{
		// Clear color.
		m_renderDevice.Clear(true, true, false, m_cameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_renderingPipeline.UpdateSystems(0.0f);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Set depth frame 
		m_renderDevice.SetFBO(m_shadowMapTarget.GetID());
		m_renderDevice.SetViewport(Vector2::Zero, m_shadowMapResolution);

		// Clear color.
		m_renderDevice.Clear(false, true, false, m_cameraSystem.GetCurrentClearColor(), 0xFF);

		// Draw scene
		DrawSceneObjects(m_shadowMapDrawParams, &m_shadowMapMaterial, false);

		// Add the shadow texture
		for (std::set<Material*>::iterator it = m_shadowMappedMaterials.begin(); it != m_shadowMappedMaterials.end(); ++it)
			(*it)->SetTexture(MAT_TEXTURE2D_SHADOWMAP, &m_shadowMapRTTexture);

	}

	void RenderEngine::Draw()
	{
		// Set render target
		m_renderDevice.SetFBO(m_primaryRenderTarget.GetID());
		m_renderDevice.SetViewport(Vector2::Zero, m_viewportSize);

		// Clear color.
		m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_renderingPipeline.UpdateSystems(0.0f);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(m_defaultDrawParams);

		bool horizontal = true;

		if (m_screenQuadFinalMaterial.m_bools[MAT_BLOOMENABLED])
		{
			// Write to the pingpong buffers to apply 2 pass gaussian blur.
			bool firstIteration = true;
			unsigned int amount = 4;
			for (unsigned int i = 0; i < amount; i++)
			{
				// Select FBO
				m_renderDevice.SetFBO(horizontal ? m_pingPongRenderTarget1.GetID() : m_pingPongRenderTarget2.GetID());

				// Setup material & use.
				m_screenQuadBlurMaterial.SetBool(MAT_ISHORIZONTAL, horizontal);
				if (firstIteration)
					m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_primaryRTTexture1);
				else
				{
					if (horizontal)
						m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_pingPongRTTexture2);
					else
						m_screenQuadBlurMaterial.SetTexture(MAT_MAP_SCREEN, &m_pingPongRTTexture1);
				}

				// Update shader data & draw.
				UpdateShaderData(&m_screenQuadBlurMaterial);
				m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
				horizontal = !horizontal;
				if (firstIteration) firstIteration = false;
			}
		}

		// Back to default buffer
		m_renderDevice.SetFBO(0);
		m_renderDevice.SetViewport(m_viewportPos, m_viewportSize);

		// Clear color bit.
		m_renderDevice.Clear(true, true, false, Color::White, 0xFF);

		// Set frame buffer texture on the material.
		m_screenQuadFinalMaterial.SetTexture(MAT_MAP_SCREEN, &m_primaryRTTexture0, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		if (m_screenQuadFinalMaterial.m_bools[MAT_BLOOMENABLED])
			m_screenQuadFinalMaterial.SetTexture(MAT_MAP_BLOOM, horizontal ? &m_pingPongRTTexture1 : &m_pingPongRTTexture2, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		// m_ScreenQuadFinalMaterial.SetTexture(MAT_MAP_OUTLINE, &m_OutlineRTTexture, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		Vector2 inverseMapSize = 1.0f / m_primaryRTTexture0.GetSize();
		m_screenQuadFinalMaterial.SetVector3(MAT_INVERSESCREENMAPSIZE, Vector3(inverseMapSize.x, inverseMapSize.y, 0.0));

		// update shader w/ material data.
		UpdateShaderData(&m_screenQuadFinalMaterial);

		// Draw full screen quad.
		m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
	}

	void RenderEngine::DrawLine(Vector3 p1, Vector3 p2, Color col, float width)
	{
		m_renderDevice.SetShader(m_debugDrawMaterial.m_shaderID);
		m_renderDevice.UpdateShaderUniformColor(m_debugDrawMaterial.m_shaderID, MAT_COLOR, col);
		m_renderDevice.DrawLine(m_debugDrawMaterial.m_shaderID, Matrix::Identity(), p1, p2, width);
	}

	void RenderEngine::SetDrawParameters(const DrawParams& params)
	{
		m_renderDevice.SetDrawParameters(params);
	}

	void RenderEngine::DrawOperationsDefault()
	{
		m_renderDevice.SetFBO(0);

		// Clear color.
		m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);

		// Update pipeline.
		m_renderingPipeline.UpdateSystems(0.0f);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

		// Draw scene
		DrawSceneObjects(m_defaultDrawParams, nullptr, true);
	}

	void RenderEngine::DrawSkybox()
	{
		if (m_skyboxMaterial != nullptr)
		{
			UpdateShaderData(m_skyboxMaterial);
			m_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 36, true);
		}
		else
		{
			UpdateShaderData(&m_defaultSkyboxMaterial);
			m_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 36, true);
		}
	}

	void RenderEngine::DrawSceneObjects(DrawParams& drawParams, Material* overrideMaterial, bool drawSkybox)
	{
		m_meshRendererSystem.FlushOpaque(drawParams, overrideMaterial, true);
		m_meshRendererSystem.FlushTransparent(drawParams, overrideMaterial, true);
		m_spriteRendererSystem.Flush(drawParams, overrideMaterial, true);

		// Draw skybox.
		if (drawSkybox)
			DrawSkybox();

		// Post scene draw callback.
		if (m_postSceneDrawCallback)
			m_postSceneDrawCallback();

	}

	void RenderEngine::UpdateUniformBuffers()
	{
		Vector3 cameraLocation = m_cameraSystem.GetCameraLocation();
		Vector4 viewPos = Vector4(cameraLocation.x, cameraLocation.y, cameraLocation.z, 1.0f);

		// Update global matrix buffer
		uintptr currentGlobalDataOffset = 0;
		m_globalDataBuffer.Update(&m_cameraSystem.GetProjectionMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_globalDataBuffer.Update(&m_cameraSystem.GetViewMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_globalDataBuffer.Update(&m_cameraSystem.GetLightMatrix(m_lightingSystem.GetDirLight())[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_globalDataBuffer.Update(&viewPos, currentGlobalDataOffset, sizeof(Vector4));
		currentGlobalDataOffset += sizeof(Vector4);

		ECS::CameraComponent* cameraComponent = m_cameraSystem.GetCurrentCameraComponent();

		if (cameraComponent != nullptr)
		{
			// Update only if changed.
			if (m_bufferValueRecord.zNear != cameraComponent->m_zNear)
			{
				m_bufferValueRecord.zNear = cameraComponent->m_zNear;
				m_globalDataBuffer.Update(&cameraComponent->m_zNear, currentGlobalDataOffset, sizeof(float));
			}
			currentGlobalDataOffset += sizeof(float);


			// Update only if changed.
			if (m_bufferValueRecord.zFar != cameraComponent->m_zFar)
			{
				m_bufferValueRecord.zFar = cameraComponent->m_zFar;
				m_globalDataBuffer.Update(&cameraComponent->m_zNear, currentGlobalDataOffset, sizeof(float));
			}
			currentGlobalDataOffset += sizeof(float);
		}

		// Update lights buffer.
		Color ambient = m_lightingSystem.GetAmbientColor();
		Vector4 ambientColor = Vector4(ambient.r, ambient.g, ambient.b, 1.0f);
		m_globalLightBuffer.Update(&m_currentPointLightCount, 0, sizeof(int));
		m_globalLightBuffer.Update(&m_currentSpotLightCount, sizeof(int), sizeof(int));
		m_globalLightBuffer.Update(&ambientColor, sizeof(int) * 2, sizeof(float) * 4);
		m_globalLightBuffer.Update(&m_cameraSystem.GetCameraLocation(), (sizeof(int) * 2) + (sizeof(float) * 4), sizeof(float) * 4);

		// Update debug fufer.
		m_globalDebugBuffer.Update(&m_debugData.visualizeDepth, 0, sizeof(bool));
	}

	void RenderEngine::UpdateShaderData(Material* data)
	{

		m_renderDevice.SetShader(data->GetShaderID());

		for (auto const& d : (*data).m_floats)
			m_renderDevice.UpdateShaderUniformFloat(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_bools)
			m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_colors)
			m_renderDevice.UpdateShaderUniformColor(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_ints)
			m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_vector2s)
			m_renderDevice.UpdateShaderUniformVector2(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_vector3s)
			m_renderDevice.UpdateShaderUniformVector3(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_vector4s)
			m_renderDevice.UpdateShaderUniformVector4F(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_matrices)
			m_renderDevice.UpdateShaderUniformMatrix(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_sampler2Ds)
		{
			// Set whether the texture is active or not.
			bool isActive = (d.second.m_isActive && d.second.m_boundTexture != nullptr && !d.second.m_boundTexture->GetIsEmpty()) ? true : false;
			m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first + MAT_EXTENSION_ISACTIVE, isActive);

			// Set the texture to corresponding active unit.
			m_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first + MAT_EXTENSION_TEXTURE2D, d.second.m_unit);

			// Set texture
			if (isActive)
				m_renderDevice.SetTexture(d.second.m_boundTexture->GetID(), d.second.m_boundTexture->GetSamplerID(), d.second.m_unit, d.second.m_bindMode, true);
			else
			{

				if (d.second.m_bindMode == TextureBindMode::BINDTEXTURE_TEXTURE2D)
					m_renderDevice.SetTexture(m_defaultTexture.GetID(), m_defaultTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_TEXTURE2D);
				else
					m_renderDevice.SetTexture(m_defaultCubemapTexture.GetID(), m_defaultCubemapTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_CUBEMAP);
			}
		}


		if (data->m_receivesLighting)
			m_lightingSystem.SetLightingShaderData(data->GetShaderID());

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
		m_renderDevice.SetFBO(0);
		m_renderDevice.SetViewport(m_viewportPos, m_viewportSize);

		// Set flag
		m_hdriDataCaptured = true;

		for (Material* mat : m_hdriMaterials)
		{
			if (mat != nullptr)
				SetHDRIData(mat);
		}
	}

	void RenderEngine::CalculateHDRICubemap(Texture& hdriTexture, glm::mat4& captureProjection, glm::mat4 views[6])
	{
		// Generate sampler.
		SamplerParameters samplerParams;
		samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution.
		m_hdriResolution = Vector2(512, 512);

		// Construct Cubemap texture.
		m_hdriCubemap.ConstructRTCubemapTexture(m_renderDevice, m_hdriResolution, samplerParams);

		// Setup shader data.
		uint32 equirectangularShader = Shader::GetShader(Shaders::HDRI_Equirectangular).GetID();
		m_renderDevice.SetShader(equirectangularShader);
		m_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		m_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_ISACTIVE), 1);
		m_renderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_PROJECTION, captureProjection);
		m_renderDevice.SetTexture(hdriTexture.GetID(), hdriTexture.GetSamplerID(), 0);
		m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
		m_renderDevice.SetViewport(Vector2::Zero, m_hdriResolution);

		// Draw the cubemap.
		for (uint32 i = 0; i < 6; ++i)
		{
			m_renderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_VIEW, views[i]);
			m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false);
			m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
			m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
			m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
		}

		// Generate mipmaps & check errors.
		m_renderDevice.GenerateTextureMipmaps(m_hdriCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP);
		m_renderDevice.IsRenderTargetComplete(m_hdriCaptureRenderTarget.GetID());
	}

	void RenderEngine::CalculateHDRIIrradiance(Matrix& captureProjection, Matrix views[6])
	{
		// Generate sampler.
		SamplerParameters irradianceParams;
		irradianceParams.m_textureParams.m_wrapR = irradianceParams.m_textureParams.m_wrapS = irradianceParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		irradianceParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		irradianceParams.m_textureParams.m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		irradianceParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		irradianceParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution
		Vector2 irradianceMapResolsution = Vector2(32, 32);

		// Create irradiance texture & scale render buffer according to the resolution.
		m_hdriIrradianceMap.ConstructRTCubemapTexture(m_renderDevice, irradianceMapResolsution, irradianceParams);
		m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
		m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), irradianceMapResolsution, RenderBufferStorage::STORAGE_DEPTH_COMP24);

		// Create & setup shader info.
		uint32 irradianceShader = Shader::GetShader(Shaders::HDRI_Irradiance).GetID();
		m_renderDevice.SetShader(irradianceShader);
		m_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		m_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
		m_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_PROJECTION, captureProjection);
		m_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);
		m_renderDevice.SetViewport(Vector2::Zero, irradianceMapResolsution);

		// Draw cubemap.
		for (uint32 i = 0; i < 6; ++i)
		{
			m_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_VIEW, views[i]);
			m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriIrradianceMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false, false);
			m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
			m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
		}
	}

	void RenderEngine::CalculateHDRIPrefilter(Matrix& captureProjection, Matrix views[6])
	{
		// Generate sampler.
		SamplerParameters prefilterParams;
		prefilterParams.m_textureParams.m_generateMipMaps = true;
		prefilterParams.m_textureParams.m_wrapR = prefilterParams.m_textureParams.m_wrapS = prefilterParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		prefilterParams.m_textureParams.m_minFilter = SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR;
		prefilterParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		prefilterParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		prefilterParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution
		Vector2 prefilterResolution = Vector2(128, 128);

		// Construct prefilter texture.
		m_hdriPrefilterMap.ConstructRTCubemapTexture(m_renderDevice, prefilterResolution, prefilterParams);

		// Setup shader data.
		uint32 prefilterShader = Shader::GetShader(Shaders::HDRI_Prefilter).GetID();
		m_renderDevice.SetShader(prefilterShader);
		m_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		m_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
		m_renderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ENVIRONMENTRESOLUTION, 512.0f);
		m_renderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_PROJECTION, captureProjection);
		m_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);

		// Setup mip levels & switch fbo.
		uint32 maxMipLevels = 5;
		m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());

		for (uint32 mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), Vector2(mipWidth, mipHeight), RenderBufferStorage::STORAGE_DEPTH_COMP24);
			m_renderDevice.SetViewport(Vector2::Zero, Vector2(mipWidth, mipHeight));

			// Draw prefiltered map
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			m_renderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ROUGHNESSMULTIPLIER, roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				m_renderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_VIEW, views[i]);
				m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriPrefilterMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, mip, false, false);
				m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
				m_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
			}
		}
	}

	void RenderEngine::CalculateHDRIBRDF(Matrix& captureProjection, Matrix views[6])
	{
		// Generate sampler.
		SamplerParameters samplerParams;
		samplerParams.m_textureParams.m_wrapR = samplerParams.m_textureParams.m_wrapS = samplerParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;
		samplerParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_minFilter = SamplerFilter::FILTER_LINEAR;
		samplerParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGB16F;
		samplerParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;

		// Set resolution.
		Vector2 brdfLutSize = Vector2(512, 512);

		// Create BRDF texture.
		m_HDRILutMap.ConstructHDRI(m_renderDevice, samplerParams, brdfLutSize, NULL);

		// Scale render buffer according to the resolution & bind lut map to frame buffer.
		m_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), brdfLutSize, RenderBufferStorage::STORAGE_DEPTH_COMP24);
		m_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_HDRILutMap.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, 0, true, false);

		// Setup shader.
		uint32 brdfShader = Shader::GetShader(Shaders::HDRI_BRDF).GetID();
		m_renderDevice.SetShader(brdfShader);

		// Switch framebuffer & draw.
		m_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
		m_renderDevice.SetViewport(Vector2::Zero, brdfLutSize);
		m_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
		m_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
	}

	void RenderEngine::SetHDRIData(Material* mat)
	{
		if (mat == nullptr)
		{
			LINA_CORE_WARN("Material to set HDRI data is null, returning...");
			return;
		}

		if (!m_hdriDataCaptured)
		{
			LINA_CORE_WARN("HDRI data is not captured, please capture it first then set the material's data.");
			return;
		}

		mat->SetTexture(MAT_TEXTURE2D_IRRADIANCEMAP, &m_hdriIrradianceMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
		mat->SetTexture(MAT_TEXTURE2D_BRDFLUTMAP, &m_HDRILutMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);
		mat->SetTexture(MAT_TEXTURE2D_PREFILTERMAP, &m_hdriPrefilterMap, TextureBindMode::BINDTEXTURE_CUBEMAP);
	}

	void RenderEngine::RemoveHDRIData(Material* mat)
	{
		if (mat == nullptr)
		{
			LINA_CORE_WARN("Material to remove HDRI data from is null, returning...");
			return;
		}

		mat->RemoveTexture(MAT_TEXTURE2D_IRRADIANCEMAP);
		mat->RemoveTexture(MAT_TEXTURE2D_BRDFLUTMAP);		mat->RemoveTexture(MAT_TEXTURE2D_PREFILTERMAP);

	}

	void RenderEngine::PushLayer(Layer& layer)
	{
		m_guiLayerStack.PushLayer(layer);
	}

	void RenderEngine::PushOverlay(Layer& layer)
	{
		m_guiLayerStack.PushOverlay(layer);
	}

	void* RenderEngine::GetFinalImage()
	{
		return (void*)m_primaryRTTexture0.GetID();
	}

	void* RenderEngine::GetShadowMapImage()
	{
		return (void*)m_shadowMapRTTexture.GetID();
	}

}