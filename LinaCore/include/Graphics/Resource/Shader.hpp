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

#ifndef Shader_HPP
#define Shader_HPP

#include "Resources/Core/IResource.hpp"
#include "Graphics/Core/CommonGraphics.hpp"
#include "Data/String.hpp"
#include "Data/Vector.hpp"
#include "Data/HashMap.hpp"
#include "Data/Bitmask.hpp"

namespace Lina
{
	class MaterialPropertyBase;
	class Renderer;
	class IPipeline;

	class Shader : public IResource
	{
	public:
		Shader(ResourceManager* rm, bool isUserManaged, const String& path, StringID sid);
		virtual ~Shader();

		const ShaderByteCode& GetCompiledCode(ShaderStage stage) const;

		inline Bitmask16 GetDrawPassMask() const
		{
			return m_drawPassMask;
		}

		inline const Vector<MaterialPropertyBase*>& GetProperties() const
		{
			return m_properties;
		}

		inline const Vector<MaterialPropertyBase*>& GetTextures() const
		{
			return m_textures;
		}

		const HashMap<ShaderStage, String>& GetStages()
		{
			return m_stages;
		}

		const Vector<UserBinding>& GetBindings()
		{
			return m_materialBindings;
		}

		inline PipelineType GetPipelineType() const
		{
			return m_pipelineType;
		}

	protected:
		// Inherited via IResource
		virtual void LoadFromFile(const char* path) override;
		virtual void SaveToStream(OStream& stream) override;
		virtual void LoadFromStream(IStream& stream) override;
		virtual void Upload() override;
		virtual void Flush() override;

	private:
		friend class Renderer;

		IPipeline*							 m_pipeline = nullptr;
		Renderer*							 m_renderer = nullptr;
		Vector<MaterialPropertyBase*>		 m_properties;
		Vector<MaterialPropertyBase*>		 m_textures;
		HashMap<ShaderStage, String>		 m_stages;
		Vector<UserBinding>					 m_materialBindings;
		HashMap<ShaderStage, ShaderByteCode> m_compiledCode;
		PipelineType						 m_pipelineType = PipelineType::Standard;

		// Runtime
		String	  m_text	  = "";
		Bitmask16 m_drawPassMask;
	};

} // namespace Lina

#endif
