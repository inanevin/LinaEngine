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

#pragma once

#include "Core/Resources/CommonResources.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/World/CommonWorld.hpp"

namespace LinaGX
{
	class Instance;
	class CommandStream;
} // namespace LinaGX

namespace Lina
{
	class Component;
	class EntityWorld;
	class ResourceManagerV2;
	class CompModel;
	class JobExecutor;
	class View;
	class GfxContext;
	class Buffer;

	struct StaticDraw
	{
	};

	class DrawCollector
	{
	public:
		struct DrawEntity
		{
			GPUEntity entity;
			EntityID  entityGUID = 0;
			uint32	  uniqueID2	 = 0;
			uint32	  uniqueID3	 = 0;
			uint32	  uniqueID4	 = 0;
		};

		struct DrawCall
		{
			uint32	shaderHandle		   = 0;
			Buffer* vertexBuffer		   = nullptr;
			Buffer* indexBuffer			   = nullptr;
			size_t	vertexSize			   = 0;
			uint32	pushConstant		   = 0;
			uint32	baseVertexLocation	   = 0;
			uint32	indexCountPerInstance  = 0;
			uint32	instanceCount		   = 0;
			uint32	startIndexLocation	   = 0;
			uint32	vertexCountPerInstance = 0;
			uint32	startInstanceLocation  = 0;
		};

		struct ModelDrawInstance
		{
			CompModel* compModel		= nullptr;
			GPUEntity  customEntityData = {};
			EntityID   customEntityGUID = {};
			ResourceID materialID		= 0;
		};

		struct CustomDrawInstance
		{
			GPUDrawArguments args;
			ResourceID		 materialID = 0;
			GPUEntity		 entity;
			EntityID		 entityGUID					   = 0;
			bool			 useEntityAsFirstArgument	   = true;
			bool			 useMaterialIDAsSecondArgument = true;
		};

		struct ModelDraw
		{
			ResourceID		  modelID		 = 0;
			ResourceID		  shaderID		 = 0;
			StringID		  shaderVariant	 = 0;
			uint32			  meshIndex		 = 0;
			uint32			  primitiveIndex = 0;
			bool			  isSkinned		 = false;
			ModelDrawInstance instance;
		};

		struct CustomDraw
		{
			Buffer*			   vertexBuffer			 = nullptr;
			Buffer*			   indexBuffer			 = nullptr;
			size_t			   vertexSize			 = 0;
			ResourceID		   shaderID				 = 0;
			StringID		   shaderVariant		 = 0;
			uint32			   baseVertexLocation	 = 0;
			uint32			   indexCountPerInstance = 0;
			uint32			   startIndexLocation	 = 0;
			CustomDrawInstance instance;
		};

		struct CustomDrawRaw
		{
			ResourceID		   shaderID				  = 0;
			StringID		   shaderVariant		  = 0;
			uint32			   baseVertexLocation	  = 0;
			uint32			   vertexCountPerInstance = 0;
			CustomDrawInstance instance;
		};

		struct InstancedModelDraw
		{
			ResourceID				  modelID;
			ResourceID				  shaderID;
			StringID				  shaderVariant;
			uint32					  meshIndex;
			uint32					  primitiveIndex;
			Vector<ModelDrawInstance> instances;

			bool operator==(const ModelDraw& other) const
			{
				return modelID == other.modelID && shaderID == other.shaderID && shaderVariant == other.shaderVariant && meshIndex == other.meshIndex && primitiveIndex == other.primitiveIndex;
			}
		};

		struct InstancedCustomDraw
		{
			Buffer*					   vertexBuffer;
			Buffer*					   indexBuffer;
			size_t					   vertexSize;
			ResourceID				   shaderID;
			StringID				   shaderVariant;
			uint32					   baseVertexLocation;
			uint32					   indexCountPerInstance;
			uint32					   startIndexLocation;
			Vector<CustomDrawInstance> instances;

			bool operator==(const CustomDraw& other) const
			{
				return shaderID == other.shaderID && shaderVariant == other.shaderVariant && vertexBuffer == other.vertexBuffer && indexBuffer == other.indexBuffer && baseVertexLocation == other.baseVertexLocation &&
					   indexCountPerInstance == other.indexCountPerInstance && startIndexLocation == other.startIndexLocation;
			}
		};

		struct InstancedCustomDrawRaw
		{
			ResourceID				   shaderID;
			StringID				   shaderVariant;
			uint32					   baseVertexLocation;
			uint32					   vertexCountPerInstance;
			Vector<CustomDrawInstance> instances;

			bool operator==(const CustomDrawRaw& other) const
			{
				return shaderID == other.shaderID && shaderVariant == other.shaderVariant && baseVertexLocation == other.baseVertexLocation && vertexCountPerInstance == other.vertexCountPerInstance;
			}
		};

