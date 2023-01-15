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

#pragma once
#ifndef EventSystem_HPP
#define EventSystem_HPP

#include "Data/HashMap.hpp"
#include "Data/Mutex.hpp"
#include "Functional/Functional.hpp"
#include "Utility/StringId.hpp"

namespace Lina
{
    class Engine;
    class FileWatcher;
    class Log;

    namespace Editor
    {
        class Editor;
        class ShortcutManager;
    } // namespace Editor

    namespace World
    {
        class Entity;
        class LevelManager;
        class Component;
        class EntityWorld;
    } // namespace World

    namespace Resources
    {
        class EditorResourceLoader;
        class ResourceLoader;
        class ResourcePackager;
        class ResourceUtility;
    } // namespace Resources

    namespace Graphics
    {
        class Renderer;
        class WorldRenderer;
        class SurfaceRenderer;
        class RenderEngine;
        class Win32Window;
        class WindowManager;
    } // namespace Graphics

} // namespace Lina

namespace Lina::Event
{

    template <typename T> class EventSink
    {
    public:
        typedef Delegate<void(const T& t)> FuncTemplate;

        template <auto DATA, typename Type> void Connect(Type* obj)
        {
            functions[static_cast<void*>(obj)] = [obj](const T& ev) { (obj->*DATA)(ev); };
        }

        void Trigger(const T& t) const
        {
            for (auto& [ptr, func] : functions)
                func(t);
        }

        void Trigger() const
        {
            for (auto& [ptr, func] : functions)
                func(T());
        }

        template <typename T> void Disconnect(T* inst)
        {
            void*       ptr = static_cast<void*>(inst);
            const auto& it  = functions.find(ptr);
            if (it != functions.end())
                functions.erase(it);
        }

        HashMap<void*, FuncTemplate> functions;
    };

    template <typename T> void DestroySink(void* sink)
    {
        EventSink<T>* sinkPtr = static_cast<EventSink<T>*>(sink);
        sinkPtr->functions.clear();
        delete sinkPtr;
    }

    typedef Delegate<void(void* sink)> DisconnectFunc;

    class EventSystem
    {
    public:
        EventSystem()  = default;
        ~EventSystem() = default;

        inline static EventSystem* Get()
        {
            return s_eventSystem;
        }

    public:
        template <typename T, auto Candidate, typename Type> void Connect(Type* inst)
        {
            LOCK_GUARD(m_mtx);
            GetSink<T>()->Connect<Candidate>(inst);
        }

        template <typename T, typename Type> void Disconnect(Type* inst)
        {
            LOCK_GUARD(m_mtx);
            GetSink<T>()->Disconnect(inst);
        }

    private:
        friend class Engine;
        friend class Log;
        friend class Editor::Editor;
        friend class Editor::ShortcutManager;
        friend class Resources::EditorResourceLoader;
        friend class Graphics::Renderer;
        friend class Graphics::WorldRenderer;
        friend class Graphics::SurfaceRenderer;
        friend class Graphics::Win32Window;
        friend class Graphics::WindowManager;
        friend class Resources::ResourceLoader;
        friend class Resources::ResourceUtility;
        friend class Resources::ResourcePackager;
        friend class World::Entity;
        friend class World::LevelManager;
        friend class World::Component;
        friend class World::EntityWorld;

        template <typename T> EventSink<T>* GetSink()
        {
            const TypeID  tid  = GetTypeID<T>();
            EventSink<T>* sink = nullptr;

            if (m_eventSinks.find(tid) == m_eventSinks.end())
            {
                sink                       = new EventSink<T>();
                m_eventSinks[tid]          = static_cast<void*>(sink);
                m_disconnectFunctions[tid] = std::bind(DestroySink<T>, std::placeholders::_1);
            }
            else
                sink = static_cast<EventSink<T>*>(m_eventSinks.at(tid));
            return sink;
        }

        template <typename T> void Trigger(const T& args)
        {
            GetSink<T>()->Trigger(args);
        }

        template <typename T> void Trigger(T&& args)
        {
            GetSink<T>()->Trigger(args);
        }

        template <typename T> void Trigger()
        {
            GetSink<T>()->Trigger();
        }

        void Initialize();
        void Shutdown();

    private:
        static EventSystem*             s_eventSystem;
        HashMap<TypeID, void*>          m_eventSinks;
        HashMap<TypeID, DisconnectFunc> m_disconnectFunctions;
        Mutex                           m_mtx;
    };
} // namespace Lina::Event

#endif
