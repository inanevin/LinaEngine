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

#include "Core/Graphics/Resource/Shader.hpp"
#include "Common/Platform/LinaGXIncl.hpp"
#include "Common/Data/String.hpp"
#include "Core/Graphics/Data/RenderData.hpp"
#include "Core/Graphics/CommonGraphics.hpp"

namespace LinaGX
{
	enum class CullMode;
	enum class Format;
	enum class FrontFace;
} // namespace LinaGX

namespace Lina
{
	struct ShaderVariant;

	class ShaderPreprocessor
	{
	public:
		static ShaderType		 GetShaderType(const String& str);
		static BlendMode		 GetBlendModeFromStr(const String& str);
		static DepthTesting		 GetDepthTestingFromStr(const String& str);
		static LinaGX::Format	 GetTargetFromStr(const String& str, bool& isMSAA);
		static LinaGX::CullMode	 GetCullFromStr(const String& str);
		static LinaGX::FrontFace GetFaceFromStr(const String& str);
		static void				 ExtractVariants(const String& input, Vector<ShaderVariant>& variants);
		static bool				 VerifyFullShader(const String& input);
		static bool				 ExtractVertexFrag(const String& input, String& outVertex, String& outFrag);
		static void				 InjectVersionAndExtensions(String& input, bool insertAtHeader = false);
		static bool				 InjectMaterialIfRequired(String& input, Vector<ShaderPropertyDefinition>& outProperties);
		static void				 InjectUserShader(String& input, const String& shader);
		static String			 MakeVariantBlock(const String& userBlock, const String& variant, const String& renderPassInclude);
		static ShaderVariant	 MakeVariant(const String& name, const String& vertexBlock, const String& fragBlock, LinaGX::CullMode cull, BlendMode blend, DepthTesting depth, const Vector<LinaGX::Format>& targets);
		static void				 ApplyBlending(ShaderVariant& variant, BlendMode blendMode);
		static void				 ApplyDepth(ShaderVariant& variant, DepthTesting depth);
	};
} // namespace Lina
