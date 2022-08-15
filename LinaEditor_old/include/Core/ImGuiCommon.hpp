/*
Class: ImGuiCommon



Timestamp: 1/2/2022 5:03:25 PM
*/

#pragma once

#ifndef ImGuiCommon_HPP
#define ImGuiCommon_HPP

// Headers here.
#include "imgui/imgui.h"
#include <Data/String.hpp>

namespace Lina::Editor
{
    struct InputTextCallback_UserData
    {
        String*           Str;
        ImGuiInputTextCallback ChainCallback;
        void*                  ChainCallbackUserData;
    };

    extern int InputTextCallback(ImGuiInputTextCallbackData* data);
}

#endif
