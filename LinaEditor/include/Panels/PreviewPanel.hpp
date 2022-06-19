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
Class: PreviewPanel



Timestamp: 1/4/2022 6:18:09 PM
*/

#pragma once

#ifndef ModelPanel_HPP
#define ModelPanel_HPP

// Headers here.
#include "Panels/EditorPanel.hpp"
#include "Math/Quaternion.hpp"
#include "Data/Vector.hpp"
namespace Lina
{
    namespace Graphics
    {
        class Model;
        class Texture;
        class Shader;
        class Material;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    enum class PreviewType
    {
        None,
        Model,
        Material,
        Texture
    };

    class PreviewPanel : public EditorPanel
    {

    public:
        PreviewPanel()  = default;
        ~PreviewPanel();

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;
        virtual void Open() override;

        void SetTargetModel(Graphics::Model* model);
        void SetTargetTexture(Graphics::Texture* texture);
        void SetTargetMaterial(Graphics::Material* mat);

    private:
        void DrawPreviewArea();
        void BeginPreviewCamera(const Vector2& bgSize);
        void EndPreviewCamera(const Vector2& bgMin, const Vector2& bgMax);
        void PreviewModelSelector(const Vector2& cursorPos, float confineRectHeight);

    private:
        int                      m_currentSelectedPrimitive = 0;
        Vector<String> m_primitives;
        Graphics::Model*         m_targetModel             = nullptr;
        Graphics::Texture*       m_targetTexture           = nullptr;
        Graphics::Material*      m_targetMaterial          = nullptr;
        Graphics::Material*      m_shaderPreviewMaterial   = nullptr;
        Graphics::Shader*        m_targetShader            = nullptr;
        float                    m_leftPaneWidth           = 0.0f;
        float                    m_leftPaneMaxWidth        = 0.0f;
        float                    m_leftPaneMinWidth        = 0.0f;
        float                    m_resizeDividerPressedPos = 0.0f;
        bool                     m_draggingVerticalDivider = false;
        Vector2                  m_rightPaneSize           = Vector2::Zero;
        Vector3                  m_previewCameraPosition   = Vector3::Zero;
        Quaternion               m_previewCameraRotation   = Quaternion();
        Vector2                  m_mouseDragStart          = Vector2::Zero;
        Vector2                  m_mouseAngles             = Vector2::Zero;
        PreviewType              m_previewType             = PreviewType::None;
    };
} // namespace Lina::Editor

#endif
