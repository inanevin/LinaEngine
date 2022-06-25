/*
Class: EditorApplication

This class can also be derived from Lina::Application to create a standalone editor build.
Probably will be deprecated in the future as the editor is now a linked-library from Sandbox.

Timestamp: 10/15/2020 10:44:39 PM
*/

#pragma once

#ifndef EditorApplication_HPP
#define EditorApplication_HPP

#include "Core/GUILayer.hpp"
#include "ECS/Systems/EditorCameraSystem.hpp"
#include "Core/EditorSettings.hpp"
#include "Utility/FileWatcher.hpp"

namespace Lina
{
    namespace World
    {
        class Level;
    }

    namespace Graphics
    {
        class Material;
        class RenderTarget;
        class Texture;
        class RenderBuffer;
    } // namespace Graphics

    namespace Event
    {
        struct EPlayModeChanged;
        struct ELevelInstalled;
        struct EPreSerializingLevel;
        struct ESerializedLevel;
        struct EWindowResized;
        struct EResourceLoaded;
        struct EResourceReloaded;
        struct EResourcePathUpdated;

    } // namespace Event
} // namespace Lina

namespace Lina::Editor
{
    // Used for taking fixed-size snapshots of resources such as materials/models/ shaders
    struct SnapshotBuffer
    {
        Graphics::RenderTarget* m_rt           = nullptr;
        Graphics::Texture*      m_rtTexture    = nullptr;
        Graphics::RenderBuffer* m_renderBuffer = nullptr;
    };

    class EditorApplication
    {
    public:
        EditorApplication() = default;
        ~EditorApplication();
        void Initialize();

        inline static EditorApplication* Get()
        {
            return s_editorApplication;
        }

        inline ECS::EditorCameraSystem& GetCameraSystem()
        {
            return m_editorCameraSystem;
        }

        inline EditorSettings& GetEditorSettings()
        {
            return m_editorSettings;
        }

        Graphics::RenderTarget* AddSnapshotBuffer(StringIDType sid);
        uint32                  GetSnapshotTexture(StringIDType sid);

    private:
        void PlayModeChanged(const Event::EPlayModeChanged& playmode);
        void TakeModelSnapshot(StringIDType bufferSid, StringIDType modelSid, StringIDType materialSid);
        void OnLevelInstalled(const Event::ELevelInstalled& ev);
        void OnPreSerializingLevel(const Event::EPreSerializingLevel& ev);
        void OnSerializedLevel(const Event::ESerializedLevel& ev);
        void OnWindowResized(const Event::EWindowResized& ev);
        void OnResourceLoadCompleted(const Event::EResourceLoaded& ev);
        void OnResourceReloaded(const Event::EResourceReloaded& ev);
        void OnResourcePathUpdated(const Event::EResourcePathUpdated& ev);

    private:
        static EditorApplication*                        s_editorApplication;
        EditorSettings                                   m_editorSettings;
        ECS::EditorCameraSystem                          m_editorCameraSystem;
        GUILayer                                         m_guiLayer;
        FileWatcher                                      m_shaderWatcher;
        HashMap<StringIDType, SnapshotBuffer> m_previewBuffers;
        bool                                             m_snapshotsTaken = false;
    };
} // namespace Lina::Editor

#endif
