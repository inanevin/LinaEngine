/*
Class: CommonInput



Timestamp: 12/25/2021 12:23:31 PM
*/

#pragma once

#ifndef CommonInput_HPP
#define CommonInput_HPP

// Headers here.

namespace Lina::Input
{
    enum class InputAction
    {
        Pressed  = 0,
        Released = 1,
        Repeated = 2
    };

    // Cursor Modes
    enum class CursorMode
    {
        Visible  = 1 << 0,
        Hidden   = 1 << 1,
        Disabled = 1 << 2
    };
} // namespace Lina::Input

#endif
