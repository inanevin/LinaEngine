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

#include "Drawers/TextureDrawer.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Rendering/Texture.hpp"
#include "Log/Log.hpp"
#include "Core/InputEngine.hpp"
#include "Resources/ResourceStorage.hpp"
#include "Core/EditorCommon.hpp"
#include "EventSystem/ResourceEvents.hpp"
#include "EventSystem/EventSystem.hpp"
#include "Core/GUILayer.hpp"

namespace Lina::Editor
{

    const char* pixelFormats[]{
        "R",
        "RG",
        "RGB",
        "RGBA",
        "RGB16F",
        "RGBA16F",
        "DEPTH",
        "DEPTH_AND_STENCIL",
        "SRGB",
        "SRGBA"};

    const char* samplerFilters[]{
        "Nearest",
        "Linear",
        "Nearest_Mipmap_Nearest",
        "Linear_Mipmap_Nearest",
        "Nearest_Mipmap_Linear",
        "Linear_Mipmap_Linear"};

    const char* wrapModes[]{
        "Clamp_Edge",
        "Clamp_Mirror",
        "Clamp_Border",
        "Repeat",
        "Repeat_Mirror"};

    void TextureDrawer::DrawTextureSettings(Graphics::Texture*& texture, float paneWidth)
    {
        auto*             assetData                  = texture->GetAssetData();
        const int         currentInternalPixelFormat = static_cast<int>(assetData->m_samplerParameters.m_textureParams.m_internalPixelFormat);
        const int         currentPixelFormat         = static_cast<int>(assetData->m_samplerParameters.m_textureParams.m_pixelFormat);
        const int         currentMin                 = Graphics::SamplerFilterToInt(assetData->m_samplerParameters.m_textureParams.m_minFilter);
        const int         currentMag                 = Graphics::SamplerFilterToInt(assetData->m_samplerParameters.m_textureParams.m_magFilter);
        const int         wrapS                      = Graphics::WrapModeToInt(assetData->m_samplerParameters.m_textureParams.m_wrapS);
        const int         wrapR                      = Graphics::WrapModeToInt(assetData->m_samplerParameters.m_textureParams.m_wrapR);
        const int         wrapT                      = Graphics::WrapModeToInt(assetData->m_samplerParameters.m_textureParams.m_wrapT);
        const String textureSize                = texture->GetSize().ToString();
        const String channelCount               = TO_STRING(texture->GetNumComponents());

        WidgetsUtility::IncrementCursorPosY(ImGui::GetStyle().ItemSpacing.y + 9.5f);

        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        ImGui::Text("Texture Size");

        ImGui::SameLine();
        ImGui::SetCursorPosX(CURSOR_X_VALUES);
        ImGui::Text(textureSize.c_str());

        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        ImGui::Text("Channels");
        ImGui::SameLine();
        ImGui::SetCursorPosX(CURSOR_X_VALUES);
        ImGui::Text(channelCount.c_str());

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_PopupBg));
        WidgetsUtility::HorizontalDivider(0.0f, 1.0f);
        WidgetsUtility::IncrementCursorPosY(12);

        ImGui::PopStyleColor();

        WidgetsUtility::PropertyLabel("Anisotropy");
        ImGui::DragInt("##anisotropy", &assetData->m_samplerParameters.m_anisotropy, 0.05f, 0, 8);

        WidgetsUtility::PropertyLabel("Mipmaps");
        ImGui::Checkbox("##mips", &assetData->m_samplerParameters.m_textureParams.m_generateMipMaps);

        WidgetsUtility::PropertyLabel("Internal Format", true, "OpenGL internal texture format.");
        if (WidgetsUtility::BeginComboBox("##internal_pixel", pixelFormats[currentInternalPixelFormat]))
        {

            for (int n = 0; n < IM_ARRAYSIZE(pixelFormats); n++)
            {
                const bool is_selected = (currentInternalPixelFormat == n);
                if (ImGui::Selectable(pixelFormats[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_internalPixelFormat = static_cast<Graphics::PixelFormat>(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        WidgetsUtility::PropertyLabel("Format", true, "OpenGL texture format.");
        if (WidgetsUtility::BeginComboBox("##def_pixel", pixelFormats[currentPixelFormat]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(pixelFormats); n++)
            {
                const bool is_selected = (currentPixelFormat == n);
                if (ImGui::Selectable(pixelFormats[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_pixelFormat = static_cast<Graphics::PixelFormat>(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        WidgetsUtility::PropertyLabel("Min", true, "Sampler filter to use when the texture is minified.");
        if (WidgetsUtility::BeginComboBox("##min_filter", samplerFilters[currentMin]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(samplerFilters); n++)
            {
                const bool is_selected = (currentMin == n);
                if (ImGui::Selectable(samplerFilters[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_minFilter = Graphics::SamplerFilterFromInt(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        WidgetsUtility::PropertyLabel("Mag", true, "Sampler filter to use when the texture is magnified.");
        if (WidgetsUtility::BeginComboBox("##mag_filter", samplerFilters[currentMag]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(samplerFilters); n++)
            {
                const bool is_selected = (currentMag == n);
                if (ImGui::Selectable(samplerFilters[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_magFilter = Graphics::SamplerFilterFromInt(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        WidgetsUtility::PropertyLabel("Wrap S");
        if (WidgetsUtility::BeginComboBox("##wrap_s", wrapModes[wrapS]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
            {
                const bool is_selected = (wrapS == n);
                if (ImGui::Selectable(wrapModes[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_wrapS = Graphics::WrapModeFromInt(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        WidgetsUtility::PropertyLabel("Wrap R");
        if (WidgetsUtility::BeginComboBox("##wrap_r", wrapModes[wrapR]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
            {
                const bool is_selected = (wrapR == n);
                if (ImGui::Selectable(wrapModes[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_wrapR = Graphics::WrapModeFromInt(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        WidgetsUtility::PropertyLabel("Wrap T");
        if (WidgetsUtility::BeginComboBox("##wrap_t", wrapModes[wrapT]))
        {
            for (int n = 0; n < IM_ARRAYSIZE(wrapModes); n++)
            {
                const bool is_selected = (wrapT == n);
                if (ImGui::Selectable(wrapModes[n], is_selected))
                    assetData->m_samplerParameters.m_textureParams.m_wrapT = Graphics::WrapModeFromInt(n);

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SetCursorPosX(CURSOR_X_LABELS);
        if (WidgetsUtility::Button("Save Settings", ImVec2(paneWidth - CURSOR_X_LABELS * 2, 25 * GUILayer::Get()->GetDPIScale())))
        {
            Resources::SaveArchiveToFile(assetData->GetPath(), *assetData);
            const String  path = texture->GetPath();
            const StringIDType sid  = texture->GetSID();
            const TypeID       tid  = GetTypeID<Graphics::Texture>();
            Resources::ResourceStorage::Get()->Unload<Graphics::Texture>(sid);
            Event::EventSystem::Get()->Trigger<Event::ERequestResourceReload>(Event::ERequestResourceReload{path, tid, sid});
            texture = Resources::ResourceStorage::Get()->GetResource<Graphics::Texture>(sid);
        }
    }

    void TextureDrawer::DrawTexture(Graphics::Texture* texture, const Vector2& bgMin, const Vector2& bgMax)
    {
#pragma warning(disable : 4312)

        const Vector2  bgSize      = bgMax - bgMin;
        ImVec2         drawSize    = ImVec2(0.0f, 0.0f);
        const Vector2i textureSize = texture->GetSize();

        const float aspect         = (float)textureSize.x / (float)textureSize.y;
        const float drawPadding    = 5;
        bool        widthIsBigger  = textureSize.x > bgSize.x;
        bool        heightIsBigger = textureSize.y > bgSize.y;

        if (widthIsBigger && heightIsBigger)
        {
            const float xDiff = (float)textureSize.x - bgSize.x;
            const float yDiff = (float)textureSize.y - bgSize.y;

            if (xDiff > yDiff)
                heightIsBigger = false;
            else
                widthIsBigger = false;
        }

        if (widthIsBigger)
        {
            drawSize.x = bgSize.x - drawPadding;
            drawSize.y = drawSize.x / aspect;
        }
        else if (heightIsBigger)
        {
            drawSize.y = bgSize.y - drawPadding;
            drawSize.x = aspect * drawSize.x;
        }
        else
            drawSize = ImVec2((float)textureSize.x, (float)textureSize.y);

        const ImVec2 imageMin = ImVec2(bgMin.x + bgSize.x / 2.0f - drawSize.x / 2.0f, bgMin.y + bgSize.y / 2.0f - drawSize.y / 2.0f);
        const ImVec2 imageMax = ImVec2(imageMin.x + drawSize.x, imageMin.y + drawSize.y);
        ImGui::GetWindowDrawList()->AddImage((void*)texture->GetID(), imageMin, imageMax, ImVec2(0, 1), ImVec2(1, 0));
    }
} // namespace Lina::Editor