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

#include "HeaderTool.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <filesystem>

#define ROOT_PATH "../../"

// For testing inside this project.
// #define ROOT_PATH         "../../"
// #define REGISTRY_CPP_PATH "../../Sandbox/src/Lina/ReflectionRegistry.cpp"

int main(int argc, char** argv)
{
    Lina::HeaderTool tool;
    tool.Run(ROOT_PATH);
    return 0;
}

namespace Lina
{

#define LINA_CLASS_MACRO "LINA_CLASS("
#define LINA_FUNC_MACRO  "LINA_FUNC("
#define LINA_FIELD_MACRO "LINA_FIELD("

#define LINA_COMPONENT_MACRO         "LINA_COMPONENT("
#define LINA_PROPERTY_MACRO          "LINA_FIELD("
#define REGISTER_FUNC_BGN_IDENTIFIER "//REGFUNC_BEGIN"
#define REGISTER_FUNC_END_IDENTIFIER "//REGFUNC_END"
#define INCLUDE_BGN_IDENTIFIER       "//INC_BEGIN"
#define INCLUDE_END_IDENTIFIER       "//INC_END"

    std::vector<std::string> excludePaths{
        "_Dependencies", ".vs", ".git", "build", "CMake", "Docs", "vendor", "VSItem", ".clang", ".travis", "LICENSE", "README", ".sln",
    };

    std::vector<std::string> filesToSkip{"ReflectionSystem.hpp", "ClassReflection.hpp"};
    HeaderTool::~HeaderTool()
    {
        for (auto& p : m_componentData)
            delete p.second;
    }

    const std::string GClassIdentifierStr         = "class ";
    const std::string GClassIdentifierStr_nowhite = "class";

    const HashMap<MacroType, std::string> GMacroTypeToString = {{MacroType::LHT_MacroType_Class, LINA_CLASS_MACRO}, {MacroType::LHT_MacroType_Function, LINA_FUNC_MACRO}, {MacroType::LHT_MacroType_Field, LINA_FIELD_MACRO}};

