/*
Class: ResourceEvents



Timestamp: 12/25/2021 12:18:35 PM
*/

#pragma once

#ifndef ResourceEvents_HPP
#define ResourceEvents_HPP

// Headers here.
#include "Utility/StringId.hpp"

namespace Lina::Event
{
    struct EResourceLoadCompleted
    {
        TypeID       m_tid;
        StringIDType m_sid;
    };

    struct EAllResourcesOfTypeLoaded
    {
        TypeID m_tid;
    };

    struct EResourceProgressStarted
    {
    };
    struct EResourceProgressEnded
    {
    };
    struct EResourcePathUpdated
    {
        StringIDType m_previousStringID;
        StringIDType m_newStringID;
        String m_oldPath;
        String m_newPath;
    };
    struct EResourceProgressUpdated
    {
        String m_currentResource;
        float       m_percentage;
    };
    struct EResourceUnloaded
    {
        StringIDType m_sid;
        TypeID m_tid;
    };
    struct ERequestResourceReload
    {
        String  m_fullPath;
        TypeID       m_tid;
        StringIDType m_sid;
    };
    struct EResourceReloaded
    {
        TypeID       m_tid;
        StringIDType m_sid;
    };

} // namespace Lina::Event

#endif
