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

#include "Common/Data/String.hpp"
#include "Common/Data/Mutex.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/Map.hpp"
#include "Core/Resources/ResourceManagerListener.hpp"

namespace Lina
{
	class Buffer;
	struct TextureAtlasImage;
	class JobExecutor;
	class WorldRenderer;
	class GfxManager;
} // namespace Lina

namespace Lina::Editor
{
	class Editor;
	struct DirectoryItem;

	class ThumbnailGenerator : public ResourceManagerListener
	{
	private:
		struct RequestBatch
		{
			uint32													 totalCount		= 0;
			Atomic<uint32>											 generatedCount = 0;
			Vector<DirectoryItem*>									 requests;
			ParallelHashMapMutex<DirectoryItem*, TextureAtlasImage*> atlases;
		};

	public:
		ThumbnailGenerator() = delete;
		ThumbnailGenerator(Editor* editor, JobExecutor* executor, DirectoryItem* item, bool isRecursive);
		~ThumbnailGenerator();

		virtual void OnResourceLoadEnded(int32 taskID, const Vector<ResourceIdentifier>& idents) override;

	private:
		void CollectItems(DirectoryItem* item, bool isRecursive);
		void GenerateThumbnailForItem(DirectoryItem* item, RequestBatch* batch);
		void GenerateThumbTexture(DirectoryItem* item, const String& thumbPath);
		void GenerateThumbFont(DirectoryItem* item, const String& thumbPath);
		void GenerateThumbMaterial(DirectoryItem* item, const String& thumbPath, RequestBatch* batch);
		void GenerateThumbModel(DirectoryItem* item, const String& thumbPath, RequestBatch* batch);

		WorldRenderer* CreateDataForModel(DirectoryItem* item);
		WorldRenderer* CreateDataForMaterial(DirectoryItem* item);

	private:
		ResourceManager*										 m_rm		  = nullptr;
		GfxManager*												 m_gfxManager = nullptr;
		Vector<DirectoryItem*>									 m_thumbnailItems;
		uint32													 m_totalCount	  = 0;
		Atomic<uint32>											 m_generatedCount = 0;
		ParallelHashMapMutex<DirectoryItem*, TextureAtlasImage*> m_atlases;

		JobExecutor* m_executor;
		Editor*		 m_editor = nullptr;
	};

} // namespace Lina::Editor
