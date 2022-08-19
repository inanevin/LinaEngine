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

#include "Lina/ReflectionRegistry.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Core/ComponentCache.hpp"
#include "Utility/StringId.hpp"

/// <summary>
/// *****************************************************************************
/// *
/// *   THIS IS AN AUTOMATICALLY UPDATED FILE, DO NOT CHANGE THE IDENTIFIER LINES
/// *   BEFORE EACH BUILD, LINA HEADER TOOL PROCESSES ALL THE HEADERS WITHIN THE PROJECT
/// *   AND FILES THE REFLECTED COMPONENTS & PROPERTIES, WHICH ARE THEN ADDED TO THIS FILE
/// *   AUTOMATICALLY.
/// *
/// *****************************************************************************
/// </summary>


//INC_BEGIN - !! DO NOT MODIFY THIS LINE !!
#include "Components/Renderable.hpp"
#include "Settings/EditorSettings.hpp"
#include "Settings/EngineSettings.hpp"
#include "Settings/RenderSettings.hpp"
//INC_END - !! DO NOT MODIFY THIS LINE !!

namespace Lina
{
    bool g_reflectedTypesRegistered;

    template <typename T>
    void* REF_CreateComponentFunc()
    {
        T* t = new T();
        return static_cast<void*>(t);
    }

    template <typename T>
    void* REF_CreateComponentCacheFunc()
    {
        World::ComponentCache<T>* c = new World::ComponentCache<T>();
        return static_cast<void*>(c);
    }

    template <typename T>
    void REF_DestroyComponentFunc(void* ptr)
    {
        T* t = static_cast<T*>(ptr);
        delete t;
    }
    struct Test
    {
    };

    void RegisterReflectedTypes()
    {
        g_reflectedTypesRegistered = true;

//REGFUNC_BEGIN - !! DO NOT MODIFY THIS LINE !!
Reflection::Meta<World::RenderableComponent>().AddProperty("Title"_hs,"Renderable");
Reflection::Meta<World::RenderableComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<World::RenderableComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<World::RenderableComponent>);
Reflection::Meta<World::RenderableComponent>().createFunc = std::bind(&REF_CreateComponentFunc<World::RenderableComponent>);
Reflection::Meta<World::RenderableComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<World::RenderableComponent>, std::placeholders::_1);
Reflection::Meta<Editor::EditorSettings>().AddProperty("Title"_hs,"Editor Settings");
Reflection::Meta<Editor::EditorSettings>().AddField<&Editor::EditorSettings::m_textEditorPath, Editor::EditorSettings>("m_textEditorPath"_hs);
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Title"_hs,"Text Editor");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Type"_hs,"StringPath");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Tooltip"_hs,"Default text editor to open shader & similar files.");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Lina::EngineSettings>().AddProperty("Title"_hs,"Engine Settings");
Reflection::Meta<Lina::RenderSettings>().AddProperty("Title"_hs,"Render Settings");
//REGFUNC_END - !! DO NOT MODIFY THIS LINE !!
    }
} // namespace Lina
