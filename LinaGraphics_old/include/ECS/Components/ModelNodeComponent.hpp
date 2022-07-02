/*
Class: ModelNodeComponent



Timestamp: 12/24/2021 10:20:14 PM
*/

#pragma once

#ifndef ModelNodeComponent_HPP
#define ModelNodeComponent_HPP

// Headers here.
#include "ECS/Component.hpp"
#include "Rendering/Model.hpp"

#include <cereal/access.hpp>
#include <Data/Serialization/VectorSerialization.hpp>

namespace Lina
{
    namespace Graphics
    {
        class ModelNode;
    }
} // namespace Lina

namespace Lina::ECS
{
    LINA_COMPONENT("Model Node", "ICON_FA_CUBES", "Rendering", "false", "false")
    struct ModelNodeComponent : public Component
    {
        LINA_PROPERTY("Materials", "MaterialArray")
        Vector<Resources::ResourceHandle<Graphics::Material>> m_materials;
        
        LINA_PROPERTY("Materials_Names")
        Vector<String> m_materialsNames;

        int                                        m_nodeIndex = -1;
        Resources::ResourceHandle<Graphics::Model> m_model;
        bool                                       m_culled = false;

    private:
        friend class cereal::access;

        template <class Archive>
        void serialize(Archive& archive)
        {
            archive(m_materials, m_nodeIndex, m_model, m_isEnabled);
        }
    };

} // namespace Lina::ECS

#endif
