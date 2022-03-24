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

#include "Core/ReflectionRegistry.hpp"
#include "Core/CommonECS.hpp"
#include "ECS/Registry.hpp"
#include "Log/Log.hpp"
#include "Utility/StringId.hpp"
#include <entt/entt.hpp>
#include <entt/meta/resolve.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <entt/meta/node.hpp>
#include <cereal/archives/portable_binary.hpp>
#include "IconsFontAwesome5.h"

/// <summary>
/// *****************************************************************************
/// *
/// *   THIS IS AN AUTOMATICALLY UPDATED FILE, DO NOT CHANGE THE IDENTIFIER LINES
/// *   BEFORE EACH BUILD, LINA HEADER TOOL PROCESSES ALL THE HEADERS WITHIN THE PROJECT
/// *   AND FILES THE REFLECTED COMPONENTS & PROPERTIES, WHICH ARE THEN ADDED TO THIS FILE
/// *   AUTOMATICALLY.
/// *
/// *****************************************************************************
/// </summary>

#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/PhysicsComponent.hpp"

//INC_BEGIN - !! DO NOT MODIFY THIS LINE !!
#include "ECS/Components/PhysicsComponent.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/ModelNodeComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/ReflectionAreaComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
#include "Rendering/RenderSettings.hpp"
#include "Rendering/Material.hpp"
#include "Core/EditorSettings.hpp"
#include "Rendering/ModelAssetData.hpp"
#include "Core/EngineSettings.hpp"
#include "World/Level.hpp"
//INC_END - !! DO NOT MODIFY THIS LINE !!

namespace Lina
{
    std::pair<ECS::Entity, TypeID> m_copyBuffer;
    using namespace entt::literals;

    template <typename Type>
    void REF_CloneComponent(ECS::Entity from, ECS::Entity to)
    {
        Type component = ECS::Registry::Get()->template get<Type>(from);
        ECS::Registry::Get()->template emplace<Type>(to, component);
    }

    template <typename Type>
    void REF_SerializeComponent(entt::snapshot& snapshot, cereal::PortableBinaryOutputArchive& archive)
    {
        snapshot.component<Type>(archive);
    }

    template <typename Type>
    void REF_DeserializeComponent(entt::snapshot_loader& loader, cereal::PortableBinaryInputArchive& archive)
    {
        loader.component<Type>(archive);
    }

    template <typename Type>
    void REF_SetEnabled(ECS::Entity ent, bool enabled)
    {
        ECS::Registry::Get()->template get<Type>(ent).SetIsEnabled(enabled);
    }

    template <typename Type>
    Type& REF_Get(ECS::Entity entity)
    {
        return ECS::Registry::Get()->template get<Type>(entity);
    }

    template <typename Type>
    void REF_Add(ECS::Entity entity)
    {
        ECS::Registry::Get()->template emplace<Type>(entity);
    }

    template <typename Type>
    bool REF_Has(ECS::Entity entity)
    {
        return ECS::Registry::Get()->all_of<Type>(entity);
    }

    template <typename Type>
    void REF_Reset(ECS::Entity entity)
    {
        TypeID tid = GetTypeID<Type>();

        if (tid == GetTypeID<ECS::EntityDataComponent>())
        {
            ECS::EntityDataComponent& comp = ECS::Registry::Get()->get<ECS::EntityDataComponent>(entity);
            comp.SetLocalLocation(Vector3::Zero);
            comp.SetLocalRotation(Quaternion());
            comp.SetLocalScale(Vector3::One);
        }
        else
            ECS::Registry::Get()->template replace<Type>(entity, Type());
    }

    template <typename Type>
    void REF_Remove(ECS::Entity entity)
    {
        ECS::Registry::Get()->template remove<Type>(entity);
    }

    template <typename Type>
    void REF_Copy(ECS::Entity entity, TypeID tid)
    {
        m_copyBuffer.first  = entity;
        m_copyBuffer.second = tid;
    }

