#include "Panels/PropertiesPanel.hpp"

#include "Core/Application.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/EditorCommon.hpp"
#include "Core/GUILayer.hpp"
#include "Drawers/EntityDrawer.hpp"
#include "IconsFontAwesome5.h"
#include "Rendering/Model.hpp"
#include "Utility/EditorUtility.hpp"
#include "Widgets/WidgetsUtility.hpp"

namespace Lina::Editor
{
    using namespace ECS;
    using namespace Lina;
    static bool s_openCompExistsModal;

    void PropertiesPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        Event::EventSystem::Get()->Connect<EEntitySelected, &PropertiesPanel::EntitySelected>(this);
        Event::EventSystem::Get()->Connect<EModelSelected, &PropertiesPanel::ModelSelected>(this);
        //	Event::EventSystem::Get()->Connect<EMaterialSelected, &PropertiesPanel::MaterialSelected>(this);
        Event::EventSystem::Get()->Connect<ETextureSelected, &PropertiesPanel::TextureSelected>(this);
        Event::EventSystem::Get()->Connect<EEntityUnselected, &PropertiesPanel::Unselect>(this);

        m_entityDrawer.Initialize();
    }

    void PropertiesPanel::EntitySelected(const EEntitySelected& ev)
    {
        m_entityDrawer.SetSelectedEntity(ev.m_entity);
        m_currentDrawType = DrawType::Entities;
    }

    void PropertiesPanel::TextureSelected(const ETextureSelected& ev)
    {
        // m_textureDrawer.SetSelectedTexture(ev.m_texture);
        m_currentDrawType = DrawType::Texture2D;
    }

    // void PropertiesPanel::MaterialSelected(EMaterialSelected ev)
    // {
    // 	m_materialDrawer.SetSelectedMaterial(ev.m_file, *ev.m_material);
    // 	m_currentDrawType = DrawType::Material;
    // }
    void PropertiesPanel::ModelSelected(const EModelSelected& ev)
    {
        // m_modelDrawer.SetSelectedModel(*ev.m_model);
        m_currentDrawType = DrawType::Model;
    }

    void PropertiesPanel::Draw()
    {
        if (m_show)
        {
            Begin();

            // Draw the selected item.
            if (m_currentDrawType == DrawType::Entities)
                m_entityDrawer.DrawSelectedEntity();
            // else if (m_currentDrawType == DrawType::Texture2D)
            //	m_textureDrawer.DrawSelectedTexture();
            // else if (m_currentDrawType == DrawType::Model)
            //	m_modelDrawer.DrawSelectedMesh();
            // else if (m_currentDrawType == DrawType::Material)
            //	m_materialDrawer.DrawSelectedMaterial();

            End();
        }
    }

    void PropertiesPanel::Unselect(const EEntityUnselected& ev)
    {
        m_currentDrawType = DrawType::None;
    }

} // namespace Lina::Editor
