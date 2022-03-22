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
#include "Math/Math.hpp"

namespace Lina::Editor
{
    static const char* ppnames[]  = {"LINA_PROJ", "LINA_VIEW", "LINA_LIGHTSPACE", "LINA_VP", "LINA_CAMPOS", "LINA_CAMNEAR", "LINA_CAMFAR", "LINA_AMBIENT",
                                    "LINA_PLIGHT_COUNT", "LINA_SLIGHT_COUNT", "LINA_VISUALIZEDEPTH", "LINA_SCREENSIZE", "LINA_MOUSEPOS", "LINA_DT", "LINA_ET"};
    static const char* ppvalues[] = {
        "Active camera projection matrix", "Active camera view matrix.", "Point light matrix.", "Active camera projection * view matrix.",
        "Active camera position.", "Active camera near plane.", "Active camera far plane.", "Level ambient color",
        "Total point light count.", "Total spotlight count.", "Whether depth visualization is enabled or not.", "Current screen resolution (whole editor if enabled).",
        "Mouse position (0,0 top-left, screenSize.x, screenSize.y bottom-right)", "Delta time.", "Elapsed time."};

    static const char* const keywords[] = {
        "const",
        "uniform",
        "buffer",
        "shared",
        "attribute",
        "varying",
        "coherent",
        "volatile",
        "restrict",
        "readonly",
        "writeonly",
        "atomic_uint",
        "layout",
        "centroid",
        "flat",
        "smooth",
        "noperspective",
        "patch",
        "sample",
        "invariant",
        "precise",
        "break",
        "continue",
        "do",
        "for",
        "while",
        "switch",
        "case",
        "default",
        "if",
        "else",
        "subroutine",
        "in",
        "out",
        "inout",
        "int",
        "void",
        "bool",
        "true",
        "false",
        "float",
        "double",
        "discard",
        "return",
        "vec2",
        "vec3",
        "vec4",
        "ivec2",
        "ivec3",
        "ivec4",
        "bvec2",
        "bvec3",
        "bvec4",
        "uint",
        "uvec2",
        "uvec3",
        "uvec4",
        "dvec2",
        "dvec3",
        "dvec4",
        "mat2",
        "mat3",
        "mat4",
        "mat2x2",
        "mat2x3",
        "mat2x4",
        "mat3x2",
        "mat3x3",
        "mat3x4",
        "mat4x2",
        "mat4x3",
        "mat4x4",
        "dmat2",
        "dmat3",
        "dmat4",
        "dmat2x2",
        "dmat2x3",
        "dmat2x4",
        "dmat3x2",
        "dmat3x3",
        "dmat3x4",
        "dmat4x2",
        "dmat4x3",
        "dmat4x4",
        "lowp",
        "mediump",
        "highp",
        "precision",
        "sampler1D",
        "sampler1DShadow",
        "sampler1DArray",
        "sampler1DArrayShadow",
        "isampler1D",
        "isampler1DArray",
        "usampler1D usampler1DArray",
        "sampler2D",
        "sampler2DShadow",
        "sampler2DArray",
        "sampler2DArrayShadow",
        "isampler2D",
        "isampler2DArray",
        "usampler2D",
        "usampler2DArray",
        "sampler2DRect",
        "sampler2DRectShadow",
        "isampler2DRect",
        "usampler2DRect",
        "sampler2DMS",
        "isampler2DMS",
        "usampler2DMS",
        "sampler2DMSArray",
        "isampler2DMSArray",
        "usampler2DMSArray",
        "sampler3D",
        "isampler3D",
        "usampler3D",
        "samplerCube",
        "samplerCubeShadow",
        "isamplerCube",
        "usamplerCube",
        "samplerCubeArray",
        "samplerCubeArrayShadow",
        "isamplerCubeArray",
        "usamplerCubeArray",
        "samplerBuffer",
        "isamplerBuffer",
        "usamplerBuffer",
        "image1D",
        "iimage1D",
        "uimage1D",
        "image1DArray",
        "iimage1DArray",
        "uimage1DArray",
        "image2D",
        "iimage2D",
        "uimage2D",
        "image2DArray",
        "iimage2DArray",
        "uimage2DArray",
        "image2DRect",
        "iimage2DRect",
        "uimage2DRect",
        "image2DMS",
        "iimage2DMS",
        "uimage2DMS",
        "image2DMSArray",
        "iimage2DMSArray",
        "uimage2DMSArray",
        "image3D",
        "iimage3D",
        "uimage3D",
        "imageCube",
        "iimageCube",
        "uimageCube",
        "imageCubeArray",
        "iimageCubeArray",
        "uimageCubeArray",
        "imageBuffer",
        "iimageBuffer",
        "uimageBuffer",
        "struct",
        "MaterialSampler2D",
        "MaterialSamplerCube",
    };

