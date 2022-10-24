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

#include "Lina/ReflectionRegistry.hpp"
#include "Reflection/ReflectionSystem.hpp"
#include "Core/ComponentCache.hpp"
#include "Utility/StringId.hpp"

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

//INC_BEGIN - !! DO NOT MODIFY THIS LINE !!
#include "Components/EditorFreeLookComponent.hpp"
#include "Components/SkyComponent.hpp"
#include "Components/ModelNodeComponent.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/DecalComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/LightComponent.hpp"
#include "Components/ParticleComponent.hpp"
#include "Components/RenderableComponent.hpp"
#include "Components/SpriteComponent.hpp"
#include "Settings/EditorSettings.hpp"
#include "Settings/EngineSettings.hpp"
#include "Settings/RenderSettings.hpp"
//INC_END - !! DO NOT MODIFY THIS LINE !!

namespace Lina
{

#define DEFAULT_COMP_CHUNK_SIZE 512

    bool g_reflectedTypesRegistered;

    template <typename T>
    void* REF_CreateComponentFunc()
    {
        T* t = new T();
        return static_cast<void*>(t);
    }

    template <typename T>
    void* REF_CreateComponentCacheFunc()
    {
        World::ComponentCache<T>* c = new World::ComponentCache<T>();
        return static_cast<void*>(c);
    }

    template <typename T>
    void REF_DestroyComponentFunc(void* ptr)
    {
        T* t = static_cast<T*>(ptr);
        delete t;
    }

