/*
Class: EditorCommon

General purpose macros for LinaEditor namespace.

Timestamp: 5/8/2020 11:04:45 PM

*/
#pragma once

#ifndef EditorCommon_HPP
#define EditorCommon_HPP

#include "Data/String.hpp"

namespace Lina::Editor
{
    struct EShortcut
    {
        String name       = "";
        int    heldKey    = 0;
        int    pressedKey = 0;
    };

    enum class EditorPanel
    {
        Entities,
        Level,
        Properties,
        Resources,
        Global,
    };

} // namespace Lina::Editor

#endif
