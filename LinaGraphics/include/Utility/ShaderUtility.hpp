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

#ifndef ShaderUtility_HPP
#define ShaderUtility_HPP

#include "Data/String.hpp"
#include "Data/HashMap.hpp"
#include "Core/GraphicsCommon.hpp"
#include "PipelineObjects/DescriptorSetLayout.hpp"
#include "Data/Bitmask.hpp"

namespace Lina::Graphics
{

    class ShaderUtility
    {
    public:
        /// <summary>
        /// Returns a shader block from a full linashader text.
        /// </summary>
        /// <param name="shader">Full text.</param>
        /// <param name="defineStart">Define string, e.g. #LINA_VS or #LINA_FRAG </param>
        /// <returns></returns>
        static String GetShaderStageText(const String& shader, const String& defineStart);

        /// <summary>
        /// Scans and returns the push constants in the shader text.
        /// </summary>
        /// <param name="moduleText"></param>
        /// <param name="offset"></param>
        /// <param name="size"></param>
        static Vector<PushConstantRange> CheckForPushConstants(ShaderStage stage, const String& moduleText);

        /// <summary>
        /// Scans and returns the descriptor sets in the shader text.
        /// </summary>
        /// <param name="moduleText"></param>
        /// <param name="offset"></param>
        /// <param name="size"></param>
        static Vector<ShaderDescriptorSetInfo> CheckForDescriptorSets(uint32 stageFlags, const String& moduleText);

        /// <summary>
        /// Scans and returns the draw passes the shader should be registered to.
        /// </summary>
        static Bitmask16 GetPassMask(const String& text);

        /// <summary>
        /// Fills in the given map by checking material uniforms in the shader text.
        /// </summary>
        /// <param name="text"></param>
        static void FillMaterialProperties(const String& text, HashMap<uint32, Vector<String>>& map);
    };
} // namespace Lina::Graphics

#endif
