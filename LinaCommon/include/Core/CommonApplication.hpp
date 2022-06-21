/*
Class: CommonApplication



Timestamp: 12/25/2021 12:21:22 PM
*/

#pragma once

#ifndef CommonApplication_HPP
#define CommonApplication_HPP

// Headers here.
#include "Core/CommonWindow.hpp"

namespace Lina
{
    enum class LogLevel
    {
        None     = 0,
        Debug    = 1,
        Info     = 2,
        Critical = 3,
        Error    = 4,
        Trace    = 5,
        Warn     = 6,
    };

    enum class ApplicationMode
    {
        Editor     = 1 << 0,
        Standalone = 1 << 1
    };

    struct ApplicationInfo
    {
        // Bundle name that is used to load resources package on Standalone builds.
        String m_bundleName = "";

        // App Info
        const char*    m_appName     = "Lina Engine";
        const wchar_t* m_packagePass = nullptr;
        int            m_build       = 0;
        int            m_appMajor    = 1;
        int            m_appMinor    = 0;
        int            m_appPatch    = 0;

        ApplicationMode  m_appMode          = ApplicationMode::Editor;
        WindowProperties m_windowProperties = WindowProperties();
    };

    extern String LogLevelAsString(LogLevel level);
} // namespace Lina

#endif