    template <typename Type>
    void REF_Paste(ECS::Entity entity)
    {
        TypeID pastedTid = GetTypeID<Type>();

        if (pastedTid == m_copyBuffer.second)
        {
            if (m_copyBuffer.first != entt::null)
            {
                if (pastedTid == GetTypeID<ECS::EntityDataComponent>())
                {
                    ECS::EntityDataComponent& copy = ECS::Registry::Get()->get<ECS::EntityDataComponent>(m_copyBuffer.first);

                    auto& data = ECS::Registry::Get()->get<ECS::EntityDataComponent>(entity);
                    data.SetLocation(copy.GetLocation());
                    data.SetRotation(copy.GetRotation());
                    data.SetScale(copy.GetScale());
                }
                else
                {
                    Type* copy = ECS::Registry::Get()->try_get<Type>(m_copyBuffer.first);

                    if (copy != nullptr)
                    {
                        ECS::Registry::Get()->replace<Type>(entity, *copy);
                        REF_SetEnabled<Type>(entity, copy->GetIsEnabled());
                    }
                }
            }
        }
    }

    template <typename Type>
    void REF_ValueChanged(ECS::Entity ent, const char* propertyName)
    {
    }

    void ReflectionRegistry::RegisterReflectedComponents()
    {
        //REGFUNC_BEGIN - !! DO NOT CHANGE THIS LINE !!
entt::meta<ECS::PhysicsComponent>().type().props(std::make_pair("Title"_hs, "Physics Component"), std::make_pair("Icon"_hs,ICON_FA_APPLE_ALT), std::make_pair("Category"_hs,""), std::make_pair("CanAddComponent"_hs, "0"));
entt::meta<ECS::PhysicsComponent>().data<&ECS::PhysicsComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_CloneComponent<ECS::PhysicsComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_SerializeComponent<ECS::PhysicsComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_DeserializeComponent<ECS::PhysicsComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_SetEnabled<ECS::PhysicsComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Get<ECS::PhysicsComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Reset<ECS::PhysicsComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Has<ECS::PhysicsComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Remove<ECS::PhysicsComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Copy<ECS::PhysicsComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Paste<ECS::PhysicsComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::EntityDataComponent>().type().props(std::make_pair("Title"_hs, "Entity Data Component"), std::make_pair("Icon"_hs,ICON_FA_DATABASE), std::make_pair("Category"_hs,""), std::make_pair("CanAddComponent"_hs, "0"));
entt::meta<ECS::EntityDataComponent>().data<&ECS::EntityDataComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_CloneComponent<ECS::EntityDataComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_SerializeComponent<ECS::EntityDataComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_DeserializeComponent<ECS::EntityDataComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_SetEnabled<ECS::EntityDataComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Get<ECS::EntityDataComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Reset<ECS::EntityDataComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Has<ECS::EntityDataComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Remove<ECS::EntityDataComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Copy<ECS::EntityDataComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Paste<ECS::EntityDataComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::ModelNodeComponent>().type().props(std::make_pair("Title"_hs, "Model Node"), std::make_pair("Icon"_hs,ICON_FA_CUBES), std::make_pair("Category"_hs,"Rendering"), std::make_pair("CanAddComponent"_hs, "0"));
entt::meta<ECS::ModelNodeComponent>().data<&ECS::ModelNodeComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::ModelNodeComponent>().data<&ECS::ModelNodeComponent::m_materialsNames>("m_materialsNames"_hs).props(std::make_pair("Title"_hs,"Materials_Names"),std::make_pair("Type"_hs,""),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ModelNodeComponent>().data<&ECS::ModelNodeComponent::m_materials>("m_materials"_hs).props(std::make_pair("Title"_hs,"Materials"),std::make_pair("Type"_hs,"MaterialArray"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ModelNodeComponent>().func<&REF_CloneComponent<ECS::ModelNodeComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_SerializeComponent<ECS::ModelNodeComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_DeserializeComponent<ECS::ModelNodeComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_SetEnabled<ECS::ModelNodeComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_Get<ECS::ModelNodeComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_Reset<ECS::ModelNodeComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_Has<ECS::ModelNodeComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_Remove<ECS::ModelNodeComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_Copy<ECS::ModelNodeComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::ModelNodeComponent>().func<&REF_Paste<ECS::ModelNodeComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::CameraComponent>().type().props(std::make_pair("Title"_hs, "Camera Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Rendering"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_isActive>("m_isActive"_hs).props(std::make_pair("Title"_hs,"Is Active"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_zFar>("m_zFar"_hs).props(std::make_pair("Title"_hs,"Far"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"Maximum distance the camera renders at."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_zNear>("m_zNear"_hs).props(std::make_pair("Title"_hs,"Near"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"Minimum distance the camera renders at."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_fieldOfView>("m_fieldOfView"_hs).props(std::make_pair("Title"_hs,"Field of View"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_clearColor>("m_clearColor"_hs).props(std::make_pair("Title"_hs,"Clear Color"),std::make_pair("Type"_hs,"Color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::CameraComponent>().func<&REF_CloneComponent<ECS::CameraComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_SerializeComponent<ECS::CameraComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_DeserializeComponent<ECS::CameraComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_SetEnabled<ECS::CameraComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Get<ECS::CameraComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Reset<ECS::CameraComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Has<ECS::CameraComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Remove<ECS::CameraComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Copy<ECS::CameraComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Paste<ECS::CameraComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Add<ECS::CameraComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::SpotLightComponent>().type().props(std::make_pair("Title"_hs, "Spot Light Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Lights"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_outerCutoff>("m_outerCutoff"_hs).props(std::make_pair("Title"_hs,"Outer Cutoff"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_cutoff>("m_cutoff"_hs).props(std::make_pair("Title"_hs,"Cutoff"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_distance>("m_distance"_hs).props(std::make_pair("Title"_hs,"Distance"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"Light Distance"),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_castsShadows>("m_castsShadows"_hs).props(std::make_pair("Title"_hs,"Cast Shadows"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables dynamic shadow casting for this light."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_drawDebug>("m_drawDebug"_hs).props(std::make_pair("Title"_hs,"Draw Debug"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables debug drawing for this component."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_intensity>("m_intensity"_hs).props(std::make_pair("Title"_hs,"Intensity"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_color>("m_color"_hs).props(std::make_pair("Title"_hs,"Color"),std::make_pair("Type"_hs,"Color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::SpotLightComponent>().func<&REF_CloneComponent<ECS::SpotLightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_SerializeComponent<ECS::SpotLightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_DeserializeComponent<ECS::SpotLightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_SetEnabled<ECS::SpotLightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Get<ECS::SpotLightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Reset<ECS::SpotLightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Has<ECS::SpotLightComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Remove<ECS::SpotLightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Copy<ECS::SpotLightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Paste<ECS::SpotLightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Add<ECS::SpotLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::LightComponent>().type().props(std::make_pair("Title"_hs, "Light Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Lights"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_castsShadows>("m_castsShadows"_hs).props(std::make_pair("Title"_hs,"Cast Shadows"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables dynamic shadow casting for this light."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_drawDebug>("m_drawDebug"_hs).props(std::make_pair("Title"_hs,"Draw Debug"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables debug drawing for this component."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_intensity>("m_intensity"_hs).props(std::make_pair("Title"_hs,"Intensity"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_color>("m_color"_hs).props(std::make_pair("Title"_hs,"Color"),std::make_pair("Type"_hs,"Color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::LightComponent>().func<&REF_CloneComponent<ECS::LightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::LightComponent>().func<&REF_SerializeComponent<ECS::LightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::LightComponent>().func<&REF_DeserializeComponent<ECS::LightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::LightComponent>().func<&REF_SetEnabled<ECS::LightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Get<ECS::LightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Reset<ECS::LightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Has<ECS::LightComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Remove<ECS::LightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Copy<ECS::LightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Paste<ECS::LightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Add<ECS::LightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::PointLightComponent>().type().props(std::make_pair("Title"_hs, "Point Light Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Lights"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_shadowFar>("m_shadowFar"_hs).props(std::make_pair("Title"_hs,"Shadow Far"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_castsShadows"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_shadowNear>("m_shadowNear"_hs).props(std::make_pair("Title"_hs,"Shadow Near"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_castsShadows"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_bias>("m_bias"_hs).props(std::make_pair("Title"_hs,"Bias"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"Defines the shadow crispiness."),std::make_pair("Depends"_hs,"m_castsShadows"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_distance>("m_distance"_hs).props(std::make_pair("Title"_hs,"Distance"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,"Light Distance"),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_castsShadows>("m_castsShadows"_hs).props(std::make_pair("Title"_hs,"Cast Shadows"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables dynamic shadow casting for this light."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_drawDebug>("m_drawDebug"_hs).props(std::make_pair("Title"_hs,"Draw Debug"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables debug drawing for this component."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_intensity>("m_intensity"_hs).props(std::make_pair("Title"_hs,"Intensity"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_color>("m_color"_hs).props(std::make_pair("Title"_hs,"Color"),std::make_pair("Type"_hs,"Color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::PointLightComponent>().func<&REF_CloneComponent<ECS::PointLightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_SerializeComponent<ECS::PointLightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_DeserializeComponent<ECS::PointLightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_SetEnabled<ECS::PointLightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Get<ECS::PointLightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Reset<ECS::PointLightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Has<ECS::PointLightComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Remove<ECS::PointLightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Copy<ECS::PointLightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Paste<ECS::PointLightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Add<ECS::PointLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::DirectionalLightComponent>().type().props(std::make_pair("Title"_hs, "Directional Light Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Lights"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_shadowZFar>("m_shadowZFar"_hs).props(std::make_pair("Title"_hs,"Shadow Far"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_castsShadows"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_shadowZNear>("m_shadowZNear"_hs).props(std::make_pair("Title"_hs,"Shadow Near"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_castsShadows"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_shadowOrthoProjection>("m_shadowOrthoProjection"_hs).props(std::make_pair("Title"_hs,"Projection"),std::make_pair("Type"_hs,"Vector4"),std::make_pair("Tooltip"_hs,"Defines shadow projection boundaries."),std::make_pair("Depends"_hs,"m_castsShadows"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_castsShadows>("m_castsShadows"_hs).props(std::make_pair("Title"_hs,"Cast Shadows"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables dynamic shadow casting for this light."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_drawDebug>("m_drawDebug"_hs).props(std::make_pair("Title"_hs,"Draw Debug"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables debug drawing for this component."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_intensity>("m_intensity"_hs).props(std::make_pair("Title"_hs,"Intensity"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_color>("m_color"_hs).props(std::make_pair("Title"_hs,"Color"),std::make_pair("Type"_hs,"Color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::DirectionalLightComponent>().func<&REF_CloneComponent<ECS::DirectionalLightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_SerializeComponent<ECS::DirectionalLightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_DeserializeComponent<ECS::DirectionalLightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_SetEnabled<ECS::DirectionalLightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Get<ECS::DirectionalLightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Reset<ECS::DirectionalLightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Has<ECS::DirectionalLightComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Remove<ECS::DirectionalLightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Copy<ECS::DirectionalLightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Paste<ECS::DirectionalLightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Add<ECS::DirectionalLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::ReflectionAreaComponent>().type().props(std::make_pair("Title"_hs, "Reflection Area"), std::make_pair("Icon"_hs,ICON_FA_MOUNTAIN), std::make_pair("Category"_hs,"Rendering"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::ReflectionAreaComponent>().data<&ECS::ReflectionAreaComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::ReflectionAreaComponent>().data<&ECS::ReflectionAreaComponent::m_drawDebug>("m_drawDebug"_hs).props(std::make_pair("Title"_hs,"Draw Debug"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enables debug drawing for this component."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ReflectionAreaComponent>().data<&ECS::ReflectionAreaComponent::m_resolution>("m_resolution"_hs).props(std::make_pair("Title"_hs,"Resolution"),std::make_pair("Type"_hs,"Vector2i"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ReflectionAreaComponent>().data<&ECS::ReflectionAreaComponent::m_halfExtents>("m_halfExtents"_hs).props(std::make_pair("Title"_hs,"Half Extents"),std::make_pair("Type"_hs,"Vector3"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_isLocal"_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ReflectionAreaComponent>().data<&ECS::ReflectionAreaComponent::m_isDynamic>("m_isDynamic"_hs).props(std::make_pair("Title"_hs,"Is Dynamic"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"If true, the reflection data will be captured every frame for this area."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ReflectionAreaComponent>().data<&ECS::ReflectionAreaComponent::m_isLocal>("m_isLocal"_hs).props(std::make_pair("Title"_hs,"Is Local"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"If false, any object not affected by a local area will be affected by this one."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_CloneComponent<ECS::ReflectionAreaComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_SerializeComponent<ECS::ReflectionAreaComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_DeserializeComponent<ECS::ReflectionAreaComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_SetEnabled<ECS::ReflectionAreaComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Get<ECS::ReflectionAreaComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Reset<ECS::ReflectionAreaComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Has<ECS::ReflectionAreaComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Remove<ECS::ReflectionAreaComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Copy<ECS::ReflectionAreaComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Paste<ECS::ReflectionAreaComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::ReflectionAreaComponent>().func<&REF_Add<ECS::ReflectionAreaComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::SpriteRendererComponent>().type().props(std::make_pair("Title"_hs, "Sprite Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Rendering"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::SpriteRendererComponent>().data<&ECS::SpriteRendererComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_CloneComponent<ECS::SpriteRendererComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_SerializeComponent<ECS::SpriteRendererComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_DeserializeComponent<ECS::SpriteRendererComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_SetEnabled<ECS::SpriteRendererComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Get<ECS::SpriteRendererComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Reset<ECS::SpriteRendererComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Has<ECS::SpriteRendererComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Remove<ECS::SpriteRendererComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Copy<ECS::SpriteRendererComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Paste<ECS::SpriteRendererComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Add<ECS::SpriteRendererComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::FreeLookComponent>().type().props(std::make_pair("Title"_hs, "Free Look Component"), std::make_pair("Icon"_hs,ICON_FA_EYE), std::make_pair("Category"_hs,"Input"), std::make_pair("CanAddComponent"_hs, "1"));
entt::meta<ECS::FreeLookComponent>().data<&ECS::FreeLookComponent::m_isEnabled>("m_isEnabled"_hs);
entt::meta<ECS::FreeLookComponent>().data<&ECS::FreeLookComponent::m_rotationSpeeds>("m_rotationSpeeds"_hs).props(std::make_pair("Title"_hs,"Rotation Speed"),std::make_pair("Type"_hs,"Vector2"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::FreeLookComponent>().data<&ECS::FreeLookComponent::m_movementSpeeds>("m_movementSpeeds"_hs).props(std::make_pair("Title"_hs,"Movement Speed"),std::make_pair("Type"_hs,"Vector2"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<ECS::FreeLookComponent>().func<&REF_CloneComponent<ECS::FreeLookComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_SerializeComponent<ECS::FreeLookComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_DeserializeComponent<ECS::FreeLookComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_SetEnabled<ECS::FreeLookComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Get<ECS::FreeLookComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Reset<ECS::FreeLookComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Has<ECS::FreeLookComponent>, entt::as_is_t>("has"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Remove<ECS::FreeLookComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Copy<ECS::FreeLookComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Paste<ECS::FreeLookComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Add<ECS::FreeLookComponent>, entt::as_void_t>("add"_hs);
entt::meta<Graphics::RenderSettings>().type().props(std::make_pair("Title"_hs, "Render Settings"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_vignettePow>("m_vignettePow"_hs).props(std::make_pair("Title"_hs,"Pow"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_vignetteEnabled"_hs), std::make_pair("Category"_hs, "Vignette"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_vignetteAmount>("m_vignetteAmount"_hs).props(std::make_pair("Title"_hs,"Amount"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_vignetteEnabled"_hs), std::make_pair("Category"_hs, "Vignette"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_vignetteEnabled>("m_vignetteEnabled"_hs).props(std::make_pair("Title"_hs,"Vignette"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Vignette"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_fxaaSpanMax>("m_fxaaSpanMax"_hs).props(std::make_pair("Title"_hs,"Reduce Span"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_fxaaEnabled"_hs), std::make_pair("Category"_hs, "Anti-Aliasing"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_fxaaReduceMul>("m_fxaaReduceMul"_hs).props(std::make_pair("Title"_hs,"Reduce Mul"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_fxaaEnabled"_hs), std::make_pair("Category"_hs, "Anti-Aliasing"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_fxaaReduceMin>("m_fxaaReduceMin"_hs).props(std::make_pair("Title"_hs,"Reduce Min"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,"m_fxaaEnabled"_hs), std::make_pair("Category"_hs, "Anti-Aliasing"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_fxaaEnabled>("m_fxaaEnabled"_hs).props(std::make_pair("Title"_hs,"FXAA"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"Enable fast approximate anti-aliasing."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Anti-Aliasing"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_bloomEnabled>("m_bloomEnabled"_hs).props(std::make_pair("Title"_hs,"Bloom"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Bloom"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_exposure>("m_exposure"_hs).props(std::make_pair("Title"_hs,"Exposure"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Tonemapping"));
entt::meta<Graphics::RenderSettings>().data<&Graphics::RenderSettings::m_gamma>("m_gamma"_hs).props(std::make_pair("Title"_hs,"Gamma"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Tonemapping"));
entt::meta<Graphics::Material>().type().props(std::make_pair("Title"_hs, "Material"));
entt::meta<Graphics::Material>().data<&Graphics::Material::m_shaderHandle>("m_shaderHandle"_hs).props(std::make_pair("Title"_hs,"Shader"),std::make_pair("Type"_hs,"Shader"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Editor::	EditorSettings>().type().props(std::make_pair("Title"_hs, "Editor"));
entt::meta<Editor::	EditorSettings>().data<&Editor::	EditorSettings::m_textEditorPath>("m_textEditorPath"_hs).props(std::make_pair("Title"_hs,"Text Editor"),std::make_pair("Type"_hs,"StringPath"),std::make_pair("Tooltip"_hs,"Default text editor to open shader & similar files."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Graphics::ModelAssetData>().type().props(std::make_pair("Title"_hs, "Model Data"));
entt::meta<Graphics::ModelAssetData>().data<&Graphics::ModelAssetData::m_generatePivots>("m_generatePivots"_hs).props(std::make_pair("Title"_hs,"Generate Entity Pivots"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,"If true, any entity generated via adding this model to the scene will have offset pivots as parents."),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Graphics::ModelAssetData>().data<&Graphics::ModelAssetData::m_flipUVs>("m_flipUVs"_hs).props(std::make_pair("Title"_hs,"Flip UVs"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Graphics::ModelAssetData>().data<&Graphics::ModelAssetData::m_flipWinding>("m_flipWinding"_hs).props(std::make_pair("Title"_hs,"Flip Winding"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Graphics::ModelAssetData>().data<&Graphics::ModelAssetData::m_calculateTangentSpace>("m_calculateTangentSpace"_hs).props(std::make_pair("Title"_hs,"Calc Tangents"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Graphics::ModelAssetData>().data<&Graphics::ModelAssetData::m_smoothNormals>("m_smoothNormals"_hs).props(std::make_pair("Title"_hs,"Smooth Normals"),std::make_pair("Type"_hs,"Bool"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<Graphics::ModelAssetData>().data<&Graphics::ModelAssetData::m_globalScale>("m_globalScale"_hs).props(std::make_pair("Title"_hs,"Global Scale"),std::make_pair("Type"_hs,"Float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, ""));
entt::meta<EngineSettings>().type().props(std::make_pair("Title"_hs, "Engine Settings"));
entt::meta<World::Level>().type().props(std::make_pair("Title"_hs, "Level Settings"));
entt::meta<World::Level>().data<&World::Level::m_ambientColor>("m_ambientColor"_hs).props(std::make_pair("Title"_hs,"Ambient"),std::make_pair("Type"_hs,"Color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Sky"));
entt::meta<World::Level>().data<&World::Level::m_skyboxMaterial>("m_skyboxMaterial"_hs).props(std::make_pair("Title"_hs,"Skybox"),std::make_pair("Type"_hs,"Material"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""_hs), std::make_pair("Category"_hs, "Sky"));
        //REGFUNC_END - !! DO NOT CHANGE THIS LINE !!
    }
} // namespace Lina






































































































































































































































































