    static const char* const identifiers[] = {
        "radians", "degrees", "sin", "cos", "tan", "asin", "acos", "atan", "sinh", "cosh", "tanh", "asinh", "acosh", "atanh",
        "pow", "exp", "log", "exp2", "log2", "sqrt", "inversesqrt",
        "abs", "sign", "floor", "trunc", "round", "roundEven", "ceil", "fract", "mod", "modf", "min", "max", "clamp", "mix", "step", "smoothstep", "isnan", "isinf", "floatBitsToInt", "floatBitsToUint", "intBitsToFloat", "uintBitsToFloat", "fma", "frexp", "ldexp",
        "packUnorm2x16", "packSnorm2x16", "packUnorm4x8", "packSnorm4x8", "unpackUnorm2x16", "unpackSnorm2x16", "unpackUnorm4x8", "unpackSnorm4x8", "packHalf2x16", "unpackHalf2x16", "packDouble2x32", "unpackDouble2x32",
        "length", "distance", "dot", "cross", "normalize", "ftransform", "faceforward", "reflect", "refract",
        "matrixCompMult", "outerProduct", "transpose", "determinant", "inverse",
        "lessThan", "lessThanEqual", "greaterThan", "greaterThanEqual", "equal", "notEqual", "any", "all", "not",
        "uaddCarry", "usubBorrow", "umulExtended", "imulExtended", "bitfieldExtract", "bitfieldInsert", "bitfieldReverse", "bitCount", "findLSB", "findMSB",
        "textureSize", "textureQueryLod", "textureQueryLevels", "textureSamples",
        "texture", "textureProj", "textureLod", "textureOffset", "texelFetch", "texelFetchOffset", "textureProjOffset", "textureLodOffset", "textureProjLod", "textureProjLodOffset", "textureGrad", "textureGradOffset", "textureProjGrad", "textureProjGradOffset",
        "textureGather", "textureGatherOffset", "textureGatherOffsets",
        "texture1D", "texture1DProj", "texture1DLod", "texture1DProjLod", "texture2D", "texture2DProj", "texture2DLod", "texture2DProjLod", "texture3D", "texture3DProj", "texture3DLod", "texture3DProjLod", "textureCube", "textureCubeLod", "shadow1D", "shadow2D", "shadow1DProj", "shadow2DProj", "shadow1DLod", "shadow2DLod", "shadow1DProjLod", "shadow2DProjLod",
        "atomicCounterIncrement", "atomicCounterDecrement", "atomicCounter", "atomicCounterAdd", "atomicCounterSubtract", "atomicCounterMin", "atomicCounterMax", "atomicCounterAnd", "atomicCounterOr", "atomicCounterXor", "atomicCounterExchange", "atomicCounterCompSwap",
        "atomicAdd", "atomicMin", "atomicMax", "atomicAnd", "atomicOr", "atomicXor", "atomicExchange", "atomicCompSwap",
        "imageSize", "imageSamples", "imageLoad", "imageStore", "imageAtomicAdd", "imageAtomicMin", "imageAtomicMax", "imageAtomicAnd", "imageAtomicOr", "imageAtomicXor", "imageAtomicExchange", "imageAtomicCompSwap",
        "EmitStreamVertex", "EndStreamPrimitive", "EmitVertex", "EndPrimitive",
        "dFdx", "dFdy", "dFdxFine", "dFdyFine", "dFdxCoarse", "dFdyCoarse", "fwidth", "fwidthFine", "fwidthCoarse",
        "interpolateAtCentroid", "interpolateAtSample", "interpolateAtOffset",
        "noise1", "noise2", "noise3", "noise4",
        "barrier",
        "memoryBarrier", "memoryBarrierAtomicCounter", "memoryBarrierBuffer", "memoryBarrierShared", "memoryBarrierImage", "groupMemoryBarrier",
        "subpassLoad",
        "anyInvocation", "allInvocations", "allInvocationsEqual"};

    TextEditor::LanguageDefinition    m_langDefinition;
    std::map<std::string, TextEditor> m_editors;

