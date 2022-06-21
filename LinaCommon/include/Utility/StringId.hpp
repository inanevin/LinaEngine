/*
Class: StringId


Timestamp: 12/19/2020 3:11:17 AM
*/

#pragma once

#ifndef StringId_HPP
#define StringId_HPP

// Headers here.
#include <entt/entt.hpp>

namespace Lina
{
    typedef entt::hashed_string::hash_type StringIDType;
    typedef entt::hashed_string            StringID;
    typedef entt::id_type                  TypeID;

#define SID_EMPTY StringID("")

    template <typename T>
    TypeID GetTypeID()
    {
        return entt::type_hash<T>::value();
    }
} // namespace Lina

#endif
