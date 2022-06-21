/*
Class: PropertiesPanel

The central system for drawing properties of any objects & files including materials, textures,
meshes, entities etc.

Timestamp: 6/7/2020 5:13:24 PM
*/

#pragma once

#ifndef PropertiesPanel_HPP
#define PropertiesPanel_HPP

#include "Core/EditorCommon.hpp"
#include "Drawers/EntityDrawer.hpp"
#include "Panels/EditorPanel.hpp"

namespace Lina
{
    namespace Graphics
    {
        class Model;
        class Material;
    } // namespace Graphics
} // namespace Lina

namespace Lina::Editor
{
    class EditorFile;
    struct EEntitySelected;
    struct ETextureSelected;
    struct EModelSelected;
    struct EEntityUnselected;

    class PropertiesPanel : public EditorPanel
    {

        enum class DrawType
        {
            None,
            Entities,
            Texture2D,
            Model,
            Material
        };

    public:
        PropertiesPanel() = default;
        virtual ~PropertiesPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;

        void EntitySelected(const EEntitySelected& ev);
        void TextureSelected(const ETextureSelected& ev);
        void ModelSelected(const EModelSelected& ev);
        void Unselect(const EEntityUnselected& ev);

    private:
        // Selected entity.
        EntityDrawer m_entityDrawer;

        DrawType m_currentDrawType = DrawType::None;
    };
} // namespace Lina::Editor

#endif
