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

#ifndef Material_HPP
#define Material_HPP

#include "Resources/Core/Resource.hpp"
#include "Graphics/Data/RenderData.hpp"
#include "Platform/LinaGXIncl.hpp"

namespace Lina
{
	class MaterialPropertyBase;
	class Shader;

	class Material : public Resource
	{
	public:
		Material(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid) : Resource(rm, isUserManaged, path, sid, GetTypeID<Material>()){};
		virtual ~Material();

		/// <summary>
		///
		/// </summary>
		/// <param name="sid"></param>
		void SetShader(StringID sid);

		/// <summary>
		///
		/// </summary>
		/// <param name="ptr"></param>
		/// <param name="size"></param>
		void GetDataBlob(uint8* ptr, size_t size);

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="name"></param>
		/// <param name="value"></param>
		template <typename T> inline void SetProperty(const String& name, T& value)
		{
			// const StringID sid = TO_SID(name);
			//
			// size_t dataIndex = 0;
			// for (const auto& mem : m_uboDefinition.members)
			// {
			// 	const StringID memSid = TO_SID(mem.name);
			//
			// 	if (memSid == sid)
			// 	{
			// 		MEMCPY(m_data + dataIndex, &value, sizeof(T));
			// 		break;
			// 	}
			//
			// 	dataIndex += mem.alignment;
			// }
		}

		/// <summary>
		///
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <param name="name"></param>
		/// <returns></returns>
		template <typename T> inline T GetProperty(const String& name)
		{
			// const StringID sid = TO_SID(name);
			//
			// size_t dataIndex = 0;
			// for (const auto& mem : m_uboDefinition.members)
			// {
			// 	const StringID memSid = TO_SID(mem.name);
			//
			// 	if (memSid == sid)
			// 	{
			// 		T* ptr = reinterpret_cast<T*>(m_data + dataIndex);
			// 		return *ptr;
			// 	}
			//
			// 	dataIndex += mem.alignment;
			// }
			//
			// return nullptr;
		}

	protected:
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void BatchLoaded() override;

	private:
		uint32 m_shader		   = 0;
		uint8* m_data		   = nullptr;
		size_t m_totalDataSize = 0;
	};

} // namespace Lina

#endif
