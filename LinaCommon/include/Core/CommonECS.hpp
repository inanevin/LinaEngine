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

    typedef entt::entity Entity;

} // namespace Lina::ECS

#endif
