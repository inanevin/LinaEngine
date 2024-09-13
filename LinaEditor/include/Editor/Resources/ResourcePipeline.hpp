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
}
namespace Lina::Editor
{

	class Editor;

	class ResourcePipeline
	{
	public:
		void Initialize(Editor* editor);

		/// For creating and saving user-made resources, such as GUIWidget, EntityWorld, Material etc.
		ResourceID SaveNewResource(TypeID tid, uint32 subType = 0);

		/// Save resource to its supposed path by its id.
		void SaveResource(Resource* res);

		/// For importing external resources, such as textures, models, materials etc.
		void ImportResources(ResourceDirectory* src, const Vector<String>& absPaths);

		/// Load a resource from a directory, not meant to be used by runtime but for editor inspecting/editing purposes.
		template <typename T> T* OpenResource(ResourceID resourceID, void* subdata = nullptr)
		{
			return static_cast<T*>(OpenResource(GetTypeID<T>(), resourceID, subdata));
		}

		/// Delete an opened resource, save first.
		template <typename T> void CloseAndSaveResource(T* resource)
		{
			SaveResource(resource);
			delete resource;
		}

		/// Delete an opened resource.
		template <typename T> void CloseResource(T* resource)
		{
			delete resource;
		}

		String GetResourceDirectory();
		String GetResourcePath(ResourceID id);
		void   DuplicateResource(ResourceDirectory* directory, ResourceDirectory* newParent);

	private:
		void  VerifyResources(ResourceDirectory* dir);
		void* OpenResource(TypeID tid, ResourceID resourceID, void* subdata);
		void  GenerateThumbnailAtlases(ResourceDirectory* dir);

	private:
		Editor*		   m_editor					= nullptr;
		Atomic<uint32> m_importedResourcesCount = 0;
	};

} // namespace Lina::Editor
