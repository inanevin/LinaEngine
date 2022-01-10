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

#include "World/DefaultLevel.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/LightComponent.hpp"

namespace Lina::World
{
    void DefaultLevel::Install()
    {
        auto* mat                = Graphics::RenderEngineBackend::Get()->GetDefaultSkyboxMaterial();
        m_skyboxMaterial.m_value = mat;
        m_skyboxMaterial.m_sid   = mat->GetSID();
        m_ambientColor           = Color(0.8f, 0.8f, 0.8f, 1.0f);

        Level::Install();
        
        // Default Plane
        auto* plane = Resources::ResourceStorage::Get()->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Plane.fbx");
        Graphics::RenderEngineBackend::Get()->GetModelNodeSystem()->CreateModelHierarchy(plane);

        // Directional Light
        ECS::Entity directionalLight = m_registry.CreateEntity("Directional Light");
        auto&       data             = m_registry.get<ECS::EntityDataComponent>(directionalLight);
        auto& light = m_registry.emplace<ECS::DirectionalLightComponent>(directionalLight);
        data.SetLocation(Vector3(50, 15, 0));
        light.m_color = Color(255, 255, 240, 255, true);
        light.m_intensity = 1.0f;
    }

} // namespace Lina::World
