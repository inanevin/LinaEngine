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
#include "Common/Data/CommonData.hpp"

namespace Lina
{
	class ResourceDirectory;
	class ResourceManagerV2;
	class Model;
} // namespace Lina
namespace Lina::Editor
{

	class Editor;

	class ResourcePipeline
	{
	public:
		void Initialize(Editor* editor);

		/// For creating and saving user-made resources, such as GUIWidget, EntityWorld, Material etc.
		ResourceID SaveNewResource(TypeID tid, uint32 subType = 0);

		/// For importing external resources, such as textures, models, materials etc.
		void ImportResources(ResourceDirectory* src, const Vector<String>& absPaths);

		void DuplicateResource(ResourceManagerV2* resMan, ResourceDirectory* directory, ResourceDirectory* newParent);

		void GenerateThumbnailAtlases(ResourceDirectory* dir);
		void GenerateThumbnailForResource(ResourceDirectory* dir, Resource* resource, bool refreshAtlases);

	private:
		void VerifyResources(ResourceDirectory* dir);

	private:
		JobExecutor	   m_executor;
		Editor*		   m_editor					= nullptr;
		Atomic<uint32> m_importedResourcesCount = 0;
	};

} // namespace Lina::Editor
