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
			uint64	  uniqueID	= 0;
			uint32	  uniqueID2 = 0;
			uint32	  uniqueID3 = 0;
			uint32	  uniqueID4 = 0;
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
			ResourceID materialID		= 0;
		};

		struct ModelDraw
		{
			ResourceID				  modelID		 = 0;
			uint32					  shaderHandle	 = 0;
			uint32					  meshIndex		 = 0;
			uint32					  primitiveIndex = 0;
			Vector<ModelDrawInstance> instances;
		};

		struct CustomDrawInstance
		{
			GPUDrawArguments args;
			GPUEntity		 entity;
			bool			 useEntityAsFirstArgument = true;
		};

		struct CustomDraw
		{
			Buffer* vertexBuffer		  = nullptr;
			Buffer* indexBuffer			  = nullptr;
			size_t	vertexSize			  = 0;
			uint32	shaderHandle		  = 0;
			uint32	baseVertexLocation	  = 0;
			uint32	indexCountPerInstance = 0;
			uint32	startIndexLocation	  = 0;

			Vector<CustomDrawInstance> instances;
		};

		struct CustomDrawRaw
		{
			uint32 shaderHandle			  = 0;
			uint32 baseVertexLocation	  = 0;
			uint32 vertexCountPerInstance = 0;

			Vector<CustomDrawInstance> instances;
		};

		struct DrawGroup
		{
			StringID			  id = 0;
			Vector<ModelDraw>	  modelDraws;
			Vector<CustomDraw>	  customDraws;
			Vector<CustomDrawRaw> customRawDraws;
		};

		struct DrawData
		{
			Vector<DrawGroup> drawGroups;
		};

		struct RenderingGroup
		{
			StringID		 id = 0;
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

		void Initialize(LinaGX::Instance* lgx, EntityWorld* world, ResourceManagerV2* rm, GfxContext* context);
		void Shutdown();

		void OnComponentAdded(Component* comp);
		void OnComponentRemoved(Component* comp);

		void CollectCompModels(DrawGroup& group, const View& view, uint32 shaderOverrideHandle, ShaderType shaderType);
		void AddModelDraw(DrawGroup& group, ResourceID model, uint32 meshIndex, uint32 primitiveIndex, uint32 shaderHandle, const GPUEntity& customEntityData);
		void AddCustomDraw(DrawGroup& group, const CustomDrawInstance& inst, uint32 shaderHandle, Buffer* vertexBuffer, Buffer* indexBuffer, size_t vertexSize, uint32 baseVertex, uint32 indexCount, uint32 startIndex);
		void AddCustomDrawRaw(DrawGroup& group, const CustomDrawInstance& inst, uint32 shaderHandle, uint32 baseVertex, uint32 vtxCount);

		void PrepareGPUData(JobExecutor& executor);

		void	   CreateGroup(StringID groupId);
		DrawGroup& GetGroup(StringID groupId);

		void SyncRender();
		void RenderGroup(StringID groupId, LinaGX::CommandStream* stream);

		inline const RenderingData& GetRenderingData() const
		{
			return m_renderingData;
		}

	private:
		void CalculateSkinning(const Vector<CompModel*>& comps, JobExecutor& executor);

		bool DrawEntityExists(uint32& outIndex, uint64 uid, uint32 uid2, uint32 uid3, uint32 uid4);

	private:
		GfxContext*		   m_gfxContext = nullptr;
		LinaGX::Instance*  m_lgx		= nullptr;
		ResourceManagerV2* m_rm			= nullptr;
		EntityWorld*	   m_world		= nullptr;
		DrawData		   m_cpuDraw	= {};
		DrawData		   m_gpuDraw	= {};
		Vector<CompModel*> m_compModels;
		RenderingData	   m_renderingData;
	};
} // namespace Lina
