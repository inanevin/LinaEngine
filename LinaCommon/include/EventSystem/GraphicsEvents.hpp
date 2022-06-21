/*
Class: GraphicsEvents



Timestamp: 12/25/2021 12:18:16 PM
*/

#pragma once

#ifndef GraphicsEvents_HPP
#define GraphicsEvents_HPP

// Headers here.
#include "Math/Color.hpp"
#include "Math/Quaternion.hpp"

namespace Lina::Event
{
    struct EPreRender
    {
    };
    struct EPostSceneDraw
    {
    };
    struct EPostRender
    {
    };
    struct EGUIRender
    {
    };
    struct ECustomRender
    {
    };
    struct EDrawLine
    {
        Vector3 m_from;
        Vector3 m_to;
        Color   m_color;
        float   m_lineWidth;
    };
    struct EDrawBox
    {
        Vector3 m_position;
        Vector3 m_halfExtents;
        Color   m_color;
        float   m_lineWidth;
    };
    struct EDrawSphere
    {
        Vector3 m_position;
        float   m_radius;
        Color   m_color;
        float   m_lineWidth;
    };
    struct EDrawHemiSphere
    {
        Vector3 m_position;
        float   m_radius;
        Color   m_color;
        float   m_lineWidth;
        bool    m_top;
    };
    struct EDrawCapsule
    {
        Vector3 m_position;
        float   m_radius;
        Color   m_color;
        float   m_height;
        float   m_lineWidth;
    };
    struct EDrawCircle
    {
        Vector3    m_position;
        float      m_radius;
        Color      m_color;
        float      m_lineWidth;
        bool       m_half;
        Quaternion m_rotation;
    };
} // namespace Lina::Event

#endif
