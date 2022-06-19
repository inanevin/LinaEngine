/*
Author: Inan Evin
www.inanevin.com

Copyright 2018 Inan Evin

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions
and limitations under the License.

Class: Example1Level
Timestamp: 5/6/2019 9:22:35 PM

*/

#pragma once

#ifndef Example1Level_HPP
#define Example1Level_HPP

#include "World/Level.hpp"

namespace Lina
{
    namespace Event
    {
        struct ETick;
    }
} // namespace Lina

class ExampleLevel : public Lina::World::Level
{
public:
    ExampleLevel() = default;

    virtual void Install() override;
};

#endif
