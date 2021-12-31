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
Class: CommonECS



Timestamp: 12/25/2021 12:39:49 PM
*/

#pragma once

#ifndef CommonECS_HPP
#define CommonECS_HPP

// Headers here.
#define ENTT_USE_ATOMIC
#include <cereal/archives/portable_binary.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
namespace Lina::ECS
{
#define ECSNULL entt::null

    typedef entt::entity                                                                      Entity;
    typedef entt::delegate<void(entt::snapshot&, cereal::PortableBinaryOutputArchive&)>       ComponentSerializeFunction;
    typedef entt::delegate<void(entt::snapshot_loader&, cereal::PortableBinaryInputArchive&)> ComponentDeserializeFunction;

} // namespace Lina::ECS

#endif
