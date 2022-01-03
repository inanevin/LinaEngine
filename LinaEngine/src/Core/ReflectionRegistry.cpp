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
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/LightComponent.hpp"
#include "ECS/Components/FreeLookComponent.hpp"
//INC_END - !! DO NOT MODIFY THIS LINE !!

namespace Lina
{
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
    }

    template <typename Type>
    void REF_Paste(ECS::Entity entity)
    {
    }

    template <typename Type>
    void REF_ValueChanged(ECS::Entity ent, const char* propertyName)
    {
    }

    void ReflectionRegistry::RegisterReflectedComponents()
    {
        //REGFUNC_BEGIN - !! DO NOT CHANGE THIS LINE !!
entt::meta<ECS::PhysicsComponent>().type().props(std::make_pair("Title"_hs, "PhysicsComponent"), std::make_pair("Icon"_hs,""), std::make_pair("Category"_hs,""));
entt::meta<ECS::PhysicsComponent>().func<&REF_CloneComponent<ECS::PhysicsComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_SerializeComponent<ECS::PhysicsComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_DeserializeComponent<ECS::PhysicsComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_SetEnabled<ECS::PhysicsComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Get<ECS::PhysicsComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Reset<ECS::PhysicsComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Has<ECS::PhysicsComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Remove<ECS::PhysicsComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Copy<ECS::PhysicsComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::PhysicsComponent>().func<&REF_Paste<ECS::PhysicsComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::EntityDataComponent>().type().props(std::make_pair("Title"_hs, "EntityDataComponent"), std::make_pair("Icon"_hs,""), std::make_pair("Category"_hs,""));
entt::meta<ECS::EntityDataComponent>().func<&REF_CloneComponent<ECS::EntityDataComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_SerializeComponent<ECS::EntityDataComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_DeserializeComponent<ECS::EntityDataComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_SetEnabled<ECS::EntityDataComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Get<ECS::EntityDataComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Reset<ECS::EntityDataComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Has<ECS::EntityDataComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Remove<ECS::EntityDataComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Copy<ECS::EntityDataComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::EntityDataComponent>().func<&REF_Paste<ECS::EntityDataComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::CameraComponent>().type().props(std::make_pair("Title"_hs, "CameraComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Rendering"));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_clearColor>("m_clearColor"_hs).props(std::make_pair("Title"_hs,"ClearColor"),std::make_pair("Type"_hs,"color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_fieldOfView>("m_fieldOfView"_hs).props(std::make_pair("Title"_hs,"FieldofView"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_zNear>("m_zNear"_hs).props(std::make_pair("Title"_hs,"Near"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"Minimumdistancethecamerarendersat."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::CameraComponent>().data<&ECS::CameraComponent::m_zFar>("m_zFar"_hs).props(std::make_pair("Title"_hs,"Far"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"Maximumdistancethecamerarendersat."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::CameraComponent>().func<&REF_CloneComponent<ECS::CameraComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_SerializeComponent<ECS::CameraComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_DeserializeComponent<ECS::CameraComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_SetEnabled<ECS::CameraComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Get<ECS::CameraComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Reset<ECS::CameraComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Has<ECS::CameraComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Remove<ECS::CameraComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Copy<ECS::CameraComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Paste<ECS::CameraComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_Add<ECS::CameraComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::CameraComponent>().func<&REF_ValueChanged<ECS::CameraComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::SpotLightComponent>().type().props(std::make_pair("Title"_hs, "SpotLightComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Lights"));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_distance>("m_distance"_hs).props(std::make_pair("Title"_hs,"Distance"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"LightDistance"),std::make_pair("Depends"_hs,""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_cutoff>("m_cutoff"_hs).props(std::make_pair("Title"_hs,"Cutoff"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"ThelightwillgraduallydimfromtheedgesoftheconedefinedbytheCutofftotheconedefinedbytheOuterCutoff."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::SpotLightComponent>().data<&ECS::SpotLightComponent::m_outerCutoff>("m_outerCutoff"_hs).props(std::make_pair("Title"_hs,"OuterCutoff"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"ThelightwillgraduallydimfromtheedgesoftheconedefinedbytheCutofftotheconedefinedbytheOuterCutoff."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::SpotLightComponent>().func<&REF_CloneComponent<ECS::SpotLightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_SerializeComponent<ECS::SpotLightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_DeserializeComponent<ECS::SpotLightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_SetEnabled<ECS::SpotLightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Get<ECS::SpotLightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Reset<ECS::SpotLightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Has<ECS::SpotLightComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Remove<ECS::SpotLightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Copy<ECS::SpotLightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Paste<ECS::SpotLightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_Add<ECS::SpotLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::SpotLightComponent>().func<&REF_ValueChanged<ECS::SpotLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::LightComponent>().type().props(std::make_pair("Title"_hs, "LightComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Lights"));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_color>("m_color"_hs).props(std::make_pair("Title"_hs,"Color"),std::make_pair("Type"_hs,"color"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_intensity>("m_intensity"_hs).props(std::make_pair("Title"_hs,"Intensity"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_drawDebug>("m_drawDebug"_hs).props(std::make_pair("Title"_hs,"DrawDebug"),std::make_pair("Type"_hs,"bool"),std::make_pair("Tooltip"_hs,"Enablesdebugdrawingforthiscomponent."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::LightComponent>().data<&ECS::LightComponent::m_castsShadows>("m_castsShadows"_hs).props(std::make_pair("Title"_hs,"CastShadows"),std::make_pair("Type"_hs,"bool"),std::make_pair("Tooltip"_hs,"Enablesdynamicshadowcastingforthislight."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::LightComponent>().func<&REF_CloneComponent<ECS::LightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::LightComponent>().func<&REF_SerializeComponent<ECS::LightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::LightComponent>().func<&REF_DeserializeComponent<ECS::LightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::LightComponent>().func<&REF_SetEnabled<ECS::LightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Get<ECS::LightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Reset<ECS::LightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Has<ECS::LightComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Remove<ECS::LightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Copy<ECS::LightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Paste<ECS::LightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::LightComponent>().func<&REF_Add<ECS::LightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::LightComponent>().func<&REF_ValueChanged<ECS::LightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::SpriteRendererComponent>().type().props(std::make_pair("Title"_hs, "SpriteComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Rendering"));
entt::meta<ECS::SpriteRendererComponent>().func<&REF_CloneComponent<ECS::SpriteRendererComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_SerializeComponent<ECS::SpriteRendererComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_DeserializeComponent<ECS::SpriteRendererComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_SetEnabled<ECS::SpriteRendererComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Get<ECS::SpriteRendererComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Reset<ECS::SpriteRendererComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Has<ECS::SpriteRendererComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Remove<ECS::SpriteRendererComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Copy<ECS::SpriteRendererComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Paste<ECS::SpriteRendererComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_Add<ECS::SpriteRendererComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::SpriteRendererComponent>().func<&REF_ValueChanged<ECS::SpriteRendererComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::PointLightComponent>().type().props(std::make_pair("Title"_hs, "PointLightComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Lights"));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_distance>("m_distance"_hs).props(std::make_pair("Title"_hs,"Distance"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"LightDistance"),std::make_pair("Depends"_hs,""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_bias>("m_bias"_hs).props(std::make_pair("Title"_hs,"Bias"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,"Definestheshadowcrispiness."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_shadowNear>("m_shadowNear"_hs).props(std::make_pair("Title"_hs,"ShadowNear"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::PointLightComponent>().data<&ECS::PointLightComponent::m_shadowFar>("m_shadowFar"_hs).props(std::make_pair("Title"_hs,"ShadowFar"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::PointLightComponent>().func<&REF_CloneComponent<ECS::PointLightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_SerializeComponent<ECS::PointLightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_DeserializeComponent<ECS::PointLightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_SetEnabled<ECS::PointLightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Get<ECS::PointLightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Reset<ECS::PointLightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Has<ECS::PointLightComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Remove<ECS::PointLightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Copy<ECS::PointLightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Paste<ECS::PointLightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_Add<ECS::PointLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::PointLightComponent>().func<&REF_ValueChanged<ECS::PointLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::DirectionalLightComponent>().type().props(std::make_pair("Title"_hs, "DirectionalLightComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Lights"));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_shadowOrthoProjection>("m_shadowOrthoProjection"_hs).props(std::make_pair("Title"_hs,"Projection"),std::make_pair("Type"_hs,"vector4"),std::make_pair("Tooltip"_hs,"Definesshadowprojectionboundaries."),std::make_pair("Depends"_hs,""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_shadowZNear>("m_shadowZNear"_hs).props(std::make_pair("Title"_hs,"ShadowNear"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::DirectionalLightComponent>().data<&ECS::DirectionalLightComponent::m_shadowZFar>("m_shadowZFar"_hs).props(std::make_pair("Title"_hs,"ShadowFar"),std::make_pair("Type"_hs,"float"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::DirectionalLightComponent>().func<&REF_CloneComponent<ECS::DirectionalLightComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_SerializeComponent<ECS::DirectionalLightComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_DeserializeComponent<ECS::DirectionalLightComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_SetEnabled<ECS::DirectionalLightComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Get<ECS::DirectionalLightComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Reset<ECS::DirectionalLightComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Has<ECS::DirectionalLightComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Remove<ECS::DirectionalLightComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Copy<ECS::DirectionalLightComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Paste<ECS::DirectionalLightComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_Add<ECS::DirectionalLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::DirectionalLightComponent>().func<&REF_ValueChanged<ECS::DirectionalLightComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::FreeLookComponent>().type().props(std::make_pair("Title"_hs, "FreeLookComponent"), std::make_pair("Icon"_hs,"ICON_FA_EYE"), std::make_pair("Category"_hs,"Input"));
entt::meta<ECS::FreeLookComponent>().data<&ECS::FreeLookComponent::m_movementSpeeds>("m_movementSpeeds"_hs).props(std::make_pair("Title"_hs,"MovementSpeed"),std::make_pair("Type"_hs,"vector2"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::FreeLookComponent>().data<&ECS::FreeLookComponent::m_rotationSpeeds>("m_rotationSpeeds"_hs).props(std::make_pair("Title"_hs,"RotationSpeed"),std::make_pair("Type"_hs,"vector2"),std::make_pair("Tooltip"_hs,""),std::make_pair("Depends"_hs,""));
entt::meta<ECS::FreeLookComponent>().func<&REF_CloneComponent<ECS::FreeLookComponent>, entt::as_void_t>("clone"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_SerializeComponent<ECS::FreeLookComponent>, entt::as_void_t>("serialize"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_DeserializeComponent<ECS::FreeLookComponent>, entt::as_void_t>("deserialize"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_SetEnabled<ECS::FreeLookComponent>, entt::as_void_t>("setEnabled"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Get<ECS::FreeLookComponent>, entt::as_ref_t>("get"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Reset<ECS::FreeLookComponent>, entt::as_void_t>("reset"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Has<ECS::FreeLookComponent>, entt::as_void_t>("has"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Remove<ECS::FreeLookComponent>, entt::as_void_t>("remove"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Copy<ECS::FreeLookComponent>, entt::as_void_t>("copy"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Paste<ECS::FreeLookComponent>, entt::as_void_t>("paste"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_Add<ECS::FreeLookComponent>, entt::as_void_t>("add"_hs);
entt::meta<ECS::FreeLookComponent>().func<&REF_ValueChanged<ECS::FreeLookComponent>, entt::as_void_t>("add"_hs);
        //REGFUNC_END - !! DO NOT CHANGE THIS LINE !!
    }
} // namespace Lina






