    void RegisterReflectedTypes()
    {
        g_reflectedTypesRegistered = true;

//REGFUNC_BEGIN - !! DO NOT MODIFY THIS LINE !!
Reflection::Meta<World::EditorFreeLookComponent>().AddProperty("Title"_hs,"Editor Free Look");
Reflection::Meta<World::EditorFreeLookComponent>().AddProperty("Category"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<World::EditorFreeLookComponent>().AddField<&World::EditorFreeLookComponent::movementSpeed, World::EditorFreeLookComponent>("movementSpeed"_hs);
Reflection::Meta<World::EditorFreeLookComponent>().GetField("movementSpeed"_hs)->AddProperty("Title"_hs,"Movement Speed");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("movementSpeed"_hs)->AddProperty("Type"_hs,"Float");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("movementSpeed"_hs)->AddProperty("Tooltip"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("movementSpeed"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("movementSpeed"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().AddField<&World::EditorFreeLookComponent::rotationPower, World::EditorFreeLookComponent>("rotationPower"_hs);
Reflection::Meta<World::EditorFreeLookComponent>().GetField("rotationPower"_hs)->AddProperty("Title"_hs,"Rotation Power");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("rotationPower"_hs)->AddProperty("Type"_hs,"float");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("rotationPower"_hs)->AddProperty("Tooltip"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("rotationPower"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().GetField("rotationPower"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<World::EditorFreeLookComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<World::EditorFreeLookComponent>);
Reflection::Meta<World::EditorFreeLookComponent>().createFunc = std::bind(&REF_CreateComponentFunc<World::EditorFreeLookComponent>);
Reflection::Meta<World::EditorFreeLookComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<World::EditorFreeLookComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::SkyComponent>().AddProperty("Title"_hs,"Sky Component");
Reflection::Meta<Graphics::SkyComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::SkyComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::SkyComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::SkyComponent>);
Reflection::Meta<Graphics::SkyComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::SkyComponent>);
Reflection::Meta<Graphics::SkyComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::SkyComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::ModelNodeComponent>().AddProperty("Title"_hs,"Model Node");
Reflection::Meta<Graphics::ModelNodeComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::ModelNodeComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::ModelNodeComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::ModelNodeComponent>);
Reflection::Meta<Graphics::ModelNodeComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::ModelNodeComponent>);
Reflection::Meta<Graphics::ModelNodeComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::ModelNodeComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::CameraComponent>().AddProperty("Title"_hs,"Camera Component");
Reflection::Meta<Graphics::CameraComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::CameraComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::CameraComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::CameraComponent>);
Reflection::Meta<Graphics::CameraComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::CameraComponent>);
Reflection::Meta<Graphics::CameraComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::CameraComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::DecalComponent>().AddProperty("Title"_hs,"Decal Component");
Reflection::Meta<Graphics::DecalComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::DecalComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::DecalComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::DecalComponent>);
Reflection::Meta<Graphics::DecalComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::DecalComponent>);
Reflection::Meta<Graphics::DecalComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::DecalComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::SpotLightComponent>().AddProperty("Title"_hs,"Spot Light Component");
Reflection::Meta<Graphics::SpotLightComponent>().AddProperty("Category"_hs,"Lights");
Reflection::Meta<Graphics::SpotLightComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::SpotLightComponent>().AddField<&Graphics::SpotLightComponent::distance, Graphics::SpotLightComponent>("distance"_hs);
Reflection::Meta<Graphics::SpotLightComponent>().GetField("distance"_hs)->AddProperty("Title"_hs,"Distance");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("distance"_hs)->AddProperty("Type"_hs,"Float");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("distance"_hs)->AddProperty("Tooltip"_hs,"Light Distance");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("distance"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("distance"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Graphics::SpotLightComponent>().AddField<&Graphics::SpotLightComponent::cutoff, Graphics::SpotLightComponent>("cutoff"_hs);
Reflection::Meta<Graphics::SpotLightComponent>().GetField("cutoff"_hs)->AddProperty("Title"_hs,"Cutoff");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("cutoff"_hs)->AddProperty("Type"_hs,"Float");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("cutoff"_hs)->AddProperty("Tooltip"_hs,"The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("cutoff"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("cutoff"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Graphics::SpotLightComponent>().AddField<&Graphics::SpotLightComponent::outerCutoff, Graphics::SpotLightComponent>("outerCutoff"_hs);
Reflection::Meta<Graphics::SpotLightComponent>().GetField("outerCutoff"_hs)->AddProperty("Title"_hs,"Outer Cutoff");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("outerCutoff"_hs)->AddProperty("Type"_hs,"Float");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("outerCutoff"_hs)->AddProperty("Tooltip"_hs,"The light will gradually dim from the edges of the cone defined by the Cutoff, to the cone defined by the Outer Cutoff.");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("outerCutoff"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Graphics::SpotLightComponent>().GetField("outerCutoff"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Graphics::SpotLightComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::SpotLightComponent>);
Reflection::Meta<Graphics::SpotLightComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::SpotLightComponent>);
Reflection::Meta<Graphics::SpotLightComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::SpotLightComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::LightComponent>().AddProperty("Title"_hs,"Light Component");
Reflection::Meta<Graphics::LightComponent>().AddProperty("Category"_hs,"Lights");
Reflection::Meta<Graphics::LightComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::LightComponent>().AddField<&Graphics::LightComponent::color, Graphics::LightComponent>("color"_hs);
Reflection::Meta<Graphics::LightComponent>().GetField("color"_hs)->AddProperty("Title"_hs,"Color");
Reflection::Meta<Graphics::LightComponent>().GetField("color"_hs)->AddProperty("Type"_hs,"Color");
Reflection::Meta<Graphics::LightComponent>().GetField("color"_hs)->AddProperty("Tooltip"_hs,"");
Reflection::Meta<Graphics::LightComponent>().GetField("color"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Graphics::LightComponent>().GetField("color"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Graphics::LightComponent>().AddField<&Graphics::LightComponent::intensity, Graphics::LightComponent>("intensity"_hs);
Reflection::Meta<Graphics::LightComponent>().GetField("intensity"_hs)->AddProperty("Title"_hs,"Intensity");
Reflection::Meta<Graphics::LightComponent>().GetField("intensity"_hs)->AddProperty("Type"_hs,"Float");
Reflection::Meta<Graphics::LightComponent>().GetField("intensity"_hs)->AddProperty("Tooltip"_hs,"");
Reflection::Meta<Graphics::LightComponent>().GetField("intensity"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Graphics::LightComponent>().GetField("intensity"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Graphics::PointLightComponent>().AddProperty("Title"_hs,"Point Light Component");
Reflection::Meta<Graphics::PointLightComponent>().AddProperty("Category"_hs,"Lights");
Reflection::Meta<Graphics::PointLightComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::PointLightComponent>().AddField<&Graphics::PointLightComponent::distance, Graphics::PointLightComponent>("distance"_hs);
Reflection::Meta<Graphics::PointLightComponent>().GetField("distance"_hs)->AddProperty("Title"_hs,"Distance");
Reflection::Meta<Graphics::PointLightComponent>().GetField("distance"_hs)->AddProperty("Type"_hs,"Float");
Reflection::Meta<Graphics::PointLightComponent>().GetField("distance"_hs)->AddProperty("Tooltip"_hs,"Light Distance");
Reflection::Meta<Graphics::PointLightComponent>().GetField("distance"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Graphics::PointLightComponent>().GetField("distance"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Graphics::PointLightComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::PointLightComponent>);
Reflection::Meta<Graphics::PointLightComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::PointLightComponent>);
Reflection::Meta<Graphics::PointLightComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::PointLightComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::DirectionalLightComponent>().AddProperty("Title"_hs,"Directional Light Component");
Reflection::Meta<Graphics::DirectionalLightComponent>().AddProperty("Category"_hs,"Lights");
Reflection::Meta<Graphics::DirectionalLightComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::DirectionalLightComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::DirectionalLightComponent>);
Reflection::Meta<Graphics::DirectionalLightComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::DirectionalLightComponent>);
Reflection::Meta<Graphics::DirectionalLightComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::DirectionalLightComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::ParticleComponent>().AddProperty("Title"_hs,"Particle Component");
Reflection::Meta<Graphics::ParticleComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::ParticleComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::ParticleComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::ParticleComponent>);
Reflection::Meta<Graphics::ParticleComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::ParticleComponent>);
Reflection::Meta<Graphics::ParticleComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::ParticleComponent>, std::placeholders::_1);
Reflection::Meta<Graphics::RenderableComponent>().AddProperty("Title"_hs,"Renderable");
Reflection::Meta<Graphics::RenderableComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::RenderableComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::SpriteComponent>().AddProperty("Title"_hs,"Sprite Component");
Reflection::Meta<Graphics::SpriteComponent>().AddProperty("Category"_hs,"Graphics");
Reflection::Meta<Graphics::SpriteComponent>().AddProperty("MemChunkSize"_hs,TO_STRING(DEFAULT_COMP_CHUNK_SIZE));
Reflection::Meta<Graphics::SpriteComponent>().createCompCacheFunc = std::bind(&REF_CreateComponentCacheFunc<Graphics::SpriteComponent>);
Reflection::Meta<Graphics::SpriteComponent>().createFunc = std::bind(&REF_CreateComponentFunc<Graphics::SpriteComponent>);
Reflection::Meta<Graphics::SpriteComponent>().destroyFunc = std::bind(&REF_DestroyComponentFunc<Graphics::SpriteComponent>, std::placeholders::_1);
Reflection::Meta<Editor::EditorSettings>().AddProperty("Title"_hs,"Editor Settings");
Reflection::Meta<Editor::EditorSettings>().AddField<&Editor::EditorSettings::m_textEditorPath, Editor::EditorSettings>("m_textEditorPath"_hs);
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Title"_hs,"Text Editor");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Type"_hs,"StringPath");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Tooltip"_hs,"Default text editor to open shader & similar files.");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Depends On"_hs,"");
Reflection::Meta<Editor::EditorSettings>().GetField("m_textEditorPath"_hs)->AddProperty("Category"_hs,"");
Reflection::Meta<Lina::EngineSettings>().AddProperty("Title"_hs,"Engine Settings");
Reflection::Meta<Lina::RenderSettings>().AddProperty("Title"_hs,"Render Settings");
//REGFUNC_END - !! DO NOT MODIFY THIS LINE !!
    }
} // namespace Lina
