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

#include "Core/EditorRenderer.hpp"
#include "EventSystem/EventSystem.hpp"
#include "EventSystem/GraphicsEvents.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "Graphics/Core/GUIBackend.hpp"
#include "Graphics/Core/Screen.hpp"
#include "Resource/Core/ResourceManager.hpp"
#include "Graphics/Resource/Texture.hpp"
#include "Graphics/Resource/Font.hpp"
#include "Core/EditorGUIManager.hpp"
#include "Platform/LinaVGIncl.hpp"
#include "Graphics/Core/GraphicsCommon.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Input/Core/InputMappings.hpp"
#include "GUI/GUI.hpp"
#include "Platform/LinaVGIncl.hpp"

namespace Lina::Editor
{
    void EditorRenderer::Initialize(Graphics::GUIBackend* guiBackend)
    {
        m_guiBackend = guiBackend;
        // Event::EventSystem::Get()->Connect<Event::EOnEditorDrawBegin, &EditorRenderer::OnEditorDrawBegin>(this);
      
    }

    void EditorRenderer::Shutdown()
    {
        // Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawBegin>(this);
     
    }



} // namespace Lina::Editor
