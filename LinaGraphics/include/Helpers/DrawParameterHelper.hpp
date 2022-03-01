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

/*
Class: DrawParameterHelper

Helper responsible for generating appropriate draw parameters for various draw categories.

Timestamp: 10/29/2020 6:57:41 PM
*/

#pragma once

#ifndef DrawParameterHelper_HPP
#define DrawParameterHelper_HPP

#include "Rendering/RenderingCommon.hpp"

namespace Lina::Graphics
{
    class DrawParameterHelper
    {

    public:
        DrawParameterHelper()  = default;
        ~DrawParameterHelper() = default;

        static DrawParams GetDefault()
        {
            // Set default drawing parameters.
            DrawParams params;
            params.skipParameters = false;
            params.useScissorTest = false;
            params.useDepthTest   = true;
            params.useStencilTest = true;
            params.primitiveType  = PrimitiveType::PRIMITIVE_TRIANGLES;
            params.faceCulling    = FaceCulling::FACE_CULL_BACK;
            // params.sourceBlend             = BlendFunc::BLEND_FUNC_SRC_ALPHA;
            // params.destBlend               = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
            params.sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.shouldWriteDepth        = true;
            params.depthFunc               = DrawFunc::DRAW_FUNC_LESS;
            params.stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.stencilComparisonVal    = 1;
            params.stencilTestMask         = 0xFF;
            params.stencilWriteMask        = 0x00;
            params.stencilFail             = StencilOp::STENCIL_KEEP;
            params.stencilPass             = StencilOp::STENCIL_REPLACE;
            params.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
            params.scissorStartX           = 0;
            params.scissorStartY           = 0;
            params.scissorWidth            = 0;
            params.scissorHeight           = 0;

            return params;
        }

        static DrawParams GetSkybox()
        {
            DrawParams params;
            params.skipParameters          = false;
            params.useScissorTest          = false;
            params.useDepthTest            = true;
            params.useStencilTest          = true;
            params.primitiveType           = PrimitiveType::PRIMITIVE_TRIANGLE_STRIP;
            params.faceCulling             = FaceCulling::FACE_CULL_NONE;
            // params.sourceBlend             = BlendFunc::BLEND_FUNC_SRC_ALPHA;
            // params.destBlend               = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
            params.sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.shouldWriteDepth        = false;
            params.depthFunc               = DrawFunc::DRAW_FUNC_LEQUAL;
            params.stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.stencilComparisonVal    = 0;
            params.stencilTestMask         = 0xFF;
            params.stencilWriteMask        = 0xFF;
            params.stencilFail             = StencilOp::STENCIL_KEEP;
            params.stencilPass             = StencilOp::STENCIL_REPLACE;
            params.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
            params.scissorStartX           = 0;
            params.scissorStartY           = 0;
            params.scissorWidth            = 0;
            params.scissorHeight           = 0;
            return params;
        }

        static DrawParams GetFullScreenQuad()
        {
            DrawParams params;
            params.skipParameters          = false;
            params.useScissorTest          = false;
            params.useDepthTest            = false;
            params.useStencilTest          = true;
            params.primitiveType           = PrimitiveType::PRIMITIVE_TRIANGLES;
            params.faceCulling             = FaceCulling::FACE_CULL_NONE;
            params.sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.shouldWriteDepth        = true;
            params.depthFunc               = DrawFunc::DRAW_FUNC_LESS;
            params.stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.stencilComparisonVal    = 1;
            params.stencilTestMask         = 0xFF;
            params.stencilWriteMask        = 0xFF;
            params.stencilFail             = StencilOp::STENCIL_KEEP;
            params.stencilPass             = StencilOp::STENCIL_REPLACE;
            params.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
            params.scissorStartX           = 0;
            params.scissorStartY           = 0;
            params.scissorWidth            = 0;
            params.scissorHeight           = 0;
            return params;
        }

        static DrawParams GetShadowMap()
        {
            DrawParams params;
            params.skipParameters          = false;
            params.useScissorTest          = false;
            params.useDepthTest            = true;
            params.useStencilTest          = false;
            params.primitiveType           = PrimitiveType::PRIMITIVE_TRIANGLES;
            params.faceCulling             = FaceCulling::FACE_CULL_NONE;
            params.sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.shouldWriteDepth        = true;
            params.depthFunc               = DrawFunc::DRAW_FUNC_LESS;
            params.stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.stencilComparisonVal    = 1;
            params.stencilTestMask         = 0xFF;
            params.stencilWriteMask        = 0xFF;
            params.stencilFail             = StencilOp::STENCIL_KEEP;
            params.stencilPass             = StencilOp::STENCIL_REPLACE;
            params.stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
            params.scissorStartX           = 0;
            params.scissorStartY           = 0;
            params.scissorWidth            = 0;
            params.scissorHeight           = 0;
            return params;
        }

        static DrawParams GetGUILayer()
        {
            DrawParams params;
            params.useScissorTest          = false;
            params.useDepthTest            = true;
            params.useStencilTest          = true;
            params.primitiveType           = Graphics::PrimitiveType::PRIMITIVE_TRIANGLES;
            params.faceCulling             = Graphics::FaceCulling::FACE_CULL_BACK;
            params.sourceBlend             = Graphics::BlendFunc::BLEND_FUNC_SRC_ALPHA;
            params.destBlend               = Graphics::BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
            params.shouldWriteDepth        = true;
            params.depthFunc               = Graphics::DrawFunc::DRAW_FUNC_LEQUAL;
            params.stencilFunc             = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
            params.stencilComparisonVal    = 0;
            params.stencilTestMask         = 0xFF;
            params.stencilWriteMask        = 0xFF;
            params.stencilFail             = Graphics::StencilOp::STENCIL_KEEP;
            params.stencilPass             = Graphics::StencilOp::STENCIL_REPLACE;
            params.stencilPassButDepthFail = Graphics::StencilOp::STENCIL_KEEP;
            params.scissorStartX           = 0;
            params.scissorStartY           = 0;
            params.scissorWidth            = 0;
            params.scissorHeight           = 0;
            return params;
        }

    private:
    };
} // namespace Lina::Graphics

#endif
