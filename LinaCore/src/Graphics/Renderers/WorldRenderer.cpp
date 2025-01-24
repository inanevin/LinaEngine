/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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

#include "Core/Graphics/Renderers/WorldRenderer.hpp"
#include "Core/Graphics/Renderers/WorldRendererListener.hpp"
#include "Core/Graphics/Utility/GfxHelpers.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Data/ModelNode.hpp"
#include "Core/Graphics/Data/Mesh.hpp"
#include "Core/Graphics/Pipeline/View.hpp"
#include "Core/GUI/Widgets/Widget.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/World/Components/CompModel.hpp"
#include "Core/World/Components/CompLight.hpp"
#include "Core/World/Components/CompWidget.hpp"
#include "Core/Resources/ResourceManager.hpp"
#include "Core/Application.hpp"
#include "Core/Graphics/Renderers/DrawCollector.hpp"
#include "Core/Graphics/Renderers/PhysicsDebugRenderer.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Math/Math.hpp"
#include "Common/System/SystemInfo.hpp"
#include "Common/Profiling/Profiler.hpp"
#include "Core/Physics/PhysicsWorld.hpp"

#include <Jolt/Jolt.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <LinaGX/Core/InputMappings.hpp>

namespace Lina
{
#define MAX_GFX_COMMANDS  250
#define MAX_COPY_COMMANDS 200
#define MAX_GUI_VERTICES  5000
#define MAX_GUI_INDICES	  5000
#define MAX_GUI_MATERIALS 100
#define MAX_OBJECTS		  256

