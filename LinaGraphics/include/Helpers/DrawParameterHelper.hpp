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