    void TextEditorPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);

        Event::EventSystem::Get()->Connect<EShortcut, &TextEditorPanel::OnShortcut>(this);

        m_langDefinition = TextEditor::LanguageDefinition::GLSL();

        for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = ppvalues[i];
            m_langDefinition.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
        }

        for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i)
        {
            TextEditor::Identifier id;
            m_langDefinition.mKeywords.insert(keywords[i]);
        }

        for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
        {
            TextEditor::Identifier id;
            // id.mDeclaration = std::string(idecls[i]);
            m_langDefinition.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
        }
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
                const ImVec2 textFieldSize = ImVec2(m_currentWindowSize.x - CURSOR_X_LABELS * 2.0f, m_currentWindowSize.y - currentCursor.y - 50.0f * GUILayer::Get()->GetDPIScale());

                WidgetsUtility::PushPopupStyle();

                if (ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("File"))
                    {
                        if (ImGui::MenuItem("Save", "CTRL + S"))
                        {
                            SaveCurrentFile(m_editors[m_currentFile->m_fullPath].GetText());
                        }

                        if (ImGui::MenuItem("Reload", "CTRL + R"))
                        {
                            ReloadCurrentFile();
                        }

                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Edit"))
                    {
                        auto& editor = m_editors[m_currentFile->m_fullPath];
                        bool  ro     = editor.IsReadOnly();
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

                WidgetsUtility::IncrementCursorPosY(24.0f * GUILayer::Get()->GetDPIScale());

                // Control font scale.
                if (Input::InputEngineBackend::Get()->GetKey(LINA_KEY_LCTRL))
                {
                    const Vector2 mouseScroll = Input::InputEngineBackend::Get()->GetMouseScroll() * 0.1f;
                    m_currentFontSize += mouseScroll.y;
                    m_currentFontSize = Math::Clamp(m_currentFontSize, 0.6f, 1.2f);
                }

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

                            if (m_currentFile != file)
                            {
                                m_currentFile = file;
                            }
                            ImGui::PushFont(GUILayer::Get()->GetTextEditorFont());
                            WidgetsUtility::PushScaledFont(m_currentFontSize);
                            m_editors[m_currentFile->m_fullPath].Render("TextEditor");
                            WidgetsUtility::PopScaledFont();
                            ImGui::PopFont();
                            ImGui::EndTabItem();
                        }

                        if (!isOpen)
                            markedForErase.push_back(file);
                    }

                    for (auto& file : markedForErase)
                    {
                        m_openFiles.erase(file);
                        if (file == m_currentFile)
                            m_currentFile = nullptr;
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
        m_currentFile = nullptr;
    }

    void TextEditorPanel::AddFile(Utility::File* file)
    {
        m_openFiles[file] = Utility::GetFileContents(file->m_fullPath);
        auto& editor      = m_editors[file->m_fullPath];

        editor.SetLanguageDefinition(m_langDefinition);
        editor.SetShowWhitespaces(false);
        editor.SetText(m_openFiles[file]);
    }

    void TextEditorPanel::OnShortcut(const EShortcut& ev)
    {
        if (!m_show || m_currentFile == nullptr)
            return;

        if (ev.m_name.compare("save") == 0)
            SaveCurrentFile(m_editors[m_currentFile->m_fullPath].GetText());
        else if (ev.m_name.compare("reload") == 0)
            ReloadCurrentFile();
    }

    void TextEditorPanel::SaveCurrentFile(const std::string& text)
    {
        if (m_currentFile == nullptr)
            return;

        Utility::RewriteFileContents(m_currentFile, text);

        const std::string  path   = m_currentFile->m_fullPath;
        const StringIDType sid    = m_currentFile->m_sid;
        TypeID             tid    = 0;
        bool               reload = false;

        if (m_currentFile->m_extension.compare("linaglh") == 0)
        {
            reload = true;
            tid    = GetTypeID<Graphics::ShaderInclude>();
            Resources::ResourceStorage::Get()->Unload<Graphics::ShaderInclude>(sid);
        }
        else if (m_currentFile->m_extension.compare("linaglsl") == 0)
        {
            reload = true;
            tid    = GetTypeID<Graphics::Shader>();
            Resources::ResourceStorage::Get()->Unload<Graphics::Shader>(sid);
        }

        if (reload)
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{m_currentFile->m_fullPath, tid, sid});
    }

    void TextEditorPanel::ReloadCurrentFile()
    {
        if (m_currentFile != nullptr)
        {
            m_openFiles.erase(m_currentFile);
            AddFile(m_currentFile);
        }

    }

} // namespace Lina::Editor