	WorldRenderer::WorldRenderer(GfxContext* context, ResourceManagerV2* rm, EntityWorld* world, const Vector2ui& viewSize, const String& name, Buffer* snapshotBuffers, bool standaloneSubmit)
	{
		m_name										= name.empty() ? "WorldRenderer" : name;
		m_gfxContext								= context;
		m_standaloneSubmit							= standaloneSubmit;
		m_snapshotBuffer							= snapshotBuffers;
		m_world										= world;
		m_size										= viewSize;
		m_resourceManagerV2							= rm;
		m_gBufSampler								= m_resourceManagerV2->CreateResource<TextureSampler>(m_resourceManagerV2->ConsumeResourceID(), name + " Sampler");
		m_shaderLightingQuad						= m_resourceManagerV2->GetResource<Shader>(ENGINE_SHADER_LIGHTING_QUAD_ID);
		m_shaderDebugLine							= m_resourceManagerV2->GetResource<Shader>(ENGINE_SHADER_WORLD_DEBUG_LINE_ID);
		m_samplerGUIText							= m_resourceManagerV2->GetResource<TextureSampler>(ENGINE_SAMPLER_GUI_TEXT_ID);
		m_world->GetLVGDrawer().GetCallbacks().draw = BIND(&WorldRenderer::OnWorldLVGDraw, this, std::placeholders::_1);

		LinaGX::SamplerDesc gBufSampler = {
			.anisotropy = 1,
		};
		m_gBufSampler->GenerateHW(gBufSampler);
		m_lgx = Application::GetLGX();

		m_deferredPass.Create(GfxHelpers::GetRenderPassDescription(RenderPassType::RENDER_PASS_DEFERRED));
		m_forwardPass.Create(GfxHelpers::GetRenderPassDescription(RenderPassType::RENDER_PASS_FORWARD));
		m_forwardPass.SetSize(m_size);
		m_deferredPass.SetSize(m_size);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			const String cmdStreamName	= name + " Gfx Stream " + TO_STRING(i);
			const String cmdStreamName2 = name + " Copy Stream " + TO_STRING(i);
			data.gfxStream				= m_lgx->CreateCommandStream({LinaGX::CommandType::Graphics, MAX_GFX_COMMANDS, 24000, 4096, 32, cmdStreamName.c_str()});
			data.copyStream				= m_lgx->CreateCommandStream({LinaGX::CommandType::Transfer, MAX_COPY_COMMANDS, 4000, 4096, 32, cmdStreamName2.c_str()});
			data.signalSemaphore		= SemaphoreData(m_lgx->CreateUserSemaphore());
			data.copySemaphore			= SemaphoreData(m_lgx->CreateUserSemaphore());

			data.argumentsBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPUDrawArguments) * 1000, m_name + " InstanceDataBuffer");
			data.entityBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPUEntity) * 1000, m_name + " EntityBuffer");
			data.boneBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(Matrix4) * 1000, m_name + " BoneBuffer");
			data.lightBuffer.Create(LinaGX::ResourceTypeHint::TH_StorageBuffer, sizeof(GPULight) * 100, m_name + " LightBuffer");

			data.lvgVtxBuffer.Create(LinaGX::ResourceTypeHint::TH_VertexBuffer, sizeof(LinaVG::Vertex) * 2000, m_name + " LVGVertexBuffer");
			data.lvgIdxBuffer.Create(LinaGX::ResourceTypeHint::TH_IndexBuffer, sizeof(LinaVG::Index) * 5000, m_name + "LVGIndexBuffer");

			const uint16 setDf = m_deferredPass.GetDescriptorSet(i);
			const uint16 setFw = m_forwardPass.GetDescriptorSet(i);

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setDf,
				.binding   = 1,
				.buffers   = {data.argumentsBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setDf,
				.binding   = 2,
				.buffers   = {data.entityBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setDf,
				.binding   = 3,
				.buffers   = {data.boneBuffer.GetGPUResource()},
			});

			// FW
			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setFw,
				.binding   = 1,
				.buffers   = {data.argumentsBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setFw,
				.binding   = 2,
				.buffers   = {data.entityBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setFw,
				.binding   = 3,
				.buffers   = {data.boneBuffer.GetGPUResource()},
			});

			m_lgx->DescriptorUpdateBuffer({
				.setHandle = setFw,
				.binding   = 4,
				.buffers   = {data.lightBuffer.GetGPUResource()},
			});
		}

		CreateSizeRelativeResources();
		m_shapeRenderer.Initialize();
	}

	WorldRenderer::~WorldRenderer()
	{
#ifdef JPH_DEBUG_RENDERER
		if (m_physicsDebugRenderer)
			delete m_physicsDebugRenderer;
#endif
		m_physicsDebugRenderer = nullptr;
		m_shapeRenderer.Shutdown();
		m_gBufSampler->DestroyHW();
		m_resourceManagerV2->DestroyResource(m_gBufSampler);

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			m_lgx->DestroyCommandStream(data.gfxStream);
			m_lgx->DestroyCommandStream(data.copyStream);

			m_lgx->DestroyUserSemaphore(data.signalSemaphore.GetSemaphore());
			m_lgx->DestroyUserSemaphore(data.copySemaphore.GetSemaphore());

			data.argumentsBuffer.Destroy();
			data.entityBuffer.Destroy();
			data.boneBuffer.Destroy();
			data.lightBuffer.Destroy();

			data.lvgVtxBuffer.Destroy();
			data.lvgIdxBuffer.Destroy();
		}

		m_deferredPass.Destroy();
		m_forwardPass.Destroy();

		DestroySizeRelativeResources();
	}

	void WorldRenderer::CalculateSkinning(const Vector<CompModel*>& comps)
	{
		Vector<uint32> boneIDs;
		uint32		   boneIdxCounter = 0;
		for (CompModel* comp : comps)
		{
			const Vector<ModelSkin>& skins = comp->GetModelPtr()->GetAllSkins();
			boneIDs.push_back(boneIdxCounter);
			for (const ModelSkin& skin : skins)
				boneIdxCounter += static_cast<uint32>(skin.jointIndices.size());
		}

		m_cpuDrawData.bones.resize(boneIdxCounter);

		Taskflow tf;
		tf.for_each_index(0, static_cast<int>(comps.size()), 1, [&](int i) {
			CompModel*				 comp  = comps.at(i);
			const Vector<ModelSkin>& skins = comp->GetModelPtr()->GetAllSkins();
			Vector<CompModelNode>&	 nodes = comp->GetNodes();

			uint32 boneID = boneIDs.at(i);

			for (const ModelSkin& skin : skins)
			{
				const Matrix4 rootGlobal		= skin.rootJointIndex != -1 ? comp->CalculateGlobalMatrix(skin.rootJointIndex) : comp->GetEntity()->GetTransform().ToMatrix();
				const Matrix4 rootGlobalInverse = rootGlobal.Inverse();

				uint32 idx = 0;
				for (uint32 jointIndex : skin.jointIndices)
				{
					const Matrix4 global			  = comp->CalculateGlobalMatrix(jointIndex);
					m_cpuDrawData.bones[boneID + idx] = rootGlobalInverse * global * nodes.at(jointIndex).inverseBindTransform;
					idx++;
				}
			}
		});
		m_executor.RunAndWait(tf);
	}

	void WorldRenderer::CreateSizeRelativeResources()
	{
		LinaGX::TextureDesc rtDesc = {
			.format = SystemInfo::GetHDRFormat(),
			.flags	= LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	= m_size.x,
			.height = m_size.y,
		};

		LinaGX::TextureDesc rtDescLighting = {
			.format = SystemInfo::GetHDRFormat(),
			.flags	= LinaGX::TF_ColorAttachment | LinaGX::TF_Sampled,
			.width	= m_size.x,
			.height = m_size.y,
		};

		LinaGX::TextureDesc depthDesc = {
			.format					  = SystemInfo::GetDepthFormat(),
			.depthStencilSampleFormat = LinaGX::Format::R32_SFLOAT,
			.flags					  = LinaGX::TF_DepthTexture | LinaGX::TF_Sampled,
			.width					  = m_size.x,
			.height					  = m_size.y,
		};

		if (m_snapshotBuffer != nullptr)
			rtDescLighting.flags |= LinaGX::TF_CopySource;

		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];

			data.gBufAlbedo			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufAlbedo " + TO_STRING(i));
			data.gBufPosition		= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufPosition " + TO_STRING(i));
			data.gBufNormal			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufNormal " + TO_STRING(i));
			data.gBufDepth			= m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufDepth " + TO_STRING(i));
			data.lightingPassOutput = m_resourceManagerV2->CreateResource<Texture>(m_resourceManagerV2->ConsumeResourceID(), m_name + " GBufLightingPass " + TO_STRING(i));

			rtDesc.debugName = data.gBufAlbedo->GetName().c_str();
			data.gBufAlbedo->GenerateHWFromDesc(rtDesc);

			rtDesc.debugName = data.gBufPosition->GetName().c_str();
			data.gBufPosition->GenerateHWFromDesc(rtDesc);

			rtDesc.debugName = data.gBufNormal->GetName().c_str();
			data.gBufNormal->GenerateHWFromDesc(rtDesc);

			depthDesc.debugName = data.gBufDepth->GetName().c_str();
			data.gBufDepth->GenerateHWFromDesc(depthDesc);

			rtDescLighting.debugName = data.lightingPassOutput->GetName().c_str();
			data.lightingPassOutput->GenerateHWFromDesc(rtDescLighting);

			m_deferredPass.SetColorAttachment(i, 0, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufAlbedo->GetGPUHandle(), .isSwapchain = false});
			m_deferredPass.SetColorAttachment(i, 1, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufPosition->GetGPUHandle(), .isSwapchain = false});
			m_deferredPass.SetColorAttachment(i, 2, {.clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.gBufNormal->GetGPUHandle(), .isSwapchain = false});
			m_deferredPass.DepthStencilAttachment(i, {.useDepth = true, .texture = data.gBufDepth->GetGPUHandle(), .depthLoadOp = LinaGX::LoadOp::Clear, .depthStoreOp = LinaGX::StoreOp::Store, .clearDepth = 1.0f});

			m_forwardPass.SetColorAttachment(i, 0, {.loadOp = LinaGX::LoadOp::Load, .storeOp = LinaGX::StoreOp::Store, .clearColor = {0.0f, 0.0f, 0.0f, 1.0f}, .texture = data.lightingPassOutput->GetGPUHandle(), .isSwapchain = false});
			m_forwardPass.DepthStencilAttachment(i,
												 {
													 .useDepth	   = true,
													 .texture	   = data.gBufDepth->GetGPUHandle(),
													 .depthLoadOp  = LinaGX::LoadOp::Load,
													 .depthStoreOp = LinaGX::StoreOp::Store,
													 .clearDepth   = 1.0f,
												 });
		}

		for (WorldRendererListener* l : m_listeners)
			l->OnWorldRendererCreateSizeRelative();

		m_gfxContext->MarkBindlessDirty();
	}

	void WorldRenderer::DestroySizeRelativeResources()
	{
		for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			auto& data = m_pfd[i];
			data.gBufAlbedo->DestroyHW();
			data.gBufPosition->DestroyHW();
			data.gBufNormal->DestroyHW();
			data.gBufDepth->DestroyHW();
			data.lightingPassOutput->DestroyHW();

			m_resourceManagerV2->DestroyResource(data.gBufAlbedo);
			m_resourceManagerV2->DestroyResource(data.gBufPosition);
			m_resourceManagerV2->DestroyResource(data.gBufNormal);
			m_resourceManagerV2->DestroyResource(data.gBufDepth);
			m_resourceManagerV2->DestroyResource(data.lightingPassOutput);
		}

		for (WorldRendererListener* l : m_listeners)
			l->OnWorldRendererDestroySizeRelative();
		m_gfxContext->MarkBindlessDirty();
	}

	void WorldRenderer::AddListener(WorldRendererListener* listener)
	{
		m_listeners.push_back(listener);
	}

	void WorldRenderer::RemoveListener(WorldRendererListener* listener)
	{
		auto it = linatl::find_if(m_listeners.begin(), m_listeners.end(), [listener](WorldRendererListener* l) -> bool { return l == listener; });
		m_listeners.erase(it);
	}

	uint32 WorldRenderer::GetBoneIndex(CompModel* comp)
	{
		uint32 ctr = 0;

		for (CompModel* c : m_skinnedModels)
		{
			if (comp == c)
				return ctr;

			const Vector<ModelSkin>& skins = c->GetModelPtr()->GetAllSkins();
			for (const ModelSkin& skin : skins)
				ctr += static_cast<uint32>(skin.jointIndices.size());
		}

		return 0;
	}

	uint32 WorldRenderer::GetArgumentCount()
	{
		return static_cast<uint32>(m_cpuDrawData.arguments.size());
	}

	uint32 WorldRenderer::PushEntity(const GPUEntity& e, const EntityIdent& ident)
	{
		uint32 idx = 0;
		if (!DrawEntityExists(idx, ident))
		{
			idx = static_cast<uint32>(m_cpuDrawData.entities.size());
			m_cpuDrawData.entities.push_back({
				.entity = e,
				.ident	= ident,
			});
		}
		else
		{
			m_cpuDrawData.entities.at(idx).entity = e;
		}

		return idx;
	}

	uint32 WorldRenderer::PushArgument(const GPUDrawArguments& args)
	{
		const uint32 idx = static_cast<uint32>(m_cpuDrawData.arguments.size());
		m_cpuDrawData.arguments.push_back(args);
		return idx;
	}

	void WorldRenderer::AddDebugLine(const Vector3& p1, const Vector3& p2, float thickness, const ColorGrad& color)
	{
		m_debugLines.push_back({
			.p1		   = p1,
			.p2		   = p2,
			.thickness = thickness,
			.color	   = color,
		});
	}

	void WorldRenderer::OnWorldLVGDraw(LinaVG::DrawBuffer* buf)
	{
		const uint32	 vtxCount = static_cast<uint32>(buf->vertexBuffer.m_size);
		const uint32	 idxCount = static_cast<uint32>(buf->indexBuffer.m_size);
		const ResourceID id		  = buf->uid;
		Material*		 mat	  = m_resourceManagerV2->GetResource<Material>(id);
		Shader*			 shader	  = m_resourceManagerV2->GetResource<Shader>(mat->GetShader());

		if (buf->shapeType == LinaVG::DrawBufferShapeType::Text || buf->shapeType == LinaVG::DrawBufferShapeType::SDFText)
		{
			GUIBackend* guiBackend = m_gfxContext->GetGUIBackend();

			if (mat->HasProperty<LinaTexture2D>("diffuse"_hs))
			{
				LinaTexture2D* txt = mat->GetProperty<LinaTexture2D>("diffuse"_hs);

				LinaVG::Atlas* atlas   = static_cast<LinaVG::Atlas*>(buf->textureHandle);
				Texture*	   texture = guiBackend->GetFontTexture(atlas).texture;
				if (txt->texture != texture->GetID())
				{
					mat->SetProperty<LinaTexture2D>("diffuse"_hs,
													{
														.texture = texture->GetID(),
														.sampler = m_samplerGUIText->GetID(),
													});
					m_gfxContext->MarkBindlessDirty();
				}
			}
		}

		const GPUDrawArguments args = {
			.constant0 = mat->GetBindlessIndex(),
			.constant1 = PushEntity(
				{
					.model = m_currentLVGDrawEntity->GetTransform().GetMatrix(),
				},
				{
					.entityGUID = m_currentLVGDrawEntity->GetGUID(),
				}),
			.constant2 = m_currentLVGDraw3D ? (uint32)1 : 0,
		};

		RenderPass::InstancedDraw drawCall = {
			.vertexBuffers =
				{
					&m_pfd[0].lvgVtxBuffer,
					&m_pfd[1].lvgVtxBuffer,
				},
			.indexBuffers =
				{
					&m_pfd[0].lvgIdxBuffer,
					&m_pfd[1].lvgIdxBuffer,
				},
			.vertexSize	   = sizeof(LinaVG::Vertex),
			.shaderHandle  = m_currentLVGDraw3D ? shader->GetGPUHandle() : shader->GetGPUHandle(DepthTesting::None),
			.baseVertex	   = static_cast<uint32>(m_cpuDrawData.lvgVertices.size()),
			.vertexCount   = vtxCount,
			.baseIndex	   = static_cast<uint32>(m_cpuDrawData.lvgIndices.size()),
			.indexCount	   = idxCount,
			.instanceCount = 1,
			.baseInstance  = 0,
			.pushConstant  = PushArgument(args),
			.clip		   = Recti(buf->clip.x, buf->clip.y, buf->clip.z, buf->clip.w),
			.useScissors   = true,
			.sortOrder	   = m_currentLVGDraw3D,
			.sortPosition  = m_currentLVGDrawEntity->GetPosition(),
		};

		m_forwardPass.AddDrawCall(drawCall);
		m_cpuDrawData.lvgVertices.insert(m_cpuDrawData.lvgVertices.end(), buf->vertexBuffer.m_data, buf->vertexBuffer.m_data + vtxCount);
		m_cpuDrawData.lvgIndices.insert(m_cpuDrawData.lvgIndices.end(), buf->indexBuffer.m_data, buf->indexBuffer.m_data + idxCount);
	} // namespace Lina

	void WorldRenderer::Resize(const Vector2ui& newSize)
	{
		if (m_size.Equals(newSize))
			return;

		m_size = newSize;
		m_forwardPass.SetSize(m_size);
		m_deferredPass.SetSize(m_size);
		DestroySizeRelativeResources();
		CreateSizeRelativeResources();
	}

	void WorldRenderer::Tick(float delta)
	{
		// View production.
		{
			const Camera& camera = m_world->GetWorldCamera();
			View		  cameraView;
			m_forwardPass.GetView().SetView(camera.GetView());
			m_forwardPass.GetView().SetPosition(camera.GetPosition());
			m_deferredPass.GetView().SetView(camera.GetView());
			m_deferredPass.GetView().SetPosition(camera.GetPosition());
			m_cpuDrawData.worldCamera = camera;
		}

		m_compModels.resize(0);
		m_skinnedModels.resize(0);

		m_world->GetCache<CompModel>()->GetBucket().View([&](CompModel* model, uint32 idx) -> bool {
			if (!model->GetEntity()->GetVisible())
				return false;

			// Maybe some view culling, e.g. skip if not visible in any of the produces views.
			m_compModels.push_back(model);

			if (!model->GetModelPtr()->GetAllSkins().empty())
				m_skinnedModels.push_back(model);

			return false;
		});

		m_compLights.resize(0);

		m_world->GetCache<CompLight>()->GetBucket().View([&](CompLight* light, uint32 idx) -> bool {
			if (!light->GetEntity()->GetVisible())
				return false;

			m_compLights.push_back(light);
			return false;
		});

		const size_t lightsSz = m_compLights.size();
		m_cpuDrawData.lights.resize(lightsSz);

		for (size_t i = 0; i < lightsSz; i++)
		{
			GPULight&  light	 = m_cpuDrawData.lights[i];
			CompLight* compLight = m_compLights.at(i);
			Entity*	   e		 = compLight->GetEntity();

			const Color&	color		= compLight->GetColor();
			const float		intensity	= compLight->GetIntensity();
			const LightType type		= compLight->GetType();
			const float		cutoff		= compLight->GetCutoff();
			const float		outerCutoff = compLight->GetOuterCutoff();
			const float		radius		= compLight->GetRadius();
			const float		falloff		= compLight->GetFalloff();
			const GPUEntity entity		= {
					 .model	   = compLight->GetEntity()->GetTransform().ToMatrix(),
					 .position = e->GetPosition(),
					 .forward  = e->GetRotation().GetForward(),
			 };

			const uint32 idx = PushEntity(entity,
										  {
											  .entityGUID = compLight->GetEntity()->GetGUID(),
										  });

			light.colorAndIntensity = Vector4(color.x, color.y, color.z, intensity);
			light.type				= static_cast<uint32>(type);
			light.entityIndex		= idx;

			if (type == LightType::Directional)
			{
			}
			else if (type == LightType::Point)
			{
				light.radius  = radius;
				light.falloff = falloff;
			}
			else if (type == LightType::Spot)
			{
				light.radius	  = radius;
				light.falloff	  = falloff;
				light.cutoff	  = Math::Cos(Math::ToRadians(cutoff));
				light.outerCutoff = Math::Cos(Math::ToRadians(outerCutoff));
			}
		}

		CalculateSkinning(m_skinnedModels);
		DrawCollector::CollectCompModels(m_compModels, m_deferredPass, m_resourceManagerV2, this, m_gfxContext, {.allowedShaderTypes = {ShaderType::DeferredSurface}});

		// Lighting Quad
		{
			const RenderPass::InstancedDraw lightingQuad = {
				.shaderHandle  = m_shaderLightingQuad->GetGPUHandle(),
				.vertexCount   = 3,
				.instanceCount = 1,
			};
			m_forwardPass.AddDrawCall(lightingQuad);
		}

		// Skybox.
		{
			Material*			   skyMaterial = m_resourceManagerV2->GetResource<Material>(m_world->GetGfxSettings().skyMaterial);
			Shader*				   skyShader   = m_resourceManagerV2->GetResource<Shader>(skyMaterial->GetShader());
			Model*				   skyModel	   = m_resourceManagerV2->GetIfExists<Model>(m_world->GetGfxSettings().skyModel);
			const PrimitiveStatic& prim		   = skyModel->GetAllMeshes().at(0).primitivesStatic.at(0);

			if (skyShader->GetShaderType() == ShaderType::Sky)
			{
				const GPUDrawArguments args = {
					.constant1 = skyMaterial->GetBindlessIndex(),
				};

				const uint32 argIndex = PushArgument(args);

				const RenderPass::InstancedDraw skyDraw = {
					.vertexBuffers =
						{
							&m_gfxContext->GetMeshManagerDefault().GetVtxBufferStatic(),
							&m_gfxContext->GetMeshManagerDefault().GetVtxBufferStatic(),
						},
					.indexBuffers =
						{
							&m_gfxContext->GetMeshManagerDefault().GetIdxBufferStatic(),
							&m_gfxContext->GetMeshManagerDefault().GetIdxBufferStatic(),
						},
					.vertexSize	   = sizeof(VertexStatic),
					.shaderHandle  = skyShader->GetGPUHandle(),
					.baseVertex	   = prim._vertexOffset,
					.vertexCount   = static_cast<uint32>(prim.vertices.size()),
					.baseIndex	   = prim._indexOffset,
					.indexCount	   = static_cast<uint32>(prim.indices.size()),
					.instanceCount = 1,
					.baseInstance  = 0,
					.pushConstant  = argIndex,
				};

				m_forwardPass.AddDrawCall(skyDraw);
			}
		}

#ifdef JPH_DEBUG_RENDERER
		if (m_physicsDebugRenderer && m_drawPhysics)
		{
			JPH::BodyManager::DrawSettings ds = {};
			ds.mDrawShape					  = true;
			ds.mDrawVelocity				  = true;
			m_physicsDebugRenderer->BeginDraws();
			m_world->GetPhysicsWorld()->GetPhysicsSystem().DrawBodies(ds, m_physicsDebugRenderer);
			m_physicsDebugRenderer->SubmitDraws();
		}
#endif

		DrawCollector::CollectCompModels(m_compModels, m_forwardPass, m_resourceManagerV2, this, m_gfxContext, {.distanceSort = true, .allowedShaderTypes = {ShaderType::ForwardSurface}});

		m_compWidgets.resize(0);
		m_world->GetCache<CompWidget>()->GetBucket().View([&](CompWidget* w, uint32 idx) -> bool {
			if (!w->GetEntity()->GetVisible())
				return false;

			m_compWidgets.push_back(w);
			return false;
		});

		LinaVG::Drawer& drawer = m_world->GetLVGDrawer();

		for (CompWidget* w : m_compWidgets)
		{
			GUIWidget* guiWidget = w->GetWidgetPtr();
			if (!guiWidget)
				continue;

			m_currentLVGDrawEntity = w->GetEntity();
			m_currentLVGDraw3D	   = w->GetIs3D();
			const Vector2ui sz3D   = w->GetSize3D();

			WidgetManager& manager = w->GetWidgetManager();
			manager.PreTick();
			manager.Tick(delta, m_currentLVGDraw3D ? sz3D : m_size);
			manager.Draw();
			drawer.FlushBuffers();
			drawer.ResetFrame();
		}

		m_shapeRenderer.StartBatch();

		for (const LineData& l : m_debugLines)
			m_shapeRenderer.DrawLine3D(l.p1, l.p2, l.thickness, l.color);

		m_shapeRenderer.SubmitBatch(m_forwardPass, 0, m_shaderDebugLine->GetGPUHandle());
	}

	void WorldRenderer::SyncRender()
	{
		m_gpuDrawData = m_cpuDrawData;
		m_cpuDrawData.arguments.resize(0);
		m_cpuDrawData.bones.resize(0);
		m_cpuDrawData.entities.resize(0);
		m_cpuDrawData.lvgIndices.resize(0);
		m_cpuDrawData.lvgVertices.resize(0);
		m_cpuDrawData.lights.resize(0);
		m_cpuDrawData.worldCamera = {};
		m_debugLines.resize(0);

		m_shapeRenderer.SyncRender();
		m_deferredPass.SyncRender();
		m_forwardPass.SyncRender();

#ifdef JPH_DEBUG_RENDERER
		if (m_physicsDebugRenderer)
			m_physicsDebugRenderer->SyncRender();

#endif
	}

	void WorldRenderer::CreatePhysicsDebugRenderer()
	{
		if (m_physicsDebugRenderer)
			return;

#ifdef JPH_DEBUG_RENDERER
		m_physicsDebugRenderer = new PhysicsDebugRenderer(&m_forwardPass, m_shaderDebugLine, m_resourceManagerV2->GetResource<Shader>(ENGINE_SHADER_WORLD_DEBUG_TRIANGLE_ID));
#endif
	}

	void WorldRenderer::UpdateBuffers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.ResetModified();
		currentFrame.signalSemaphore.ResetModified();

		// Pass data.
		{
			Camera& worldCam = m_gpuDrawData.worldCamera;
			// const Vector2ui sz		 = m_world->GetScreen().GetRenderSize();
			// 	worldCam.Calculate(m_size);

			const GPUDataDeferredPass deferredPassData = {
				.view	  = worldCam.GetView(),
				.proj	  = worldCam.GetProjection(),
				.viewProj = worldCam.GetViewProj(),
			};
			m_deferredPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&deferredPassData, sizeof(GPUDataDeferredPass));

			WorldGfxSettings& gfx = m_world->GetGfxSettings();

			const GPUDataForwardPass fw = {
				.view				   = worldCam.GetView(),
				.proj				   = worldCam.GetProjection(),
				.viewProj			   = worldCam.GetViewProj(),
				.orthoProj			   = GfxHelpers::GetProjectionFromSize(m_size),
				.ambientTop			   = Color(gfx.ambientTop.x, gfx.ambientTop.y, gfx.ambientTop.z, gfx.ambientIntensity),
				.ambientMid			   = gfx.ambientMid,
				.ambientBot			   = gfx.ambientBot,
				.cameraPositionAndNear = Vector4(worldCam.GetPosition(), worldCam.GetZNear()),
				.cameraDirectionAndFar = Vector4(worldCam.GetRotation().GetForward(), worldCam.GetZFar()),
				.sizeAndMouse		   = Vector4(m_size.x, m_size.y, m_world->GetInput().GetMousePosition().x, m_world->GetInput().GetMousePosition().y),
				.gBufAlbedo			   = currentFrame.gBufAlbedo->GetBindlessIndex(),
				.gBufPositionMetallic  = currentFrame.gBufPosition->GetBindlessIndex(),
				.gBufNormalRoughness   = currentFrame.gBufNormal->GetBindlessIndex(),
				.gBufSampler		   = m_gBufSampler->GetBindlessIndex(),
				.lightCount			   = static_cast<uint32>(m_gpuDrawData.lights.size()),
			};

			const size_t sz = sizeof(GPUDataForwardPass);
			m_forwardPass.GetBuffer(frameIndex, "PassData"_hs).BufferData(0, (uint8*)&fw, sz);
		}

		size_t entityIdx = 0;

		for (const DrawEntity& de : m_gpuDrawData.entities)
		{
			currentFrame.entityBuffer.BufferData(entityIdx * sizeof(GPUEntity), (uint8*)&de.entity, sizeof(GPUEntity));
			entityIdx++;
		}

		currentFrame.argumentsBuffer.BufferData(0, (uint8*)m_gpuDrawData.arguments.data(), sizeof(GPUDrawArguments) * m_gpuDrawData.arguments.size());
		currentFrame.boneBuffer.BufferData(0, (uint8*)m_gpuDrawData.bones.data(), sizeof(Matrix4) * m_gpuDrawData.bones.size());
		currentFrame.lightBuffer.BufferData(0, (uint8*)m_gpuDrawData.lights.data(), sizeof(GPULight) * m_gpuDrawData.lights.size());

		currentFrame.lvgVtxBuffer.BufferData(0, (uint8*)m_gpuDrawData.lvgVertices.data(), sizeof(LinaVG::Vertex) * m_gpuDrawData.lvgVertices.size());
		currentFrame.lvgIdxBuffer.BufferData(0, (uint8*)m_gpuDrawData.lvgIndices.data(), sizeof(LinaVG::Index) * m_gpuDrawData.lvgIndices.size());

		m_uploadQueue.AddBufferRequest(&currentFrame.lvgVtxBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.lvgIdxBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.argumentsBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.boneBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.entityBuffer);
		m_uploadQueue.AddBufferRequest(&currentFrame.lightBuffer);

		m_deferredPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);
		m_forwardPass.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

		m_shapeRenderer.AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

