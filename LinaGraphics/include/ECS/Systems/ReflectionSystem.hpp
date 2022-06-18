/* 
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

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
Class: ReflectionSystem



Timestamp: 1/23/2022 2:11:17 AM
*/

#pragma once

#ifndef ReflectionSystem_HPP
#define ReflectionSystem_HPP

// Headers here.
#include "ECS/System.hpp"
#include "Math/Vector.hpp"
#include "Core/RenderEngineFwd.hpp"
#include "Core/CommonApplication.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Material;
        class Texture;
    } // namespace Graphics

    namespace Event
    {
        struct EPlayModeChanged;
    }
} // namespace Lina

namespace Lina::ECS
{
    class ReflectionSystem : public System
    {

    public:
        ReflectionSystem()  = default;
        ~ReflectionSystem() = default;
        virtual void Initialize(const std::string& name, ApplicationMode& appMode);
        virtual void UpdateComponents(float delta){UpdateReflectionData();};

        /// <summary>
        /// Goes through the reflection areas & captures their cubemaps if they are dynamic.
        /// </summary>
        void UpdateReflectionData();

        /// <summary>
        /// Sets the reflection area map on a material.
        /// </summary>
        void SetReflectionsOnMaterial(Graphics::Material* mat, const Vector3& transformLocation);

    private:
        void ConstructRefAreaTexture(Graphics::Texture* texture, const Vector2i& res);
        void OnPlayModeChanged(const Event::EPlayModeChanged& ev);

    private:
        Graphics::RenderEngine* m_renderEngine = nullptr;
        ApplicationMode         m_appMode;
    };
} // namespace Lina::ECS

#endif
