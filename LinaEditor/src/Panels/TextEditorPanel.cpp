/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-2020] [Inan Evin]

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

#include "Panels/TextEditorPanel.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/ImGuiCommon.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/InputBackend.hpp"
#include "imgui/imguicolortextedit/TextEditor.h"
#include "EventSystem/EventSystem.hpp"
#include "Core/GUILayer.hpp"
#include "Rendering/ShaderInclude.hpp"

namespace Lina::Editor
{
    TextEditor editor;

    void TextEditorPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);

        Event::EventSystem::Get()->Connect<EShortcut, &TextEditorPanel::OnShortcut>(this);

        auto lang = TextEditor::LanguageDefinition::GLSL();

        static const char* ppnames[]  = {"NULL"};
        static const char* ppvalues[] = {
            "#define NULL ((void*)0)"};

        for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = ppvalues[i];
            lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
        }

        static const char* identifiers[] = {
            "attribute", "uniform", "varying", "layout", "centroid", "flat", "smooth", "noperspective", "patch", "sample", "subroutine", "in", "out", "inout",
            "invariant", "discard", "mat2", "mat3", "mat4", "dmat2", "dmat3", "dmat4", "mat2x2", "mat2x3", "mat2x4", "dmat2x2", "dmat2x3", "dmat2x4", "mat3x2", "mat3x3",
            "mat3x4", "dmat3x2", "dmat3x3", "dmat3x4", "mat4x2", "mat4x3", "mat4x4 ", "dmat4x2", "dmat4x3", "dmat4x4", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "bvec2",
            "bvec3", "bvec4", "dvec2", "dvec3", "dvec4", "uvec2", "uvec3", "uvec4", "lowp", "mediump", "highp", "precision", "sampler1D", "sampler2D", "sampler3D", "samplerCube",
            "sampler1DShadow", "sampler2DShadow", "samplerCubeShadow", "sampler1DArray", "sampler2DArray", "sampler1DArrayShadow", "sampler2DArrayShadow",
            "isampler1D", "isampler2D", "isampler3D", "isamplerCube", "isampler1DArray", "isampler2DArray", "usampler1D", "usampler2D", "usampler3D", "usamplerCube",
            "usampler1DArray", "usampler2DArray", "sampler2DRect", "sampler2DRectShadow", "isampler2DRect", "usampler2DRect", "samplerBuffer", "isamplerBuffer", "usamplerBuffer",
            "sampler2DMS", "isampler2DMS", "usampler2DMS", "sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray", "samplerCubeArray", "samplerCubeArrayShadow",
            "isamplerCubeArray",
            "usamplerCubeArray", "MaterialSamplerCube", "MaterialSampler2D"};

        static const char* idecls[] =
            {
                "attribute", "uniform", "varying", "layout", "centroid", "flat", "smooth", "noperspective", "patch", "sample", "subroutine", "in", "out", "inout",
                "invariant", "discard", "mat2", "mat3", "mat4", "dmat2", "dmat3", "dmat4", "mat2x2", "mat2x3", "mat2x4", "dmat2x2", "dmat2x3", "dmat2x4", "mat3x2", "mat3x3",
                "mat3x4", "dmat3x2", "dmat3x3", "dmat3x4", "mat4x2", "mat4x3", "mat4x4 ", "dmat4x2", "dmat4x3", "dmat4x4", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "bvec2",
                "bvec3", "bvec4", "dvec2", "dvec3", "dvec4", "uvec2", "uvec3", "uvec4", "lowp", "mediump", "highp", "precision", "sampler1D", "sampler2D", "sampler3D", "samplerCube",
                "sampler1DShadow", "sampler2DShadow", "samplerCubeShadow", "sampler1DArray", "sampler2DArray", "sampler1DArrayShadow", "sampler2DArrayShadow",
                "isampler1D", "isampler2D", "isampler3D", "isamplerCube", "isampler1DArray", "isampler2DArray", "usampler1D", "usampler2D", "usampler3D", "usamplerCube",
                "usampler1DArray", "usampler2DArray", "sampler2DRect", "sampler2DRectShadow", "isampler2DRect", "usampler2DRect", "samplerBuffer", "isamplerBuffer", "usamplerBuffer",
                "sampler2DMS", "isampler2DMS", "usampler2DMS", "sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray", "samplerCubeArray", "samplerCubeArrayShadow",
                "isamplerCubeArray",
                "usamplerCubeArray", "MaterialSamplerCube", "MaterialSampler2D"};

        for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = std::string(idecls[i]);
            lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
        }

        editor.SetShowWhitespaces(false);
        editor.SetLanguageDefinition(lang);
    }

    void TextEditorPanel::Draw()
    {
        if (m_show)
        {
            m_windowFlags |= ImGuiWindowFlags_MenuBar;
            if (Begin())
            {
                const ImGuiTabBarFlags flags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;
                ImGui::SetCursorPosX(CURSOR_X_LABELS);
                const ImVec2 currentCursor = ImGui::GetCursorPos();
                const ImVec2 textFieldSize = ImVec2(m_currentWindowSize.x - CURSOR_X_LABELS * 2.0f, m_currentWindowSize.y - currentCursor.y - 50.0f);

                WidgetsUtility::PushPopupStyle();

                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("File"))
                    {
                        if (ImGui::MenuItem("Save", "CTRL + S"))
                        {
                            SaveCurrentFile(editor.GetText());
                        }

                        if (ImGui::MenuItem("Reload", "CTRL + R"))
                        {
                            ReloadCurrentFile();
                        }

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Edit"))
                    {
                        bool ro = editor.IsReadOnly();
                        if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                            editor.SetReadOnly(ro);
                        ImGui::Separator();

                        if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                            editor.Undo();
                        if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                            editor.Redo();

                        ImGui::Separator();

                        if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                            editor.Copy();
                        if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                            editor.Cut();
                        if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                            editor.Delete();
                        if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                            editor.Paste();

                        ImGui::Separator();

                        if (ImGui::MenuItem("Select all", nullptr, nullptr))
                            editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                        ImGui::EndMenu();
                    }

                    ImGui::EndMenuBar();
                }

                WidgetsUtility::PopPopupStyle();

                WidgetsUtility::IncrementCursorPosY(24.0f);

                if (ImGui::BeginTabBar("texteditortabbar", flags))
                {
                    std::vector<Utility::File*> markedForErase;
                    for (auto& [file, contents] : m_openFiles)
                    {
                        bool isOpen = true;
                        if (ImGui::BeginTabItem(file->m_fullName.c_str(), &isOpen, ImGuiTabItemFlags_None))
                        {
                            if (ImGui::IsItemHovered())
                                WidgetsUtility::Tooltip(file->m_fullPath.c_str());

                            if (m_lastFileOnTextEditor != file)
                            {
                                m_lastFileOnTextEditor = file;
                                editor.SetText(m_openFiles[file]);
                            }
                            ImGui::PushFont(GUILayer::Get()->GetTextEditorFont());
                            editor.Render("TextEditor");
                            ImGui::PopFont();
                            ImGui::EndTabItem();
                        }

                        if (!isOpen)
                            markedForErase.push_back(file);
                    }

                    for (auto& file : markedForErase)
                    {
                        m_openFiles.erase(file);
                        if (file == m_lastFileOnTextEditor)
                            m_lastFileOnTextEditor = nullptr;
                    }

                    ImGui::EndTabBar();
                }
                End();
            }
        }
    }

    void TextEditorPanel::Close()
    {
        EditorPanel::Close();
        m_lastFileOnTextEditor = nullptr;
    }

    void TextEditorPanel::AddFile(Utility::File* file)
    {
        m_openFiles[file] = Utility::GetFileContents(file->m_fullPath);

        if (m_lastFileOnTextEditor == file)
            editor.SetText(m_openFiles[file]);
    }

    void TextEditorPanel::OnShortcut(const EShortcut& ev)
    {
        if (!m_show || m_lastFileOnTextEditor == nullptr)
            return;

        if (ev.m_name.compare("save") == 0)
            SaveCurrentFile(editor.GetText());
        else if (ev.m_name.compare("reload") == 0)
            ReloadCurrentFile();
    }

    void TextEditorPanel::SaveCurrentFile(const std::string& text)
    {
        if (m_lastFileOnTextEditor == nullptr)
            return;

        Utility::RewriteFileContents(m_lastFileOnTextEditor, text);

        const std::string  path   = m_lastFileOnTextEditor->m_fullPath;
        const StringIDType sid    = m_lastFileOnTextEditor->m_sid;
        TypeID             tid    = 0;
        bool               reload = false;

        if (m_lastFileOnTextEditor->m_extension.compare("glh") == 0)
        {
            reload = true;
            tid    = GetTypeID<Graphics::ShaderInclude>();
            Resources::ResourceStorage::Get()->Unload<Graphics::ShaderInclude>(sid);
        }
        else if (m_lastFileOnTextEditor->m_extension.compare("glsl") == 0)
        {
            reload = true;
            tid    = GetTypeID<Graphics::Shader>();
            Resources::ResourceStorage::Get()->Unload<Graphics::Shader>(sid);
        }

        if (reload)
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{m_lastFileOnTextEditor->m_fullPath, tid, sid});
    }

    void TextEditorPanel::ReloadCurrentFile()
    {
        if (m_lastFileOnTextEditor != nullptr)
        {
            m_openFiles.erase(m_lastFileOnTextEditor);
            AddFile(m_lastFileOnTextEditor);
        }
    }

} // namespace Lina::Editor