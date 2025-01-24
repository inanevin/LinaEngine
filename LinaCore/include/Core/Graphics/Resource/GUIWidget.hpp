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

#include "Core/Resources/Resource.hpp"
#include "Core/GUI/Widgets/Widget.hpp"

namespace Lina
{
	class GUIWidget : public Resource
	{

	public:
		GUIWidget(ResourceID id = 0, const String& name = "") : Resource(id, GetTypeID<GUIWidget>(), name) {};

		static constexpr uint32 VERSION = 0;

		virtual ~GUIWidget();
		virtual bool		LoadFromFile(const String& path) override;
		virtual void		SaveToStream(OStream& stream) const override;
		virtual void		LoadFromStream(IStream& stream) override;
		virtual void		GenerateHW() override;
		virtual void		DestroyHW() override;
		void				SetStream(const RawStream& stream, Widget* root);
		HashSet<ResourceID> GetResourceReferences();

		inline const RawStream& GetStream() const
		{
			return m_rootStream;
		}

	private:
		ALLOCATOR_BUCKET_MEM;
		Vector<ResourceID> m_references;
		RawStream		   m_rootStream = {};
	};

	LINA_RESOURCE_BEGIN(GUIWidget);
	LINA_CLASS_END(GUIWidget);

} // namespace Lina
