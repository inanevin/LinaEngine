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

#pragma once

#ifndef GUIParseUtility_HPP
#define GUIParseUtility_HPP

#include "Data/String.hpp"
#include "Core/Theme.hpp"
#include "Core/EditorRenderer.hpp"
#include "Platform/LinaVGIncl.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Lina::Editor
{
    class GNode;

    class GUIParseUtility
    {
    public:
     //   static GNode*                CreateFromTypeStr(const String& type);
     //   static GNode*                LoadFromFile(const String& file);
     //   static LinaVG::GradientType  GetGradientType(const String& name);
     //   static LinaVG::TextAlignment GetTextAlignment(const String& align);

    private:
    };
} // namespace Lina::Editor

namespace LinaVG
{
  // void to_json(json& j, const LinaVG::Vec4Grad& col)
  // {
  // }
  //
  // void from_json(const json& data, LinaVG::Vec4Grad& col)
  // {
  //     const Lina::String color1Str    = data.value("Start", "White").c_str();
  //     const Lina::String color2Str    = data.value("End", "").c_str();
  //     const Lina::String gradientType = data.value("GradientType", "Horizontal").c_str();
  //     Lina::Color        color1       = Lina::Editor::Theme::GetThemeColor(color1Str);
  //     Lina::Color        color2       = color2Str.compare("") == 0 ? color1 : Lina::Editor::Theme::GetThemeColor(color2Str);
  //     col.start                       = LV4(color1);
  //     col.end                         = LV4(color2);
  //     col.gradientType                = Lina::Editor::GUIParseUtility::GetGradientType(gradientType);
  //     col.radialSize                  = data.value("RadialSize", 0.0f);
  // }
  //
  // void to_json(json& j, const LinaVG::TextOptions& opts)
  // {
  // }
  //
  // void from_json(const json& data, LinaVG::TextOptions& opts)
  // {
  //     // Color
  //     json color = data.value("Color", json());
  //     opts.color = color.empty() ? LinaVG::Vec4Grad() : color.get<LinaVG::Vec4Grad>();
  //
  //     // Alignment
  //     const Lina::String alignment = data.value("Alignment", "").c_str();
  //     opts.alignment               = Lina::Editor::GUIParseUtility::GetTextAlignment(alignment);
  //
  //     // Drop shadow
  //     const Lina::String dropShadowColStr = data.value("DropShadowColor", "").c_str();
  //     json               dropShadowOff    = data.value("DropShadowOffset", json());
  //     opts.dropShadowColor                = LV4(Lina::Editor::Theme::GetThemeColor(dropShadowColStr));
  //     opts.dropShadowOffset               = dropShadowOff.empty() ? LinaVG::Vec2() : LinaVG::Vec2(dropShadowOff.value("X", 0.0f), dropShadowOff.value("Y", 0.0f));
  //
  //     // Font
  //     opts.font = Lina::Editor::Theme::GetThemeFont(data.value("Font", "Default").c_str());
  //
  //     // Others
  //     opts.newLineSpacing = data.value("NewLineSpacing", 0.0f);
  //     opts.spacing        = data.value("Spacing", 0.0f);
  //     opts.textScale      = data.value("Scale", 1.0f);
  //     opts.wrapWidth      = data.value("WrapWidth", 0.0f);
  // }
  //
  // void to_json(json& j, const LinaVG::SDFTextOptions& opts)
  // {
  // }
  //
  // void from_json(const json& data, LinaVG::SDFTextOptions& opts)
  // {
  //     // Color
  //     json color = data.value("Color", json());
  //     opts.color = color.empty() ? LinaVG::Vec4Grad() : color.get<LinaVG::Vec4Grad>();
  //
  //     // Alignment
  //     const Lina::String alignment = data.value("Alignment", "").c_str();
  //     opts.alignment               = Lina::Editor::GUIParseUtility::GetTextAlignment(alignment);
  //
  //     // Drop shadow
  //     const Lina::String dropShadowColStr = data.value("DropShadowColor", "").c_str();
  //     json               dropShadowOff    = data.value("DropShadowOffset", json());
  //     opts.dropShadowColor                = dropShadowColStr.compare("") == 0 ? LinaVG::Vec4() : LV4(Lina::Editor::Theme::GetThemeColor(dropShadowColStr));
  //     opts.dropShadowOffset               = dropShadowOff.empty() ? LinaVG::Vec2() : LinaVG::Vec2(dropShadowOff.value("X", 0.0f), dropShadowOff.value("Y", 0.0f));
  //
  //     // Font
  //     opts.font = Lina::Editor::Theme::GetThemeFont(data.value("Font", "Default").c_str());
  //
  //     // Others
  //     opts.newLineSpacing         = data.value("NewLineSpacing", 0.0f);
  //     opts.spacing                = data.value("Spacing", 0.0f);
  //     opts.textScale              = data.value("Scale", 1.0f);
  //     opts.wrapWidth              = data.value("WrapWidth", 0.0f);
  //     opts.flipAlpha              = data.value("FlipAlpha", false);
  //     opts.sdfThickness           = data.value("SDFThickness", 0.1f);
  //     opts.sdfSoftness            = data.value("SDFSoftness", 0.1f);
  //     opts.sdfOutlineThickness    = data.value("SDFOutlineThickness", 0.1f);
  //     opts.sdfDropShadowThickness = data.value("SDFDropShadowThickness", 0.1f);
  //     opts.sdfDropShadowSoftness  = data.value("SDFDropShadowSoftness", 0.1f);
  //
  //     const Lina::String outlineColStr = data.value("SDFOutlineColor", "").c_str();
  //     opts.sdfOutlineColor             = outlineColStr.compare("") == 0 ? LinaVG::Vec4() : LV4(Lina::Editor::Theme::GetThemeColor(outlineColStr));
  // }
  //
  // void to_json(json& j, const LinaVG::OutlineOptions& col)
  // {
  // }
  //
  // void from_json(const json& data, LinaVG::OutlineOptions& opts)
  // {
  //     // Color
  //     json color = data.value("Color", json());
  //     opts.color = color.empty() ? LinaVG::Vec4Grad() : color.get<LinaVG::Vec4Grad>();
  //
  //     // Draw direction
  //     const Lina::String drawDir = data.value("DrawDirection", "").c_str();
  //     if (drawDir.compare("Inwards"))
  //         opts.drawDirection = OutlineDrawDirection::Inwards;
  //     else if (drawDir.compare("Both"))
  //         opts.drawDirection = OutlineDrawDirection::Both;
  //     else
  //         opts.drawDirection = OutlineDrawDirection::Outwards;
  //
  //     // Base texture
  //     const Lina::String txtHandle = data.value("Texture", "").c_str();
  //     if (txtHandle.compare("") != 0)
  //         opts.textureHandle = Lina::FnvHash(txtHandle.c_str());
  //     json txtUVOffset     = data.value("TextureUVOffset", json());
  //     json txtUVTiling     = data.value("TextureUVTiling", json());
  //     opts.textureUVOffset = txtUVOffset.empty() ? LinaVG::Vec2() : LinaVG::Vec2(txtUVOffset.value("X", 0.0f), txtUVOffset.value("Y", 0.0f));
  //     opts.textureUVTiling = txtUVTiling.empty() ? LinaVG::Vec2() : LinaVG::Vec2(txtUVTiling.value("X", 0.0f), txtUVTiling.value("Y", 0.0f));
  //
  //     // Thickness
  //     opts.thickness = data.value("Thickness", 0.0f);
  // }
  //
  // void to_json(json& j, const LinaVG::StyleOptions& opts)
  // {
  // }
  //
  // void from_json(const json& data, LinaVG::StyleOptions& opts)
  // {
  //     json color = data.value("Color", json());
  //     opts.color = color.get<LinaVG::Vec4Grad>();
  //
  //     json outline        = data.value("Outline", json());
  //     opts.outlineOptions = outline.empty() ? LinaVG::OutlineOptions() : outline.get<LinaVG::OutlineOptions>();
  //
  //     // Rounding
  //     opts.rounding         = data.value("Rounding", 0.0f);
  //     json onlyRoundCorners = data.value("OnlyRoundCorners", json());
  //     for (auto& e : onlyRoundCorners)
  //         opts.onlyRoundTheseCorners.push_back(e.value("Val", 0));
  //
  //     // Base texture
  //     const Lina::String txtHandle = data.value("Texture", "").c_str();
  //     if (txtHandle.compare("") != 0)
  //         opts.textureHandle = Lina::FnvHash(txtHandle.c_str());
  //     json txtUVOffset = data.value("TextureUVOffset", json());
  //     json txtUVTiling = data.value("TextureUVTiling", json());
  //
  //     opts.textureUVOffset = txtUVOffset.empty() ? LinaVG::Vec2() : LinaVG::Vec2(txtUVOffset.value("X", 0.0f), txtUVOffset.value("Y", 0.0f));
  //     opts.textureUVTiling = txtUVTiling.empty() ? LinaVG::Vec2() : LinaVG::Vec2(txtUVTiling.value("X", 0.0f), txtUVTiling.value("Y", 0.0f));
  //
  //     // others
  //     opts.isFilled = data.value("IsFilled", true);
  // }
} // namespace LinaVG

#endif
