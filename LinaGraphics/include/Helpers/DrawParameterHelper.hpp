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
            params.m_useScissorTest = false;
            params.m_useDepthTest   = true;
            params.m_useStencilTest = true;
            params.primitiveType  = PrimitiveType::PRIMITIVE_TRIANGLES;
            params.m_faceCulling    = FaceCulling::FACE_CULL_BACK;
            // params.sourceBlend             = BlendFunc::BLEND_FUNC_SRC_ALPHA;
            // params.destBlend               = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
            params.m_sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.m_destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.m_shouldWriteDepth        = true;
            params.m_depthFunc               = DrawFunc::DRAW_FUNC_LESS;
            params.m_stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.m_stencilComparisonVal    = 1;
            params.m_stencilTestMask         = 0xFF;
            params.m_stencilWriteMask        = 0x00;
            params.m_stencilFail             = StencilOp::STENCIL_KEEP;
            params.m_stencilPass             = StencilOp::STENCIL_REPLACE;
            params.m_stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
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
            params.m_useScissorTest          = false;
            params.m_useDepthTest            = true;
            params.m_useStencilTest          = true;
            params.primitiveType           = PrimitiveType::PRIMITIVE_TRIANGLE_STRIP;
            params.m_faceCulling             = FaceCulling::FACE_CULL_NONE;
            // params.sourceBlend             = BlendFunc::BLEND_FUNC_SRC_ALPHA;
            // params.destBlend               = BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
            params.m_sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.m_destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.m_shouldWriteDepth        = false;
            params.m_depthFunc               = DrawFunc::DRAW_FUNC_LEQUAL;
            params.m_stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.m_stencilComparisonVal    = 0;
            params.m_stencilTestMask         = 0xFF;
            params.m_stencilWriteMask        = 0xFF;
            params.m_stencilFail             = StencilOp::STENCIL_KEEP;
            params.m_stencilPass             = StencilOp::STENCIL_REPLACE;
            params.m_stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
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
            params.m_useScissorTest          = false;
            params.m_useDepthTest            = false;
            params.m_useStencilTest          = true;
            params.primitiveType           = PrimitiveType::PRIMITIVE_TRIANGLES;
            params.m_faceCulling             = FaceCulling::FACE_CULL_NONE;
            params.m_sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.m_destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.m_shouldWriteDepth        = true;
            params.m_depthFunc               = DrawFunc::DRAW_FUNC_LESS;
            params.m_stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.m_stencilComparisonVal    = 1;
            params.m_stencilTestMask         = 0xFF;
            params.m_stencilWriteMask        = 0xFF;
            params.m_stencilFail             = StencilOp::STENCIL_KEEP;
            params.m_stencilPass             = StencilOp::STENCIL_REPLACE;
            params.m_stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
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
            params.m_useScissorTest          = false;
            params.m_useDepthTest            = true;
            params.m_useStencilTest          = false;
            params.primitiveType           = PrimitiveType::PRIMITIVE_TRIANGLES;
            params.m_faceCulling             = FaceCulling::FACE_CULL_NONE;
            params.m_sourceBlend             = BlendFunc::BLEND_FUNC_NONE;
            params.m_destBlend               = BlendFunc::BLEND_FUNC_NONE;
            params.m_shouldWriteDepth        = true;
            params.m_depthFunc               = DrawFunc::DRAW_FUNC_LESS;
            params.m_stencilFunc             = DrawFunc::DRAW_FUNC_ALWAYS;
            params.m_stencilComparisonVal    = 1;
            params.m_stencilTestMask         = 0xFF;
            params.m_stencilWriteMask        = 0xFF;
            params.m_stencilFail             = StencilOp::STENCIL_KEEP;
            params.m_stencilPass             = StencilOp::STENCIL_REPLACE;
            params.m_stencilPassButDepthFail = StencilOp::STENCIL_KEEP;
            params.scissorStartX           = 0;
            params.scissorStartY           = 0;
            params.scissorWidth            = 0;
            params.scissorHeight           = 0;
            return params;
        }

        static DrawParams GetGUILayer()
        {
            DrawParams params;
            params.m_useScissorTest          = false;
            params.m_useDepthTest            = true;
            params.m_useStencilTest          = true;
            params.primitiveType           = Graphics::PrimitiveType::PRIMITIVE_TRIANGLES;
            params.m_faceCulling             = Graphics::FaceCulling::FACE_CULL_BACK;
            params.m_sourceBlend             = Graphics::BlendFunc::BLEND_FUNC_SRC_ALPHA;
            params.m_destBlend               = Graphics::BlendFunc::BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
            params.m_shouldWriteDepth        = true;
            params.m_depthFunc               = Graphics::DrawFunc::DRAW_FUNC_LEQUAL;
            params.m_stencilFunc             = Graphics::DrawFunc::DRAW_FUNC_ALWAYS;
            params.m_stencilComparisonVal    = 0;
            params.m_stencilTestMask         = 0xFF;
            params.m_stencilWriteMask        = 0xFF;
            params.m_stencilFail             = Graphics::StencilOp::STENCIL_KEEP;
            params.m_stencilPass             = Graphics::StencilOp::STENCIL_REPLACE;
            params.m_stencilPassButDepthFail = Graphics::StencilOp::STENCIL_KEEP;
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
