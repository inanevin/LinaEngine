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
#include "ECS/ECS.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "PackageManager/OpenGL/GLRenderDevice.hpp"
#include "Helpers/DrawParameterHelper.hpp"
#include "Core/Timer.hpp"

namespace LinaEngine::Graphics
{
	RenderDevice RenderEngine::s_renderDevice;
	Texture RenderEngine::s_defaultTexture;
	Material RenderEngine::s_defaultUnlit;
	Shader* RenderEngine::s_standardUnlitShader;

	constexpr size_t UNIFORMBUFFER_VIEWDATA_SIZE = (sizeof(Matrix) * 4) + (sizeof(Vector4)) + (sizeof(float) * 2);
	constexpr int UNIFORMBUFFER_VIEWDATA_BINDPOINT = 0;
	constexpr auto UNIFORMBUFFER_VIEWDATA_NAME = "ViewData";

	constexpr size_t UNIFORMBUFFER_LIGHTDATA_SIZE = (sizeof(int) * 2) + sizeof(float) * 4;
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
		m_skyboxVAO = s_renderDevice.ReleaseVertexArray(m_skyboxVAO);
		m_screenQuadVAO = s_renderDevice.ReleaseVertexArray(m_screenQuadVAO);
		m_hdriCubeVAO = s_renderDevice.ReleaseVertexArray(m_hdriCubeVAO);
		m_lineVAO = s_renderDevice.ReleaseVertexArray(m_lineVAO);

