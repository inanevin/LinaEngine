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

#include "Core/Graphics/CommonGraphics.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"

namespace Lina
{
	bool ShaderPropertyDefinition::VerifySimilarity(const Vector<ShaderPropertyDefinition>& v1, const Vector<ShaderPropertyDefinition>& v2)
	{
		if (v1.empty() || v2.empty())
			return true;

		for (const ShaderPropertyDefinition& def : v1)
		{
			auto it = linatl::find_if(v2.begin(), v2.end(), [def](const ShaderPropertyDefinition& propDef) -> bool { return def.type == propDef.type && def.sid == propDef.sid; });
			if (it == v2.end())
				return false;
		}
		return true;
	}

	LINA_CLASS_BEGIN(LinaTexture2D)
	LINA_FIELD(LinaTexture2D, texture, "Texture", FieldType::ResourceID, GetTypeID<Texture>())
	LINA_FIELD(LinaTexture2D, sampler, "Sampler", FieldType::ResourceID, GetTypeID<TextureSampler>())
	LINA_CLASS_END(LinaTexture2D)

} // namespace Lina
