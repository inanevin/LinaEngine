/*
Class: Animation



Timestamp: 12/7/2021 11:34:48 AM
*/

#pragma once

#ifndef Animation_HPP
#define Animation_HPP

// Headers here.
#include <Data/String.hpp>

namespace Lina::Graphics
{
    class Animation
    {

    public:
        Animation() = default;
        ~Animation() = default;

        bool LoadAnimation(const String& animPath);

    private:
    };
} // namespace Lina::Graphics

#endif