		LINA_CORE_TRACE("[Destructor] -> RenderEngine ({0})", typeid(*this).name());
	}

	void RenderEngine::Initialize(LinaEngine::ECS::ECSRegistry& ecsReg, Window& appWindow)
	{
		if (Utility::FileExists(RENDERSETTINGS_FULLPATH))
			m_renderSettings = RenderSettings::DeserializeRenderSettings(RENDERSETTINGS_FOLDERPATH, RENDERSETTINGS_FILE);

		// Register ECS components
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::CameraComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::PointLightComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::SpotLightComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::DirectionalLightComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::MeshRendererComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::ModelRendererComponent>();
		ecsReg.RegisterComponentToClone<LinaEngine::ECS::SpriteRendererComponent>();

		// Set references.
		m_appWindow = &appWindow;

		// Flip loaded images.
		ArrayBitmap::SetImageFlip(true);

		// Setup draw parameters.
		m_defaultDrawParams = DrawParameterHelper::GetDefault();
		m_skyboxDrawParams = DrawParameterHelper::GetSkybox();
		m_fullscreenQuadDP = DrawParameterHelper::GetFullScreenQuad();
		m_shadowMapDrawParams = DrawParameterHelper::GetShadowMap();


		// Initialize the render device.
		s_renderDevice.Initialize(m_appWindow->GetWidth(), m_appWindow->GetHeight(), m_defaultDrawParams);

		// Construct the uniform buffer for global matrices.
		m_globalDataBuffer.Construct(s_renderDevice, UNIFORMBUFFER_VIEWDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_globalDataBuffer.Bind(UNIFORMBUFFER_VIEWDATA_BINDPOINT);

		// Construct the uniform buffer for lights.
		m_globalLightBuffer.Construct(s_renderDevice, UNIFORMBUFFER_LIGHTDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_globalLightBuffer.Bind(UNIFORMBUFFER_LIGHTDATA_BINDPOINT);

		// Construct the uniform buffer for debugging.
		m_globalDebugBuffer.Construct(s_renderDevice, UNIFORMBUFFER_DEBUGDATA_SIZE, BufferUsage::USAGE_DYNAMIC_DRAW, NULL);
		m_globalDebugBuffer.Bind(UNIFORMBUFFER_DEBUGDATA_BINDPOINT);

		// Initialize the engine shaders.
		ConstructEngineShaders();

		// Initialize engine materials
		ConstructEngineMaterials();

		// Initialize engine vertex arrays.
		ConstructEnginePrimitives();

		// Initialize built-in vertex array objects.
		m_skyboxVAO = s_renderDevice.CreateSkyboxVertexArray();
		m_hdriCubeVAO = s_renderDevice.CreateHDRICubeVertexArray();
		m_screenQuadVAO = s_renderDevice.CreateScreenQuadVertexArray();
		m_lineVAO = s_renderDevice.CreateLineVertexArray();

		// Construct render targets
		ConstructRenderTargets();

		// Build default textures.
		s_defaultTexture.ConstructEmpty(s_renderDevice);
		m_defaultCubemapTexture.ConstructRTCubemapTexture(s_renderDevice, m_viewportSize, SamplerParameters());

		// Initialize ECS Camera System.
		m_cameraSystem.Construct(ecsReg);
		m_cameraSystem.SetAspectRatio((float)m_viewportSize.x / (float)m_viewportSize.y);

		// Initialize ECS Mesh Renderer System
		m_meshRendererSystem.Construct(ecsReg, *this, s_renderDevice);
		m_spriteRendererSystem.Construct(ecsReg, *this, s_renderDevice);

		// Initialize ECS Lighting system.
		m_lightingSystem.Construct(ecsReg, s_renderDevice, *this);
	
		// Add the ECS systems into the pipeline.
		m_renderingPipeline.AddSystem(m_cameraSystem);
		m_renderingPipeline.AddSystem(m_meshRendererSystem);
		m_renderingPipeline.AddSystem(m_spriteRendererSystem);
		m_renderingPipeline.AddSystem(m_lightingSystem);

		// Initialize Animation System.
		m_animationSystem.Construct(ecsReg, this);

		// Animation pipeline
		m_animationPipeline.AddSystem(m_animationSystem);

		// Set debug values.
		m_debugData.visualizeDepth = false;

	}

	void RenderEngine::Tick(float delta)
	{
		m_animationPipeline.UpdateSystems(delta);
	}

	void RenderEngine::Render()
	{

		if (m_preDrawCallback)
			m_preDrawCallback();


		Draw();

		if (m_postDrawCallback)
			m_postDrawCallback();

		if (!m_firstFrameDrawn)
		{
			m_firstFrameDrawn = true;
		}

	}

	void RenderEngine::RenderLayers()
	{
		s_renderDevice.SetFBO(0);
		s_renderDevice.SetViewport(m_viewportPos, m_viewportSize);

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
		s_renderDevice.SetViewport(pos, size);
		m_viewportPos = pos;
		m_viewportSize = size;

		m_cameraSystem.SetAspectRatio((float)m_viewportSize.x / (float)m_viewportSize.y);

		// Resize render buffers & frame buffer textures
		s_renderDevice.ResizeRTTexture(m_pingPongRTTexture1.GetID(), m_viewportSize, m_pingPongRTParams.m_textureParams.m_internalPixelFormat, m_pingPongRTParams.m_textureParams.m_pixelFormat);
		s_renderDevice.ResizeRTTexture(m_pingPongRTTexture1.GetID(), m_viewportSize, m_pingPongRTParams.m_textureParams.m_internalPixelFormat, m_pingPongRTParams.m_textureParams.m_pixelFormat);
		s_renderDevice.ResizeRTTexture(m_primaryMSAARTTexture0.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
		s_renderDevice.ResizeRTTexture(m_primaryMSAARTTexture1.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
		s_renderDevice.ResizeRTTexture(m_primaryRTTexture0.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
		s_renderDevice.ResizeRTTexture(m_primaryRTTexture1.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);

		for (auto& p : m_postProcessMap)
		{
			SamplerParameters sp = p.second.GetParams();
			s_renderDevice.ResizeRTTexture(p.second.GetTexture().GetID(), m_viewportSize, sp.m_textureParams.m_internalPixelFormat, sp.m_textureParams.m_pixelFormat);
		}

#ifdef LINA_EDITOR
		s_renderDevice.ResizeRTTexture(m_secondaryRTTexture.GetID(), m_viewportSize, m_primaryRTParams.m_textureParams.m_internalPixelFormat, m_primaryRTParams.m_textureParams.m_pixelFormat);
#endif
	}


	void RenderEngine::MaterialUpdated(Material& mat)
	{
		std::set<Material*>& hdriMaterials = Material::GetHDRIMaterials();
		if (!mat.m_usesHDRI && hdriMaterials.find(&mat) != hdriMaterials.end())
		{
			hdriMaterials.erase(&mat);
			RemoveHDRIData(&mat);
		}
		else if (mat.m_usesHDRI && hdriMaterials.find(&mat) == hdriMaterials.end())
		{
			hdriMaterials.emplace(&mat);
			SetHDRIData(&mat);
		}

		std::set<Material*>& shadowMappedMaterials = Material::GetShadowMappedMaterials();
		if (!mat.m_isShadowMapped && shadowMappedMaterials.find(&mat) != shadowMappedMaterials.end())
		{
			shadowMappedMaterials.erase(&mat);
			mat.RemoveTexture(MAT_TEXTURE2D_SHADOWMAP);
		}
		else if (mat.m_isShadowMapped && shadowMappedMaterials.find(&mat) == shadowMappedMaterials.end())
		{
			shadowMappedMaterials.emplace(&mat);
			mat.SetTexture(MAT_TEXTURE2D_SHADOWMAP, nullptr);
		}

	}

	void RenderEngine::ConstructEngineShaders()
	{
		// Unlit.
		s_standardUnlitShader = &Shader::CreateShader("resources/engine/shaders/Unlit/Unlit.glsl");
		s_standardUnlitShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		s_standardUnlitShader->BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// PBR Lit
		Shader& pbrLit = Shader::CreateShader("resources/engine/shaders/PBR/PBRLitStandard.glsl", false);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
		pbrLit.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);

		// Point shadows depth shader
		m_pointShadowsDepthShader = &Shader::CreateShader("resources/engine/shaders/PBR/PointShadowsDepth.glsl", true);

		// Skies
		m_skyboxSingleColorShader = &Shader::CreateShader("resources/engine/shaders/Skybox/SkyboxColor.glsl");
		Shader::CreateShader("resources/engine/shaders/Skybox/SkyboxGradient.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader("resources/engine/shaders/Skybox/SkyboxCubemap.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader("resources/engine/shaders/Skybox/SkyboxProcedural.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader("resources/engine/shaders/Skybox/SkyboxHDRI.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		Shader::CreateShader("resources/engine/shaders/Skybox/SkyboxAtmospheric.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// Equirectangular cube & irradiance for HDRI skbox
		m_hdriEquirectangularShader = &Shader::CreateShader("resources/engine/shaders/HDRI/HDRIEquirectangular.glsl");
		m_hdriIrradianceShader = &Shader::CreateShader("resources/engine/shaders/HDRI/HDRIIrradiance.glsl");
		m_hdriPrefilterShader = &Shader::CreateShader("resources/engine/shaders/HDRI/HDRIPrefilter.glsl");
		m_hdriBRDFShader = &Shader::CreateShader("resources/engine/shaders/HDRI/HDRIBRDF.glsl");


		// Screen Quad Shaders
		m_sqFinalShader = &Shader::CreateShader("resources/engine/shaders/ScreenQuads/SQFinal.glsl");
		m_sqFinalShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		m_sqBlurShader = &Shader::CreateShader("resources/engine/shaders/ScreenQuads/SQBlur.glsl");
		m_sqBlurShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
		m_sqShadowMapShader = &Shader::CreateShader("resources/engine/shaders/ScreenQuads/SQShadowMap.glsl");
		m_sqShadowMapShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);;

		// Line
		m_debugLineShader = &Shader::CreateShader("resources/engine/shaders/Misc/DebugLine.glsl");
		m_debugLineShader->BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);

		// 2D
		Shader::CreateShader("resources/engine/shaders/2D/Sprite.glsl").BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
	}

	bool RenderEngine::ValidateEngineShaders()
	{
		bool validated = false;

		std::map<int, Shader*>& loadedShaders = Shader::GetLoadedShaders();

		for (std::map<int, Shader*>::iterator it = loadedShaders.begin(); it != loadedShaders.end(); ++it)
		{
			LINA_CORE_TRACE("Validating {0}", it->second->GetPath());
			bool success = s_renderDevice.ValidateShaderProgram(it->second->GetID());

			if (!success)
				LINA_CORE_TRACE("Failed validation");

			validated |= success;
		}

		return !validated;
	}

	void RenderEngine::ConstructEngineMaterials()
	{
		Material::SetMaterialShader(m_screenQuadFinalMaterial, *m_sqFinalShader);
		Material::SetMaterialShader(m_screenQuadBlurMaterial, *m_sqBlurShader);
		Material::SetMaterialShader(m_hdriMaterial, *m_hdriEquirectangularShader);
		Material::SetMaterialShader(m_debugDrawMaterial, *m_debugLineShader);
		Material::SetMaterialShader(m_shadowMapMaterial, *m_sqShadowMapShader);
		Material::SetMaterialShader(m_defaultSkyboxMaterial, *m_skyboxSingleColorShader);
		Material::SetMaterialShader(s_defaultUnlit, *s_standardUnlitShader);
		Material::SetMaterialShader(m_pLightShadowDepthMaterial, *m_pointShadowsDepthShader);

		UpdateRenderSettings();
	}

	void RenderEngine::ConstructEnginePrimitives()
	{
		// Primitives
		Model::CreateModel("resources/engine/meshes/primitives/cube.fbx", ModelParameters(), Primitives::Cube);
		Model::CreateModel("resources/engine/meshes/primitives/cylinder.fbx", ModelParameters(), Primitives::Cylinder);
		Model::CreateModel("resources/engine/meshes/primitives/capsule.fbx", ModelParameters(), Primitives::Capsule);
		Model::CreateModel("resources/engine/meshes/primitives/quad.fbx", ModelParameters(), Primitives::Quad);
		Model::CreateModel("resources/engine/meshes/primitives/plane.fbx", ModelParameters(), Primitives::Plane);
		Model::CreateModel("resources/engine/meshes/primitives/sphere.fbx", ModelParameters(), Primitives::Sphere);
	}

	void RenderEngine::ConstructRenderTargets()
	{
		// Primary
		m_primaryRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
		m_primaryRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
		m_primaryRTParams.m_textureParams.m_minFilter = m_primaryRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		m_primaryRTParams.m_textureParams.m_wrapS = m_primaryRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		// Ping pong
		m_pingPongRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_RGB;
		m_pingPongRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_RGBA16F;
		m_pingPongRTParams.m_textureParams.m_minFilter = m_pingPongRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_LINEAR;
		m_pingPongRTParams.m_textureParams.m_wrapS = m_pingPongRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		// Shadows depth.
		m_shadowsRTParams.m_textureParams.m_pixelFormat = PixelFormat::FORMAT_DEPTH;
		m_shadowsRTParams.m_textureParams.m_internalPixelFormat = PixelFormat::FORMAT_DEPTH;
		m_shadowsRTParams.m_textureParams.m_minFilter = m_shadowsRTParams.m_textureParams.m_magFilter = SamplerFilter::FILTER_NEAREST;
		m_shadowsRTParams.m_textureParams.m_wrapS = m_shadowsRTParams.m_textureParams.m_wrapR = m_shadowsRTParams.m_textureParams.m_wrapT = SamplerWrapMode::WRAP_CLAMP_EDGE;

		// Initialize primary RT textures
		m_primaryMSAARTTexture0.ConstructRTTextureMSAA(s_renderDevice, m_viewportSize, m_primaryRTParams, 4);
		m_primaryMSAARTTexture1.ConstructRTTextureMSAA(s_renderDevice, m_viewportSize, m_primaryRTParams, 4);
		m_primaryRTTexture0.ConstructRTTexture(s_renderDevice, m_viewportSize, m_primaryRTParams, false);
		m_primaryRTTexture1.ConstructRTTexture(s_renderDevice, m_viewportSize, m_primaryRTParams, false);

		// Initialize ping pong rt texture
		m_pingPongRTTexture1.ConstructRTTexture(s_renderDevice, m_viewportSize, m_pingPongRTParams, false);
		m_pingPongRTTexture2.ConstructRTTexture(s_renderDevice, m_viewportSize, m_pingPongRTParams, false);

		// Shadow map RT texture
		m_shadowMapRTTexture.ConstructRTTexture(s_renderDevice, m_shadowMapResolution, m_shadowsRTParams, true);

		// Point light RT texture
		for (int i = 0; i < MAX_POINT_LIGHTS; i++)
			m_pLightShadowTextures[i].ConstructRTCubemapTexture(s_renderDevice, m_pLightShadowResolution, m_shadowsRTParams);

		// Initialize primary render buffer
		m_primaryBuffer.Construct(s_renderDevice, RenderBufferStorage::STORAGE_DEPTH, m_viewportSize);
		m_primaryMSAABuffer.Construct(s_renderDevice, RenderBufferStorage::STORAGE_DEPTH, m_viewportSize, 4);

		// Initialize hdri render buffer
		m_hdriCaptureRenderBuffer.Construct(s_renderDevice, RenderBufferStorage::STORAGE_DEPTH_COMP24, m_hdriResolution);

		// Initialize primary render target.
		m_primaryRenderTarget.Construct(s_renderDevice, m_primaryRTTexture0, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		m_primaryMSAATarget.Construct(s_renderDevice, m_primaryMSAARTTexture0, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_primaryMSAABuffer.GetID(), 0);

		// Bind the extre texture to primary render target, also tell open gl that we are running mrts.
		s_renderDevice.BindTextureToRenderTarget(m_primaryRenderTarget.GetID(), m_primaryRTTexture1.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
		s_renderDevice.BindTextureToRenderTarget(m_primaryMSAATarget.GetID(), m_primaryMSAARTTexture1.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D_MULTISAMPLE, FrameBufferAttachment::ATTACHMENT_COLOR, 1);
		uint32 attachments[2] = { FrameBufferAttachment::ATTACHMENT_COLOR , (FrameBufferAttachment::ATTACHMENT_COLOR + (uint32)1) };
		s_renderDevice.MultipleDrawBuffersCommand(m_primaryRenderTarget.GetID(), 2, attachments);
		s_renderDevice.MultipleDrawBuffersCommand(m_primaryMSAATarget.GetID(), 2, attachments);

		// Initialize ping pong render targets
		m_pingPongRenderTarget1.Construct(s_renderDevice, m_pingPongRTTexture1, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);
		m_pingPongRenderTarget2.Construct(s_renderDevice, m_pingPongRTTexture2, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR);

		// Initialize HDRI render target
		m_hdriCaptureRenderTarget.Construct(s_renderDevice, m_hdriResolution, FrameBufferAttachment::ATTACHMENT_DEPTH, m_hdriCaptureRenderBuffer.GetID());

		// Initialize depth map for shadows
		m_shadowMapTarget.Construct(s_renderDevice, m_shadowMapRTTexture, m_shadowMapResolution, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

		// Initialize depth map for point light shadows.
		for (int i = 0; i < MAX_POINT_LIGHTS; i++)
			m_pLightShadowTargets[i].Construct(s_renderDevice, m_pLightShadowTextures[i], m_pLightShadowResolution, TextureBindMode::BINDTEXTURE_TEXTURE, FrameBufferAttachment::ATTACHMENT_DEPTH, true);

#ifdef LINA_EDITOR
		m_secondaryRTTexture.ConstructRTTexture(s_renderDevice, m_viewportSize, m_primaryRTParams, false);
		m_secondaryRenderBuffer.Construct(s_renderDevice, RenderBufferStorage::STORAGE_DEPTH, m_viewportSize);
		m_secondaryRenderTarget.Construct(s_renderDevice, m_secondaryRTTexture, m_viewportSize, TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, FrameBufferAttachment::ATTACHMENT_DEPTH, m_secondaryRenderBuffer.GetID());
#endif

	}

	void RenderEngine::DumpMemory()
	{
		// Clear dumps.
		Model::UnloadAll();
		Texture::UnloadAll();
		Material::UnloadAll();
		Shader::UnloadAll();
	}


	void RenderEngine::Draw()
	{
		UpdateSystems();

		// Set render targets for point light shadows & calculate all the depth textures.
		auto& tuple = m_lightingSystem.GetPointLights();
		for (int i = 0; i < m_lightingSystem.GetPointLights().size(); i++)
		{
			if (std::get<1>(tuple[i])->m_castsShadows)
			{
				Vector3 lightPos = std::get<0>(tuple[i])->GetLocation();
				float farPlane = std::get<1>(tuple[i])->m_shadowFar;
				float nearPlane = std::get<1>(tuple[i])->m_shadowNear;

				std::vector<Matrix> shadowTransforms = m_lightingSystem.GetPointLightMatrices(lightPos, m_pLightShadowResolution, nearPlane, farPlane);

				// Set render target
				s_renderDevice.SetFBO(m_pLightShadowTargets[i].GetID());
				s_renderDevice.SetViewport(Vector2::Zero, m_pLightShadowResolution);

				// Clear color bit.
				s_renderDevice.Clear(false, true, false, Color::White, 0xFF);

				// Update depth shader data.
				m_pLightShadowDepthMaterial.SetVector3(UF_LIGHTPOS, lightPos);
				m_pLightShadowDepthMaterial.SetFloat(UF_LIGHTFARPLANE, farPlane);

				for (unsigned int j = 0; j < 6; j++)
					m_pLightShadowDepthMaterial.SetMatrix4(std::string(UF_SHADOWMATRICES) + "[" + std::to_string(j) + "]", shadowTransforms[j]);


				// Draw scene
				DrawSceneObjects(m_shadowMapDrawParams, &m_pLightShadowDepthMaterial, false);
			}
		}

		s_renderDevice.SetFBO(m_primaryMSAATarget.GetID());
		s_renderDevice.SetViewport(Vector2::Zero, m_viewportSize);


		if (m_customDrawEnabled && m_customDrawFunction)
			m_customDrawFunction();
		else
		{
			s_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
			DrawSkybox();
			DrawSceneObjects(m_defaultDrawParams);
		}

		// Draw debugs
		ProcessDebugQueue();

		// Finalize drawing.
		DrawFinalize();


	}

	void RenderEngine::DrawFinalize()
	{
		s_renderDevice.BlitFrameBuffers(m_primaryMSAATarget.GetID(), m_viewportSize.x, m_viewportSize.y, m_primaryRenderTarget.GetID(), m_viewportSize.x, m_viewportSize.y, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST, FrameBufferAttachment::ATTACHMENT_COLOR, (uint32)0);
		s_renderDevice.BlitFrameBuffers(m_primaryMSAATarget.GetID(), m_viewportSize.x, m_viewportSize.y, m_primaryRenderTarget.GetID(), m_viewportSize.x, m_viewportSize.y, BufferBit::BIT_COLOR, SamplerFilter::FILTER_NEAREST, FrameBufferAttachment::ATTACHMENT_COLOR, (uint32)1);

		// Below we process bloom post fx based on brightColor output in shaders.
		bool horizontal = true;
		if (m_renderSettings.m_bloomEnabled)
		{
			// Write to the pingpong buffers to apply 2 pass gaussian blur.
			bool firstIteration = true;
			unsigned int amount = 10;
			for (unsigned int i = 0; i < amount; i++)
			{
				// Select FBO
				s_renderDevice.SetFBO(horizontal ? m_pingPongRenderTarget1.GetID() : m_pingPongRenderTarget2.GetID());

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
				s_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
				horizontal = !horizontal;
				if (firstIteration) firstIteration = false;
			}
		}

		// Take the final image (without bloom), run it through a custom post processing pipeline.
		Texture* screenMap = &m_primaryRTTexture0;
		for (auto& pp : m_postProcessMap)
		{
			pp.second.Draw(screenMap);
			screenMap = &pp.second.GetTexture();
		}

		// After we've applied custom post processing, draw the final image either to the screen, or to a secondary frame buffer to display it in editor.
#ifdef LINA_EDITOR
		s_renderDevice.SetFBO(m_secondaryRenderTarget.GetID());
#else
		// Back to default buffer
		s_renderDevice.SetFBO(0);
#endif
		s_renderDevice.SetViewport(m_viewportPos, m_viewportSize);
		s_renderDevice.Clear(true, true, true, Color::White, 0xFF);

		// Set frame buffer texture on the material.
		m_screenQuadFinalMaterial.SetTexture(MAT_MAP_SCREEN, screenMap, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		if (m_screenQuadFinalMaterial.m_bools[MAT_BLOOMENABLED])
			m_screenQuadFinalMaterial.SetTexture(MAT_MAP_BLOOM, horizontal ? &m_pingPongRTTexture1 : &m_pingPongRTTexture2, TextureBindMode::BINDTEXTURE_TEXTURE2D);

		Vector2 inverseMapSize = 1.0f / m_primaryRTTexture0.GetSize();
		m_screenQuadFinalMaterial.SetVector3(MAT_INVERSESCREENMAPSIZE, Vector3(inverseMapSize.x, inverseMapSize.y, 0.0));

		UpdateShaderData(&m_screenQuadFinalMaterial);
		s_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
	}

	void RenderEngine::DrawLine(Vector3 p1, Vector3 p2, Color col, float width)
	{
		m_debugLineQueue.push(DebugLine{ p1, p2, col, width });
	}

	void RenderEngine::DrawAABB(Vector3 center, Vector3 halfWidths, Color color, float width)
	{
		Vector3 p1 = center + Vector3(-halfWidths.x, -halfWidths.y, -halfWidths.z);
		Vector3 p2 = center + Vector3(-halfWidths.x, -halfWidths.y, halfWidths.z);
		Vector3 p3 = center + Vector3(halfWidths.x, -halfWidths.y, halfWidths.z);
		Vector3 p4 = center + Vector3(halfWidths.x, -halfWidths.y, -halfWidths.z);

		Vector3 p5 = center + Vector3(-halfWidths.x, halfWidths.y, -halfWidths.z);
		Vector3 p6 = center + Vector3(-halfWidths.x, halfWidths.y, halfWidths.z);
		Vector3 p7 = center + Vector3(halfWidths.x, halfWidths.y, halfWidths.z);
		Vector3 p8 = center + Vector3(halfWidths.x, halfWidths.y, -halfWidths.z);

		// Down horizontal
		m_debugLineQueue.push(DebugLine{ p1, p2, color, width });
		m_debugLineQueue.push(DebugLine{ p2, p3, color, width });
		m_debugLineQueue.push(DebugLine{ p3, p4, color, width });
		m_debugLineQueue.push(DebugLine{ p4, p1, color, width });

		// Up horizontal
		m_debugLineQueue.push(DebugLine{ p5, p6, color, width });
		m_debugLineQueue.push(DebugLine{ p6, p7, color, width });
		m_debugLineQueue.push(DebugLine{ p7, p8, color, width });
		m_debugLineQueue.push(DebugLine{ p8, p5, color, width });

		// Vertical
		m_debugLineQueue.push(DebugLine{ p1, p5, color, width });
		m_debugLineQueue.push(DebugLine{ p2, p6, color, width });
		m_debugLineQueue.push(DebugLine{ p3, p7, color, width });
		m_debugLineQueue.push(DebugLine{ p4, p8, color, width });

	}

	void RenderEngine::ProcessDebugQueue()
	{
		while (!m_debugLineQueue.empty())
		{
			DebugLine line = m_debugLineQueue.front();
			s_renderDevice.SetShader(m_debugDrawMaterial.m_shaderID);
			s_renderDevice.UpdateShaderUniformColor(m_debugDrawMaterial.m_shaderID, MAT_COLOR, line.m_color);
			s_renderDevice.DrawLine(m_debugDrawMaterial.m_shaderID, Matrix::Identity(), line.m_from, line.m_to, line.m_width);
			m_debugLineQueue.pop();
		}

	}

	void RenderEngine::SetDrawParameters(const DrawParams& params)
	{
		s_renderDevice.SetDrawParameters(params);
	}

	void RenderEngine::UpdateRenderSettings()
	{
		m_screenQuadFinalMaterial.SetBool(MAT_FXAAENABLED, m_renderSettings.m_fxaaEnabled);
		m_screenQuadFinalMaterial.SetBool(MAT_BLOOMENABLED, m_renderSettings.m_bloomEnabled);
		m_screenQuadFinalMaterial.SetBool(MAT_VIGNETTEENABLED, m_renderSettings.m_vignetteEnabled);
		m_screenQuadFinalMaterial.SetFloat(MAT_FXAAREDUCEMIN, m_renderSettings.m_fxaaReduceMin);
		m_screenQuadFinalMaterial.SetFloat(MAT_FXAAREDUCEMUL, m_renderSettings.m_fxaaReduceMul);
		m_screenQuadFinalMaterial.SetFloat(MAT_FXAASPANMAX, m_renderSettings.m_fxaaSpanMax);
		m_screenQuadFinalMaterial.SetFloat(MAT_GAMMA, m_renderSettings.m_gamma);
		m_screenQuadFinalMaterial.SetFloat(MAT_EXPOSURE, m_renderSettings.m_exposure);
		m_screenQuadFinalMaterial.SetFloat(MAT_VIGNETTEAMOUNT, m_renderSettings.m_vignetteAmount);
		m_screenQuadFinalMaterial.SetFloat(MAT_VIGNETTEPOW, m_renderSettings.m_vignettePow);
	}


	void RenderEngine::DrawSkybox()
	{
		if (m_skyboxMaterial != nullptr)
		{
			UpdateShaderData(m_skyboxMaterial);
			s_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 4, true);
		}
		else
		{
			UpdateShaderData(&m_defaultSkyboxMaterial);
			s_renderDevice.Draw(m_skyboxVAO, m_skyboxDrawParams, 1, 4, true);
		}
	}

	PostProcessEffect& RenderEngine::AddPostProcessEffect(Shader& shader)
	{
		if (m_postProcessMap.find(&shader) == m_postProcessMap.end())
		{
			m_postProcessMap[&shader] = PostProcessEffect();
			m_postProcessMap[&shader].Construct(shader, this);
		}

		return m_postProcessMap[&shader];
	}

	void RenderEngine::DrawSceneObjects(DrawParams& drawParams, Material* overrideMaterial, bool completeFlush)
	{
		m_meshRendererSystem.FlushOpaque(drawParams, overrideMaterial, completeFlush);
		m_meshRendererSystem.FlushTransparent(drawParams, overrideMaterial, completeFlush);
		m_spriteRendererSystem.Flush(drawParams, overrideMaterial, completeFlush);

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

		m_globalDataBuffer.Update(&m_lightingSystem.GetDirectionalLightMatrix()[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		Matrix VP = m_cameraSystem.GetProjectionMatrix() * m_cameraSystem.GetViewMatrix();
		m_globalDataBuffer.Update(&VP[0][0], currentGlobalDataOffset, sizeof(Matrix));
		currentGlobalDataOffset += sizeof(Matrix);

		m_globalDataBuffer.Update(&viewPos, currentGlobalDataOffset, sizeof(Vector4));
		currentGlobalDataOffset += sizeof(Vector4);

		ECS::CameraComponent* cameraComponent = m_cameraSystem.GetActiveCameraComponent();

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
		Color color = m_lightingSystem.GetAmbientColor();
		Vector4 ambientColor = Vector4(color.r, color.g, color.b, 1.0f);
		m_globalLightBuffer.Update(&ambientColor, 0, sizeof(float) * 4);
		m_globalLightBuffer.Update(&m_currentPointLightCount, sizeof(float) * 4, sizeof(int));
		m_globalLightBuffer.Update(&m_currentSpotLightCount, sizeof(float) * 4 + sizeof(int), sizeof(int));

		// Update debug fufer.
		m_globalDebugBuffer.Update(&m_debugData.visualizeDepth, 0, sizeof(bool));
	}

	void RenderEngine::UpdateShaderData(Material* data)
	{

		s_renderDevice.SetShader(data->GetShaderID());

		for (auto const& d : (*data).m_floats)
			s_renderDevice.UpdateShaderUniformFloat(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_bools)
			s_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_colors)
			s_renderDevice.UpdateShaderUniformColor(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_ints)
			s_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_vector2s)
			s_renderDevice.UpdateShaderUniformVector2(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_vector3s)
			s_renderDevice.UpdateShaderUniformVector3(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_vector4s)
			s_renderDevice.UpdateShaderUniformVector4F(data->m_shaderID, d.first, d.second);

		for (auto const& d : (*data).m_matrices)
			s_renderDevice.UpdateShaderUniformMatrix(data->m_shaderID, d.first, d.second);

		// Set material's shadow textures to the FBO textures.
		if (data->m_receivesLighting)
		{
			auto& tuple = m_lightingSystem.GetPointLights();

			for (int i = 0; i < m_lightingSystem.GetPointLights().size(); i++)
			{
				std::string textureName = std::string(MAT_MAPS_SHADOWDEPTH) + "[" + std::to_string(i) + "]";
				if (std::get<1>(tuple[i])->m_castsShadows)
					data->SetTexture(textureName, &m_pLightShadowTextures[i], TextureBindMode::BINDTEXTURE_CUBEMAP);
				else
					data->RemoveTexture(textureName);
			}
		}

		for (auto const& d : (*data).m_sampler2Ds)
		{
			// Set whether the texture is active or not.
			bool isActive = (d.second.m_isActive && d.second.m_boundTexture != nullptr && !d.second.m_boundTexture->GetIsEmpty()) ? true : false;
			s_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first + MAT_EXTENSION_ISACTIVE, isActive);

			// Set the texture to corresponding active unit.
			s_renderDevice.UpdateShaderUniformInt(data->m_shaderID, d.first + MAT_EXTENSION_TEXTURE2D, d.second.m_unit);

			// Set texture
			if (isActive)
				s_renderDevice.SetTexture(d.second.m_boundTexture->GetID(), d.second.m_boundTexture->GetSamplerID(), d.second.m_unit, d.second.m_bindMode, true);
			else
			{

				if (d.second.m_bindMode == TextureBindMode::BINDTEXTURE_TEXTURE2D)
					s_renderDevice.SetTexture(s_defaultTexture.GetID(), s_defaultTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_TEXTURE2D);
				else
					s_renderDevice.SetTexture(m_defaultCubemapTexture.GetID(), m_defaultCubemapTexture.GetSamplerID(), d.second.m_unit, BINDTEXTURE_CUBEMAP);
			}
		}


		if (data->m_receivesLighting)
			m_lightingSystem.SetLightingShaderData(data->GetShaderID());

		if (!m_firstFrameDrawn)
		{
			s_renderDevice.ValidateShaderProgram(data->GetShaderID());
		}
	}

	void RenderEngine::CaptureCalculateHDRI(Texture& hdriTexture)
	{
		// Build projection & view matrices for capturing HDRI data.
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
		s_renderDevice.SetFBO(0);
		s_renderDevice.SetViewport(m_viewportPos, m_viewportSize);

		// Set flag
		m_hdriDataCaptured = true;

		std::set<Material*>& hdriMaterials = Material::GetHDRIMaterials();
		for (Material* mat : hdriMaterials)
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
		m_hdriCubemap.ConstructRTCubemapTexture(s_renderDevice, m_hdriResolution, samplerParams);

		// Setup shader data.
		uint32 equirectangularShader = m_hdriEquirectangularShader->GetID();
		s_renderDevice.SetShader(equirectangularShader);
		s_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		s_renderDevice.UpdateShaderUniformInt(equirectangularShader, MAT_MAP_EQUIRECTANGULAR + std::string(MAT_EXTENSION_ISACTIVE), 1);
		s_renderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_PROJECTION, captureProjection);
		s_renderDevice.SetTexture(hdriTexture.GetID(), hdriTexture.GetSamplerID(), 0);
		s_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
		s_renderDevice.SetViewport(Vector2::Zero, m_hdriResolution);

		// Draw the cubemap.
		for (uint32 i = 0; i < 6; ++i)
		{
			s_renderDevice.UpdateShaderUniformMatrix(equirectangularShader, UF_MATRIX_VIEW, views[i]);
			s_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false);
			s_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
			s_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
			s_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
		}

		// Generate mipmaps & check errors.
		s_renderDevice.GenerateTextureMipmaps(m_hdriCubemap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP);
		s_renderDevice.IsRenderTargetComplete(m_hdriCaptureRenderTarget.GetID());
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

		// Build irradiance texture & scale render buffer according to the resolution.
		m_hdriIrradianceMap.ConstructRTCubemapTexture(s_renderDevice, irradianceMapResolsution, irradianceParams);
		s_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
		s_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), irradianceMapResolsution, RenderBufferStorage::STORAGE_DEPTH_COMP24);

		// Build & setup shader info.
		uint32 irradianceShader = m_hdriIrradianceShader->GetID();
		s_renderDevice.SetShader(irradianceShader);
		s_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		s_renderDevice.UpdateShaderUniformInt(irradianceShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
		s_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_PROJECTION, captureProjection);
		s_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);
		s_renderDevice.SetViewport(Vector2::Zero, irradianceMapResolsution);

		// Draw cubemap.
		for (uint32 i = 0; i < 6; ++i)
		{
			s_renderDevice.UpdateShaderUniformMatrix(irradianceShader, UF_MATRIX_VIEW, views[i]);
			s_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriIrradianceMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, 0, false, false);
			s_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
			s_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
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
		m_hdriPrefilterMap.ConstructRTCubemapTexture(s_renderDevice, prefilterResolution, prefilterParams);

		// Setup shader data.
		uint32 prefilterShader = m_hdriPrefilterShader->GetID();
		s_renderDevice.SetShader(prefilterShader);
		s_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_TEXTURE2D), 0);
		s_renderDevice.UpdateShaderUniformInt(prefilterShader, MAT_MAP_ENVIRONMENT + std::string(MAT_EXTENSION_ISACTIVE), 1);
		s_renderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ENVIRONMENTRESOLUTION, 512.0f);
		s_renderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_PROJECTION, captureProjection);
		s_renderDevice.SetTexture(m_hdriCubemap.GetID(), m_hdriCubemap.GetSamplerID(), 0, TextureBindMode::BINDTEXTURE_CUBEMAP);

		// Setup mip levels & switch fbo.
		uint32 maxMipLevels = 5;
		s_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());

		for (uint32 mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			s_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), Vector2(mipWidth, mipHeight), RenderBufferStorage::STORAGE_DEPTH_COMP24);
			s_renderDevice.SetViewport(Vector2::Zero, Vector2(mipWidth, mipHeight));

			// Draw prefiltered map
			float roughness = (float)mip / (float)(maxMipLevels - 1);
			s_renderDevice.UpdateShaderUniformFloat(prefilterShader, MAT_ROUGHNESSMULTIPLIER, roughness);
			for (unsigned int i = 0; i < 6; ++i)
			{
				s_renderDevice.UpdateShaderUniformMatrix(prefilterShader, UF_MATRIX_VIEW, views[i]);
				s_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_hdriPrefilterMap.GetID(), TextureBindMode::BINDTEXTURE_CUBEMAP_POSITIVE_X, FrameBufferAttachment::ATTACHMENT_COLOR, 0, i, mip, false, false);
				s_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
				s_renderDevice.Draw(m_hdriCubeVAO, m_defaultDrawParams, 0, 36, true);
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

		// Build BRDF texture.
		m_HDRILutMap.ConstructHDRI(s_renderDevice, samplerParams, brdfLutSize, NULL);

		// Scale render buffer according to the resolution & bind lut map to frame buffer.
		s_renderDevice.ResizeRenderBuffer(m_hdriCaptureRenderTarget.GetID(), m_hdriCaptureRenderBuffer.GetID(), brdfLutSize, RenderBufferStorage::STORAGE_DEPTH_COMP24);
		s_renderDevice.BindTextureToRenderTarget(m_hdriCaptureRenderTarget.GetID(), m_HDRILutMap.GetID(), TextureBindMode::BINDTEXTURE_TEXTURE2D, FrameBufferAttachment::ATTACHMENT_COLOR, 0, 0, 0, true, false);

		// Setup shader.
		uint32 brdfShader = m_hdriBRDFShader->GetID();
		s_renderDevice.SetShader(brdfShader);

		// Switch framebuffer & draw.
		s_renderDevice.SetFBO(m_hdriCaptureRenderTarget.GetID());
		s_renderDevice.SetViewport(Vector2::Zero, brdfLutSize);
		s_renderDevice.Clear(true, true, true, m_cameraSystem.GetCurrentClearColor(), 0xFF);
		s_renderDevice.Draw(m_screenQuadVAO, m_fullscreenQuadDP, 0, 6, true);
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
		mat->RemoveTexture(MAT_TEXTURE2D_BRDFLUTMAP);
		mat->RemoveTexture(MAT_TEXTURE2D_PREFILTERMAP);

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
#ifdef LINA_EDITOR
		return (void*)m_secondaryRTTexture.GetID();
#else
		return (void*)m_primaryRTTexture0.GetID();
#endif
	}

	void* RenderEngine::GetShadowMapImage()
	{
		return (void*)m_shadowMapRTTexture.GetID();
	}

	void RenderEngine::UpdateSystems()
	{
		// Update pipeline.
		m_renderingPipeline.UpdateSystems(0.0f);

		// Update uniform buffers on GPU
		UpdateUniformBuffers();

	}

	void RenderEngine::BindShaderToViewBuffer(Shader& shader)
	{
		shader.BindBlockToBuffer(UNIFORMBUFFER_VIEWDATA_BINDPOINT, UNIFORMBUFFER_VIEWDATA_NAME);
	}

	void RenderEngine::BindShaderToDebugBuffer(Shader& shader)
	{
		shader.BindBlockToBuffer(UNIFORMBUFFER_DEBUGDATA_BINDPOINT, UNIFORMBUFFER_DEBUGDATA_NAME);
	}

	void RenderEngine::BindShaderToLightBuffer(Shader& shader)
	{
		shader.BindBlockToBuffer(UNIFORMBUFFER_LIGHTDATA_BINDPOINT, UNIFORMBUFFER_LIGHTDATA_NAME);
	}

}