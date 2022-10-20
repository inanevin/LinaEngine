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
#include "Core/GUIBackend.hpp"
#include "Core/Screen.hpp"

#include <LinaVG/LinaVG.hpp>

using namespace LinaVG;

namespace Lina::Editor
{

    void EditorRenderer::Initialize()
    {
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawBegin, &EditorRenderer::OnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDraw, &EditorRenderer::OnEditorDraw>(this);
        Event::EventSystem::Get()->Connect<Event::EOnEditorDrawEnd, &EditorRenderer::OnEditorDrawEnd>(this);
    }

    void EditorRenderer::Shutdown()
    {
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawBegin>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDraw>(this);
        Event::EventSystem::Get()->Disconnect<Event::EOnEditorDrawEnd>(this);
    }

    void EditorRenderer::OnEditorDrawBegin(const Event::EOnEditorDrawBegin& ev)
    {
        Graphics::GUIBackend::Get()->SetCmd(ev.cmd);

        const Vector2 screen = Graphics::Screen::SizeF();

        float aq = LinaVG::Config.displayWidth;
        LinaVG::StartFrame();
        StyleOptions opts;
        opts.isFilled = true;
        opts.color    = Vec4(0.1f, 0, 0, 1);
        DrawRect(Vec2(5, 5), Vec2(screen.x - 5, 100), opts, 0);

        // opts.color = Vec4(0.1f, 0.25f, 0, 1);
        // DrawRect(Vec2(800, 800), Vec2(1000, 1000), opts, 40);

        LinaVG::Render();
        LinaVG::EndFrame();
    }

    void EditorRenderer::OnEditorDraw(const Event::EOnEditorDraw& ev)
    {
        Graphics::GUIBackend::Get()->RecordDrawCommands();
    }

    void EditorRenderer::OnEditorDrawEnd(const Event::EOnEditorDrawEnd& ev)
    {
    }

} // namespace Lina::Editor