#ifdef JPH_DEBUG_RENDERER
		if (m_physicsDebugRenderer)
			m_physicsDebugRenderer->AddBuffersToUploadQueue(frameIndex, m_uploadQueue);

#endif
	}

	void WorldRenderer::FlushTransfers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		if (m_uploadQueue.FlushAll(currentFrame.copyStream))
			BumpAndSendTransfers(frameIndex);
	}

	void WorldRenderer::Render(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		// Global set.
		LinaGX::CMDBindDescriptorSets* bindGlobal = currentFrame.gfxStream->AddCommand<LinaGX::CMDBindDescriptorSets>();
		bindGlobal->descriptorSetHandles		  = currentFrame.gfxStream->EmplaceAuxMemory<uint16>(m_gfxContext->GetDescriptorSetGlobal(frameIndex));
		bindGlobal->firstSet					  = 0;
		bindGlobal->setCount					  = 1;
		bindGlobal->layoutSource				  = LinaGX::DescriptorSetsLayoutSource::CustomLayout;
		bindGlobal->customLayout				  = m_gfxContext->GetPipelineLayoutGlobal();

		{
			// Barrier to Attachment
			LinaGX::CMDBarrier* barrierToAttachment	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrierToAttachment->srcStageFlags		 = LinaGX::PSF_TopOfPipe;
			barrierToAttachment->dstStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrierToAttachment->textureBarrierCount = 5;
			barrierToAttachment->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 5);
			barrierToAttachment->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.gBufAlbedo->GetGPUHandle());
			barrierToAttachment->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.gBufPosition->GetGPUHandle());
			barrierToAttachment->textureBarriers[2]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.gBufNormal->GetGPUHandle());
			barrierToAttachment->textureBarriers[3]	 = GfxHelpers::GetTextureBarrierDepthRead2Att(currentFrame.gBufDepth->GetGPUHandle());
			barrierToAttachment->textureBarriers[4]	 = GfxHelpers::GetTextureBarrierColorRead2Att(currentFrame.lightingPassOutput->GetGPUHandle());
		}

		uint16 lastBoundShader = 0;
		bool   shaderBound	   = false;

		// DEFERRED PASS
		{
			DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Deferred Pass");

			m_deferredPass.Begin(currentFrame.gfxStream, frameIndex);
			m_deferredPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_gfxContext->GetPipelineLayoutPersistent(RenderPassType::RENDER_PASS_DEFERRED), 1);
			m_deferredPass.Render(frameIndex, currentFrame.gfxStream);
			m_deferredPass.End(currentFrame.gfxStream);

			DEBUG_LABEL_END(currentFrame.gfxStream);
		}

		// Barrier to shader read.
		{
			LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
			barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
			barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
			barrier->textureBarrierCount = 3;
			barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 3);
			barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufAlbedo->GetGPUHandle());
			barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufPosition->GetGPUHandle());
			barrier->textureBarriers[2]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.gBufNormal->GetGPUHandle());
		}

		// FORWARD PASS
		{
			DEBUG_LABEL_BEGIN(currentFrame.gfxStream, "Forward Pass");

			m_forwardPass.Begin(currentFrame.gfxStream, frameIndex);
			m_forwardPass.BindDescriptors(currentFrame.gfxStream, frameIndex, m_gfxContext->GetPipelineLayoutPersistent(RenderPassType::RENDER_PASS_FORWARD), 1);
			m_forwardPass.Render(frameIndex, currentFrame.gfxStream);
			m_forwardPass.End(currentFrame.gfxStream);

			DEBUG_LABEL_END(currentFrame.gfxStream);

			auto& currentFrame = m_pfd[frameIndex];

			// Barrier to shader read or transfer read
			if (m_snapshotBuffer == nullptr)
			{
				LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
				barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
				barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
				barrier->textureBarrierCount = 2;
				barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 2);
				barrier->textureBarriers[0]	 = GfxHelpers::GetTextureBarrierColorAtt2Read(currentFrame.lightingPassOutput->GetGPUHandle());
				barrier->textureBarriers[1]	 = GfxHelpers::GetTextureBarrierDepthAtt2Read(currentFrame.gBufDepth->GetGPUHandle());
			}
			else
			{
				LinaGX::CMDBarrier* barrier	 = currentFrame.gfxStream->AddCommand<LinaGX::CMDBarrier>();
				barrier->srcStageFlags		 = LinaGX::PSF_ColorAttachment | LinaGX::PSF_EarlyFragment;
				barrier->dstStageFlags		 = LinaGX::PSF_FragmentShader;
				barrier->textureBarrierCount = 1;
				barrier->textureBarriers	 = currentFrame.gfxStream->EmplaceAuxMemorySizeOnly<LinaGX::TextureBarrier>(sizeof(LinaGX::TextureBarrier) * 1);
				barrier->textureBarriers[0]	 = {
					 .texture		 = currentFrame.lightingPassOutput->GetGPUHandle(),
					 .isSwapchain	 = false,
					 .toState		 = LinaGX::TextureState::TransferSource,
					 .srcAccessFlags = LinaGX::AF_ColorAttachmentRead,
					 .dstAccessFlags = LinaGX::AF_ShaderRead,
				 };
			}
		}
	}

	void WorldRenderer::CloseAndSend(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];

		m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);

		Vector<uint16> waitSemaphoresVec;
		Vector<uint64> waitValuesVec;

		// If transfers exist.
		if (currentFrame.copySemaphore.IsModified())
		{
			waitSemaphoresVec.push_back(currentFrame.copySemaphore.GetSemaphore());
			waitValuesVec.push_back(currentFrame.copySemaphore.GetValue());
		}
		currentFrame.signalSemaphore.Increment();
		m_lgx->SubmitCommandStreams({
			.targetQueue		  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
			.streams			  = &currentFrame.gfxStream,
			.streamCount		  = 1,
			.useWait			  = !waitSemaphoresVec.empty(),
			.waitCount			  = static_cast<uint32>(waitSemaphoresVec.size()),
			.waitSemaphores		  = waitSemaphoresVec.data(),
			.waitValues			  = waitValuesVec.data(),
			.useSignal			  = true,
			.signalCount		  = 1,
			.signalSemaphores	  = currentFrame.signalSemaphore.GetSemaphorePtr(),
			.signalValues		  = currentFrame.signalSemaphore.GetValuePtr(),
			.standaloneSubmission = m_standaloneSubmit,
		});

		if (m_snapshotBuffer != nullptr)
		{
			m_lgx->WaitForUserSemaphore(currentFrame.signalSemaphore.GetSemaphore(), currentFrame.signalSemaphore.GetValue());

			LinaGX::CMDCopyTexture2DToBuffer* copy = currentFrame.gfxStream->AddCommand<LinaGX::CMDCopyTexture2DToBuffer>();
			copy->destBuffer					   = m_snapshotBuffer->GetGPUResource();
			copy->srcLayer						   = 0;
			copy->srcMip						   = 0;
			copy->srcTexture					   = currentFrame.lightingPassOutput->GetGPUHandle();

			currentFrame.copySemaphore.Increment();
			m_lgx->CloseCommandStreams(&currentFrame.gfxStream, 1);
			m_lgx->SubmitCommandStreams({
				.targetQueue		  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Graphics),
				.streams			  = &currentFrame.gfxStream,
				.streamCount		  = 1,
				.useWait			  = true,
				.waitCount			  = 1,
				.waitSemaphores		  = currentFrame.signalSemaphore.GetSemaphorePtr(),
				.waitValues			  = currentFrame.signalSemaphore.GetValuePtr(),
				.useSignal			  = true,
				.signalCount		  = 1,
				.signalSemaphores	  = currentFrame.copySemaphore.GetSemaphorePtr(),
				.signalValues		  = currentFrame.copySemaphore.GetValuePtr(),
				.standaloneSubmission = m_standaloneSubmit,
			});
		}
	}

	uint64 WorldRenderer::BumpAndSendTransfers(uint32 frameIndex)
	{
		auto& currentFrame = m_pfd[frameIndex];
		currentFrame.copySemaphore.Increment();
		m_lgx->CloseCommandStreams(&currentFrame.copyStream, 1);
		m_lgx->SubmitCommandStreams({
			.targetQueue		  = m_lgx->GetPrimaryQueue(LinaGX::CommandType::Transfer),
			.streams			  = &currentFrame.copyStream,
			.streamCount		  = 1,
			.useSignal			  = true,
			.signalCount		  = 1,
			.signalSemaphores	  = currentFrame.copySemaphore.GetSemaphorePtr(),
			.signalValues		  = currentFrame.copySemaphore.GetValuePtr(),
			.standaloneSubmission = m_standaloneSubmit,
		});
		return currentFrame.copySemaphore.GetValue();
	}

	bool WorldRenderer::DrawEntityExists(uint32& outIndex, const EntityIdent& ident)
	{
		const uint32 entitiesSize = static_cast<uint32>(m_cpuDrawData.entities.size());
		for (uint32 i = 0; i < entitiesSize; i++)
		{
			const DrawEntity& de = m_cpuDrawData.entities.at(i);

			if (de.ident.entityGUID == 0 && de.ident.entityGUID == de.ident.uniqueID2 == de.ident.uniqueID3 == de.ident.uniqueID4)
				continue;

			if (de.ident == ident)
			{
				outIndex = i;
				return true;
			}
		}
		return false;
	}

} // namespace Lina
