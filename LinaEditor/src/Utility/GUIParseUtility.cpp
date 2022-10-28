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

#include "Utility/GUIParseUtility.hpp"
#include "GUI/GNode.hpp"
#include "GUI/GShape.hpp"
#include "GUI/GImage.hpp"
#include "GUI/GText.hpp"

namespace Lina::Editor
{
    //
    //    GNode* GUIParseUtility::CreateFromTypeStr(const String& type)
    //    {
    //        if (type.compare("GShape") == 0)
    //            return new GShape("");
    //        else if (type.compare("GImage") == 0)
    //            return new GImage("");
    //        else if (type.compare("GText") == 0)
    //            return new GText("");
    //
    //        return new GNode();
    //    }
    //
    //    GNode* GUIParseUtility::LoadFromFile(const String& file)
    //    {
    //        bool debugUseAbs = true;
    //
    //        String finalPath = debugUseAbs ? "C:\\Projects\\LinaEngine\\_Resources\\" + file : file;
    //
    //        GNode* node = nullptr;
    //        try
    //        {
    //            std::ifstream f(finalPath.c_str());
    //            json          data = json::parse(f);
    //            const String  type = data.value("Node", "").c_str();
    //            node               = CreateFromTypeStr(type);
    //            node->ParseFromJSON(data);
    //        }
    //        catch (std::exception& ex)
    //        {
    //            LINA_ERR("[GUI Parser] -> Can't parse JSON!");
    //        }
    //
    //        return node;
    //    }
    //
    //    LinaVG::GradientType GUIParseUtility::GetGradientType(const String& name)
    //    {
    //        if (name.compare("Horizontal") == 0)
    //            return LinaVG::GradientType::Horizontal;
    //        else if (name.compare("Vertical") == 0)
    //            return LinaVG::GradientType::Vertical;
    //        else if (name.compare("Radial") == 0)
    //            return LinaVG::GradientType::Radial;
    //
    //        return LinaVG::GradientType::RadialCorner;
    //    }
    //    LinaVG::TextAlignment GUIParseUtility::GetTextAlignment(const String& align)
    //    {
    //        if (align.compare("Right") == 0)
    //            return LinaVG::TextAlignment::Right;
    //        else if (align.compare("Center") == 0)
    //            return LinaVG::TextAlignment::Center;
    //
    //        return LinaVG::TextAlignment::Left;
    //    }
} // namespace Lina::Editor
