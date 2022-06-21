/*
Class: ResourceSelectorPanel



Timestamp: 1/2/2022 4:40:26 PM
*/

#pragma once

#ifndef ResourceSelectorPanel_HPP
#define ResourceSelectorPanel_HPP

// Headers here.
#include "Panels/EditorPanel.hpp"
#include "Core/RenderEngineFwd.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    namespace Resources
    {
        class ResourceStorage;
    }
    namespace Utility
    {
        struct Folder;
        struct File;
    } // namespace Utility
} // namespace Lina
namespace Lina::Editor
{
    class ResourceSelectorPanel : public EditorPanel
    {

    public:
        ResourceSelectorPanel()  = default;
        ~ResourceSelectorPanel() = default;

        virtual void Initialize(const char* id, const char* icon) override;
        virtual void Draw() override;


        inline void SetCurrentTypeID(TypeID tid, const String& typeStr, const String& resourceSelectorID)
        {
            m_currentFileType   = tid;
            m_currentSelectorID = resourceSelectorID;
            m_resourceStr       = typeStr;
            m_selectedFile = nullptr;
        }

        StringIDType m_selectedResource  = 0;
        String  m_currentSelectorID = "";
    private:
        void DrawTop();
        void DrawBottom();
        void DrawFilesInFolder(Utility::Folder* folder);

    private:
   
        bool                        m_firstLaunch       = true;
        TypeID                      m_currentFileType   = 0;
        Resources::ResourceStorage* m_storage           = nullptr;
        Utility::File*              m_selectedFile      = nullptr;
        Utility::Folder*            m_root              = nullptr;
        Graphics::Window*           m_window            = nullptr;
        String                 m_searchFilter      = "";
        String                 m_resourceStr       = "";
    };
} // namespace Lina::Editor

#endif