    void HeaderTool::Run(const std::string& path)
    {
        // Scan each folder & sub-folders and find all .hpp files.
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().has_extension())
            {
                const std::string fullName  = entry.path().filename().string();
                const std::string extension = fullName.substr(fullName.find(".") + 1);

                if (extension.compare("cpp") == 0 || extension.compare("c") == 0)
                    continue;

                bool skip = false;
                for (auto f : filesToSkip)
                {
                    if (fullName.compare(f) == 0)
                    {
                        skip = true;
                        break;
                    }
                }

                if (skip)
                    continue;

                // Skip the property declaration file.
                if (extension.compare("hpp") == 0 || extension.compare("h") == 0)
                {
                    std::string replacedPath = entry.path().string();
                    std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
                    std::string include = replacedPath.substr(replacedPath.find("include"));
                    m_lastHPPInclude    = include.substr(include.find_first_of("/") + 1);
                    ReadHPP(entry.path().string());
                }
            }
            else
            {

                std::string replacedPath = entry.path().string();

                bool shouldExclude = false;
                for (auto& excludeStr : excludePaths)
                {
                    if (replacedPath.find(excludeStr) != std::string::npos || replacedPath.find("src") != std::string::npos)
                    {
                        shouldExclude = true;
                        break;
                    }
                }

                if (!shouldExclude)
                {
                    std::replace(replacedPath.begin(), replacedPath.end(), '\\', '/');
                    Run(replacedPath);
                }
            }
        }
    }

    size_t HeaderTool::CheckLineForMacro(const std::string& line, MacroType type)
    {
        size_t macroPos = line.find(GMacroTypeToString.at(type));

        if (macroPos != std::string::npos)
        {
            size_t commentPos = line.find("//");

            if (commentPos != std::string::npos && commentPos < macroPos)
                return std::string::npos;

            return macroPos;
        }

        return std::string::npos;
    }

    bool HeaderTool::ExtractClassProperties(const std::string& line)
    {
        const std::string lineNoWhitespace = RemoveWhitespacesExceptInQuotes(line);

        const size_t openingParanthesis = lineNoWhitespace.find("(");
        const size_t closingParanthesis = lineNoWhitespace.find(")");

        if (openingParanthesis == std::string::npos || closingParanthesis == std::string::npos)
            return false;

        const std::string parameters = lineNoWhitespace.substr(openingParanthesis + 1, closingParanthesis - openingParanthesis - 1);

        std::string param = "";
        for (char c : parameters)
        {
            if (c == ',')
            {
                RemoveDoubleQuote(param);
                m_currentlyParsedClass->parameters.push_back(param);
                param = "";
            }
            else
                param += c;
        }

        RemoveDoubleQuote(param);
        m_currentlyParsedClass->parameters.push_back(param);
        return true;
    }

    std::string HeaderTool::RemoveWhitespacesExceptInQuotes(const std::string& line)
    {
        std::string newStr;

        bool insideQuotes = false;
        for (char c : line)
        {
            if (c == '\"')
                insideQuotes = !insideQuotes;
            else if (!insideQuotes && c == ' ')
                continue;
            newStr += c;
        }

        return newStr;
    }

    void HeaderTool::ReadHPP(const std::string& hpp)
    {
        std::ifstream file;
        file.open(hpp);
        std::string line;

        m_lastClass = "";
        // Read the hpp line by line, find LINA_CLASS and LINA_PROPERTY macros.
        if (file.is_open())
        {
            while (file.good())
            {
                getline(file, line);

                const size_t macroClass = CheckLineForMacro(line.c_str(), MacroType::LHT_MacroType_Class);
                const size_t macroField = CheckLineForMacro(line.c_str(), MacroType::LHT_MacroType_Field);
                const size_t macroFunc  = CheckLineForMacro(line.c_str(), MacroType::LHT_MacroType_Function);

                // If we found a class macro.
                if (macroClass != std::string::npos)
                {
                    std::string nextLine = "";
                    getline(file, nextLine);
                    size_t classIdentifierPos = nextLine.find(GClassIdentifierStr);

                    // If the line below class macro is not valid, continue reading after.
                    if (classIdentifierPos == std::string::npos)
                        continue;
                    else
                    {
                        if (m_currentlyParsedClass != nullptr)
                        {
                            m_classes.push_back(m_currentlyParsedClass);
                            m_currentlyParsedClass = nullptr;
                        }
                        m_currentlyParsedClass = new LinaClassV2();

                        // Extract class properties, if something goes wrong proceed on next lines.
                        if (!ExtractClassProperties(line))
                        {
                            delete m_currentlyParsedClass;
                            continue;
                        }

                        // Extract the class name from next line.
                        RemoveWhitespaces(nextLine);
                        size_t       classIdentifierPos   = nextLine.find(GClassIdentifierStr_nowhite);
                        const size_t semiColon            = nextLine.find(":");
                        const size_t classNameStart       = classIdentifierPos + GClassIdentifierStr.size() - 1;
                        const size_t classNameEnd         = semiColon != std::string::npos ? semiColon : nextLine.size();
                        m_currentlyParsedClass->className = nextLine.substr(classNameStart, classNameEnd - classNameStart);
                    }
                }
                else if (macroField != std::string::npos || macroFunc != std::string::npos)
                {
                    // If we found other macros

                    if (m_currentlyParsedClass == nullptr)
                    {
                        // Property out of class, skip.
                        continue;
                    }

                    /************************************ CONTINUE HERE ********************************************/
                }

                if (nextLineIsComponent)
                {
                    std::string componentName = line.substr(0, line.find(":"));
                    RemoveWordFromLine(componentName, "class");
                    RemoveWordFromLine(componentName, "struct");
                    RemoveWhitespaces(componentName);

                    nextLineIsComponent                = false;
                    LinaComponent* linaComponent       = new LinaComponent();
                    *linaComponent                     = m_lastComponentData;
                    linaComponent->m_hppInclude        = m_lastHPPInclude;
                    linaComponent->m_nameWithNamespace = m_lastNamespace + "::" + componentName;
                    linaComponent->m_name              = componentName;
                    m_componentData[componentName]     = linaComponent;
                    m_namespaceComponentMap[m_lastNamespace].push_back(linaComponent);
                    m_lastClass              = componentName;
                    m_lastHeaderWasComponent = true;
                }
                else if (nextLineIsClass)
                {
                    std::string className = line.substr(0, line.find(":"));
                    RemoveWordFromLine(className, "class");
                    RemoveWordFromLine(className, "struct");
                    RemoveWhitespaces(className);

                    nextLineIsClass                = false;
                    LinaClass* linaClass           = new LinaClass();
                    *linaClass                     = m_lastClassData;
                    linaClass->m_hppInclude        = m_lastHPPInclude;
                    linaClass->m_nameWithNamespace = m_lastNamespace + "::" + className;
                    linaClass->m_name              = className;
                    m_classData[className]         = linaClass;
                    m_namespaceClassMap[m_lastNamespace].push_back(linaClass);
                    m_lastClass              = className;
                    m_lastHeaderWasComponent = false;
                }
                else if (nextLineIsProperty)
                {
                    // We have read the property macro, now we will read the property name
                    auto equals = line.find("=") != std::string::npos;

                    // If has equals sign, remove everything after the sign including the sign.
                    // If not, just remove the ;
                    if (equals)
                    {
                        auto equals = line.find("=");
                        line        = line.substr(0, equals);
                    }
                    else
                        line = line.substr(0, line.find(";"));

                    // Remove the last line until it's not a whitespace.
                    while (std::isspace(line.back()))
                        line = line.substr(0, line.length() - 1);

                    // Now the last character should be the last char of the variable's name
                    // Find the last whitespace, which should be the one right before the variable name
                    // and cut the string before that.
                    line = line.substr(line.find_last_of(' '));
                    RemoveWhitespaces(line);

                    m_lastProperty.m_propertyName = line;

                    if (m_lastHeaderWasComponent)
                        m_componentData[m_lastClass]->m_properties.push_back(m_lastProperty);
                    else
                        m_classData[m_lastClass]->m_properties.push_back(m_lastProperty);

                    nextLineIsProperty = false;
                }
                else
                {
                    if (line.find(LINA_COMPONENT_MACRO) != std::string::npos)
                    {
                        ProcessComponentMacro(line);
                        nextLineIsComponent = true;
                    }
                    else if (line.find(LINA_PROPERTY_MACRO) != std::string::npos && (m_lastClass.compare("") != 0))
                    {
                        ProcessPropertyMacro(line);
                        nextLineIsProperty = true;
                    }
                    else if (line.find(LINA_CLASS_MACRO) != std::string::npos)
                    {
                        ProcessClassMacro(line);
                        nextLineIsClass = true;
                    }
                    else if (line.find("namespace") != std::string::npos)
                    {
                        RemoveBrackets(line);
                        RemoveString(line, "namespace");
                        RemoveWhitespaces(line);

                        if (line.find("Lina::") != std::string::npos)
                            RemoveString(line, "Lina::");
                        m_lastNamespace = line;
                    }
                }
            }
        }
    }

    void HeaderTool::RemoveWordFromLine(std::string& line, const std::string& word)
    {
        auto n = line.find(word);
        if (n != std::string::npos)
        {
            line.erase(n, word.length());
        }

        const std::string t  = "\t";
        auto              n2 = line.find(t);
        if (n2 != std::string::npos)
            line.erase(n2, t.length());
    }

    void HeaderTool::ProcessPropertyMacro(const std::string& line)
    {
        std::string trimmed           = line.substr(line.find("(") + 1);
        std::string insideParanthesis = trimmed.substr(0, trimmed.find(")"));

        const int itemCount = 5;
        for (int i = 0; i < itemCount; i++)
        {
            const size_t firstQuote  = insideParanthesis.find("\"");
            const size_t secondQuote = insideParanthesis.find("\"", firstQuote + 1);
            std::string  prop        = insideParanthesis.substr(firstQuote + 1, secondQuote - 1 - firstQuote);
            RemoveWhitespacesPreAndPost(prop);
            insideParanthesis = insideParanthesis.substr(secondQuote + 1);

            if (i == 0)
                m_lastProperty.m_title = prop;
            else if (i == 1)
                m_lastProperty.m_type = prop;
            else if (i == 2)
                m_lastProperty.m_tooltip = prop;
            else if (i == 3)
                m_lastProperty.m_dependsOn = prop;
            else if (i == 4)
                m_lastProperty.m_category = prop;
        }
    }

    void HeaderTool::ProcessComponentMacro(const std::string& line)
    {
        std::string trimmed           = line.substr(line.find("(") + 1);
        std::string insideParanthesis = trimmed.substr(0, trimmed.find(")"));

        const int itemCount = 4;
        for (int i = 0; i < itemCount; i++)
        {
            const size_t firstQuote  = insideParanthesis.find("\"");
            const size_t secondQuote = insideParanthesis.find("\"", firstQuote + 1);
            std::string  prop        = insideParanthesis.substr(firstQuote + 1, secondQuote - 1 - firstQuote);
            RemoveWhitespacesPreAndPost(prop);
            insideParanthesis = insideParanthesis.substr(secondQuote + 1);

            if (i == 0)
                m_lastComponentData.m_title = prop;
            else if (i == 1)
                m_lastComponentData.m_category = prop;
            else if (i == 2)
                m_lastComponentData.m_isAbstract = prop;
            else if (i == 3)
                m_lastComponentData.m_chunkSize = prop;
        }
    }

    void HeaderTool::ProcessClassMacro(const std::string& line)
    {
        std::string trimmed           = line.substr(line.find("(") + 1);
        std::string insideParanthesis = trimmed.substr(0, trimmed.find(")"));

        const int itemCount = 1;
        for (int i = 0; i < itemCount; i++)
        {
            const size_t firstQuote  = insideParanthesis.find("\"");
            const size_t secondQuote = insideParanthesis.find("\"", firstQuote + 1);
            std::string  property    = insideParanthesis.substr(firstQuote + 1, secondQuote - 1 - firstQuote);
            RemoveWhitespacesPreAndPost(property);
            insideParanthesis = insideParanthesis.substr(secondQuote + 1);

            if (i == 0)
                m_lastClassData.m_title = property;
        }
    }

    void HeaderTool::RemoveWhitespaces(std::string& str)
    {
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
        str.erase(end_pos, str.end());
    }

    void HeaderTool::RemoveWhitespacesPreAndPost(std::string& str)
    {
        const size_t firstChar = str.find_first_not_of(' ');
        const size_t lastChar  = str.find_last_not_of(' ');

        if (str.size() > lastChar)
            str = str.substr(firstChar, lastChar + 1);
    }

    void HeaderTool::RemoveComma(std::string& str)
    {
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), ',');
        str.erase(end_pos, str.end());
    }

    void HeaderTool::RemoveDoubleQuote(std::string& str)
    {
        std::string::iterator end_pos = std::remove(str.begin(), str.end(), '"');
        str.erase(end_pos, str.end());
    }

    void HeaderTool::RemoveString(std::string& str, const std::string& toErase)
    {
        size_t pos = str.find(toErase);
        str.erase(pos, toErase.length());
    }

    void HeaderTool::RemoveBrackets(std::string& str)
    {
        if (str.find("{") != std::string::npos)
        {
            std::string::iterator end_pos = std::remove(str.begin(), str.end(), '{');
            str.erase(end_pos, str.end());
        }

        if (str.find("}") != std::string::npos)
        {
            std::string::iterator end_pos = std::remove(str.begin(), str.end(), '}');
            str.erase(end_pos, str.end());
        }
    }
    void HeaderTool::SerializeReadData()
    {

        std::ifstream file;
        std::string   line;
        // file.open(REGISTRY_CPP_PATH);
        bool                     registerFunctionFound = false;
        bool                     includeFound          = false;
        std::vector<std::string> fileContents;

        if (file.is_open())
        {
            while (file.good())
            {
                getline(file, line);

                if (registerFunctionFound)
                {
                    if (line.find(REGISTER_FUNC_END_IDENTIFIER) == std::string::npos)
                        continue;
                    else
                    {
                        registerFunctionFound = false;
                        fileContents.push_back(line);
                    }
                }
                else if (includeFound)
                {
                    if (line.find(INCLUDE_END_IDENTIFIER) == std::string::npos)
                        continue;
                    else
                    {
                        includeFound = false;
                        fileContents.push_back(line);
                    }
                }
                else
                    fileContents.push_back(line);

                if (line.find(INCLUDE_BGN_IDENTIFIER) != std::string::npos)
                {
                    includeFound = true;

                    for (auto& [actualName, compData] : m_componentData)
                        fileContents.push_back("#include \"" + compData->m_hppInclude + "\"");

                    for (auto& [actualName, classData] : m_classData)
                        fileContents.push_back("#include \"" + classData->m_hppInclude + "\"");
                }
                else if (line.find(REGISTER_FUNC_BGN_IDENTIFIER) != std::string::npos)
                {
                    registerFunctionFound = true;
                    for (auto& [actualName, componentData] : m_componentData)
                    {
                        const std::string        className = componentData->m_nameWithNamespace;
                        std::vector<std::string> functionCommands;

                        fileContents.push_back("Reflection::Meta<" + className + ">().AddProperty(\"Title\"_hs,\"" + componentData->m_title + "\");");
                        fileContents.push_back("Reflection::Meta<" + className + ">().AddProperty(\"Category\"_hs,\"" + componentData->m_category + "\");");

                        if (componentData->m_chunkSize.compare("") == 0)
                            fileContents.push_back("Reflection::Meta<" + className + ">().AddProperty(\"MemChunkSize\"_hs," + "TO_STRING(DEFAULT_COMP_CHUNK_SIZE)" + ");");
                        else
                            fileContents.push_back("Reflection::Meta<" + className + ">().AddProperty(\"MemChunkSize\"_hs,\"" + componentData->m_chunkSize + "\");");

                        for (auto& property : componentData->m_properties)
                        {
                            fileContents.push_back("Reflection::Meta<" + className + ">().AddField<&" + className + "::" + property.m_propertyName + ", " + className + ">(\"" + property.m_propertyName + "\"_hs);");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Title\"_hs,\"" + property.m_title + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Type\"_hs,\"" + property.m_type + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Tooltip\"_hs,\"" + property.m_tooltip + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Depends On\"_hs,\"" + property.m_dependsOn + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Category\"_hs,\"" + property.m_category + "\");");
                        }

                        if (componentData->m_isAbstract.compare("True") != 0 && componentData->m_isAbstract.compare("true") != 0)
                        {
                            functionCommands.push_back("Reflection::Meta<" + className + ">().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<" + className + ">);");
                            functionCommands.push_back("Reflection::Meta<" + className + ">().createFunc = std::bind(&REF_CreateComponentFunc<" + className + ">);");
                            functionCommands.push_back("Reflection::Meta<" + className + ">().destroyFunc = std::bind(&REF_DestroyComponentFunc<" + className + ">, std::placeholders::_1);");
                        }

                        for (auto& fc : functionCommands)
                            fileContents.push_back(fc);
                    }

                    for (auto& [actualName, classData] : m_classData)
                    {
                        const std::string className = classData->m_nameWithNamespace;
                        fileContents.push_back("Reflection::Meta<" + className + ">().AddProperty(\"Title\"_hs,\"" + classData->m_title + "\");");

                        for (auto& property : classData->m_properties)
                        {
                            fileContents.push_back("Reflection::Meta<" + className + ">().AddField<&" + className + "::" + property.m_propertyName + ", " + className + ">(\"" + property.m_propertyName + "\"_hs);");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Title\"_hs,\"" + property.m_title + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Type\"_hs,\"" + property.m_type + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Tooltip\"_hs,\"" + property.m_tooltip + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Depends On\"_hs,\"" + property.m_dependsOn + "\");");
                            fileContents.push_back("Reflection::Meta<" + className + ">().GetField(\"" + property.m_propertyName + "\"_hs)->AddProperty(\"Category\"_hs,\"" + property.m_category + "\");");
                        }
                    }
                }
            }
        }

        file.close();

        std::ofstream newFile;
        //    newFile.open(REGISTRY_CPP_PATH, std::ofstream::out | std::ofstream::trunc);

        for (int i = 0; i < fileContents.size() - 1; i++)
            newFile << fileContents[i] << std::endl;
        newFile.close();
    }

} // namespace Lina