		struct DrawGroup
		{
			StringID			  id   = 0;
			String				  name = "";
			Vector<ModelDraw>	  modelDraws;
			Vector<CustomDraw>	  customDraws;
			Vector<CustomDrawRaw> customRawDraws;

			void AddOtherWithOverride(const DrawGroup& other, StringID newShaderVariantStatic, StringID newShaderVariantSkinned)
			{
				const size_t modelDrawSizeNow	  = modelDraws.size();
				const size_t customDrawSizeNow	  = customDraws.size();
				const size_t customRawDrawSizeNow = customRawDraws.size();

				modelDraws.insert(modelDraws.end(), other.modelDraws.begin(), other.modelDraws.end());
				customDraws.insert(customDraws.end(), other.customDraws.begin(), other.customDraws.end());
				customRawDraws.insert(customRawDraws.end(), other.customRawDraws.begin(), other.customRawDraws.end());

				for (size_t i = modelDrawSizeNow; i < modelDraws.size(); i++)
				{
					ModelDraw& draw	   = modelDraws[i];
					draw.shaderVariant = draw.isSkinned ? newShaderVariantSkinned : newShaderVariantStatic;
				}

				for (size_t i = customDrawSizeNow; i < customDraws.size(); i++)
				{
					CustomDraw& draw   = customDraws[i];
					draw.shaderVariant = newShaderVariantStatic;
				}

				for (size_t i = customRawDrawSizeNow; i < customRawDraws.size(); i++)
				{
					CustomDrawRaw& draw = customRawDraws[i];
					draw.shaderVariant	= newShaderVariantStatic;
				}
			}
		};

		struct DrawData
		{
			Vector<DrawGroup> drawGroups;
		};

		struct RenderingGroup
		{
			StringID		 id	  = 0;
			String			 name = "";
			Vector<DrawCall> drawCalls;
		};

		struct RenderingData
		{
			Vector<RenderingGroup>	 groups;
			Vector<DrawEntity>		 entities;
			Vector<Matrix4>			 bones;
			Vector<GPUDrawArguments> instanceData;
		};

		DrawCollector(){};
		virtual ~DrawCollector() = default;

		void Initialize(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm, GfxContext* context, JobExecutor* executor);
		void Shutdown();

		void OnComponentAdded(Component* comp);
		void OnComponentRemoved(Component* comp);

		void CollectCompModels(DrawGroup& group, const View& view, ShaderType shaderType);
		void AddModelDraw(DrawGroup& group, ResourceID model, uint32 meshIndex, uint32 primitiveIndex, ResourceID shaderID, StringID shaderVariant, const ModelDrawInstance& instance);
		void AddCustomDraw(DrawGroup& group, const CustomDrawInstance& inst, ResourceID shaderID, StringID shaderVariant, Buffer* vertexBuffer, Buffer* indexBuffer, size_t vertexSize, uint32 baseVertex, uint32 indexCount, uint32 startIndex);
		void AddCustomDrawRaw(DrawGroup& group, const CustomDrawInstance& inst, ResourceID shaderID, StringID shaderVariant, uint32 baseVertex, uint32 vtxCount);
		void PrepareGPUData();

		void			CreateGroup(const String& name);
		DrawGroup&		GetGroup(StringID groupId);
		RenderingGroup& GetRenderGroup(StringID groupId);
		bool			GroupExists(StringID groupId) const;
		bool			RenderGroupExists(StringID groupId) const;

		void SyncRender();
		void RenderGroup(StringID groupId, LinaGX::CommandStream* stream);

		inline const RenderingData& GetRenderingData() const
		{
			return m_renderingData;
		}

	private:
		void CalculateSkinning(const Vector<CompModel*>& comps);

		bool DrawEntityExists(uint32& outIndex, uint64 uid, uint32 uid2, uint32 uid3, uint32 uid4);

		void PrepareModelDraws(Vector<CompModel*>& skinnedModels, const DrawGroup& group, RenderingGroup& renderingGroup);
		void PrepareCustomDraws(const DrawGroup& group, RenderingGroup& renderingGroup);
		void PrepareCustomDrawsRaw(const DrawGroup& group, RenderingGroup& renderingGroup);

	private:
		JobExecutor*	   m_jobExecutor = nullptr;
		GfxContext*		   m_gfxContext	 = nullptr;
		LinaGX::Instance*  m_lgx		 = nullptr;
		ResourceManagerV2* m_rm			 = nullptr;
		EntityWorld*	   m_world		 = nullptr;
		DrawData		   m_cpuDraw	 = {};
		Vector<CompModel*> m_compModels;
		RenderingData	   m_renderingData;
	};
} // namespace Lina
