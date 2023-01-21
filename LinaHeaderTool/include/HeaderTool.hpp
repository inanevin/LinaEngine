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

/*
Class: HeaderTool



Timestamp: 12/14/2021 2:03:29 PM
*/

#pragma once

#ifndef HeaderTool_HPP
#define HeaderTool_HPP
#include "Data/HashMap.hpp"
#include "Data/Vector.hpp"
#include <string>
#include <unordered_map>

namespace Lina
{
    struct LinaProperty
    {
        std::string m_title        = "";
        std::string m_type         = "";
        std::string m_tooltip      = "";
        std::string m_dependsOn    = "";
        std::string m_category     = "";
        std::string m_propertyName = "";
    };

    struct LinaComponent
    {
        std::string               m_hppInclude        = "";
        std::string               m_name              = "";
        std::string               m_nameWithNamespace = "";
        std::string               m_title             = "";
        std::string               m_category          = "";
        std::string               m_isAbstract        = "";
        std::string               m_chunkSize         = "";
        std::vector<LinaProperty> m_properties;
    };

    struct LinaClass
    {
        std::string               m_hppInclude        = "";
        std::string               m_name              = "";
        std::string               m_nameWithNamespace = "";
        std::string               m_title             = "";
        std::vector<LinaProperty> m_properties;
    };

    struct LinaClassV2
    {
        std::string         className = "";
        Vector<std::string> parameters;
    };

    enum MacroType
    {
        LHT_MacroType_Class = 0,
        LHT_MacroType_Field,
        LHT_MacroType_Function
    };

    class HeaderTool
    {
    public:
        HeaderTool() = default;
        ~HeaderTool();

        size_t      CheckLineForMacro(const std::string& line, MacroType type);
        bool        ExtractClassProperties(const std::string& line);
        std::string RemoveWhitespacesExceptInQuotes(const std::string& line);

        void Run(const std::string& path);
        void ReadHPP(const std::string& hpp);
        void RemoveWordFromLine(std::string& line, const std::string& word);
        void ProcessPropertyMacro(const std::string& line);
        void ProcessComponentMacro(const std::string& line);
        void ProcessClassMacro(const std::string& line);
        void RemoveWhitespaces(std::string& str);
        void RemoveWhitespacesPreAndPost(std::string& str);
        void RemoveComma(std::string& str);
        void RemoveDoubleQuote(std::string& str);
        void RemoveString(std::string& str, const std::string& toErase);
        void RemoveBrackets(std::string& str);
        void SerializeReadData();

    private:
        Vector<LinaClassV2*> m_classes;
        LinaClassV2*         m_currentlyParsedClass = nullptr;

        std::unordered_map<std::string, LinaComponent*>              m_componentData;
        std::unordered_map<std::string, LinaClass*>                  m_classData;
        std::unordered_map<std::string, std::vector<LinaComponent*>> m_namespaceComponentMap;
        std::unordered_map<std::string, std::vector<LinaClass*>>     m_namespaceClassMap;
        std::string                                                  m_lastClass      = "";
        std::string                                                  m_lastNamespace  = "";
        std::string                                                  m_lastHPPInclude = "";
        LinaProperty                                                 m_lastProperty;
        LinaComponent                                                m_lastComponentData;
        LinaClass                                                    m_lastClassData;
        bool                                                         nextLineIsComponent      = false;
        bool                                                         nextLineIsClass          = false;
        bool                                                         nextLineIsProperty       = false;
        bool                                                         m_lastHeaderWasComponent = false;
    };
} // namespace Lina

#endif
