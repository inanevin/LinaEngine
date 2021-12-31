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

#include "World/Level.hpp"

#include "Core/Application.hpp"
#include "Core/RenderEngineBackend.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "ECS/Components/MeshRendererComponent.hpp"
#include "ECS/Components/ModelRendererComponent.hpp"
#include "ECS/Components/SpriteRendererComponent.hpp"
#include "ECS/Registry.hpp"
#include "Log/Log.hpp"
#include "Utility/UtilityFunctions.hpp"
#include "Resources/ResourceStorage.hpp"
#include <cereal/archives/portable_binary.hpp>
#include <fstream>
#include <stdio.h>

namespace Lina::World
{

    void Level::ExportLevel(const std::string& path, const std::string& name)
    {
        const std::string finalPath = path + "/" + name + ".linalevel";

        // Delete if exists.
        if (Utility::FileExists(finalPath))
            Utility::DeleteFileInPath(finalPath);

        ECS::Registry* registry = ECS::Registry::Get();
        {

            std::ofstream levelDataStream(finalPath, std::ios::binary);
            {
                cereal::PortableBinaryOutputArchive oarchive(levelDataStream);
                oarchive(m_levelData);
                registry->SerializeComponentsInRegistry(oarchive);
            }
        }
    }

    void Level::ImportLevel(const std::string& path, const std::string& name)
    {
        ECS::Registry* registry = ECS::Registry::Get();

        {
            std::ifstream levelDataStream(path + "/" + name + ".linalevel", std::ios::binary);
            {
                cereal::PortableBinaryInputArchive iarchive(levelDataStream);
                iarchive(m_levelData);
                registry->clear();
                registry->DeserializeComponentsInRegistry(iarchive);
            }
        }
    }

    bool Level::Install(bool loadFromFile, const std::string& path, const std::string& levelName)
    {
        if (loadFromFile)
        {
            if (Utility::FileExists(path + "/" + levelName + ".linalevel"))
                LoadLevelResources();
        }

        return true;
    }

    void Level::Uninstall()
    {
    }

    void Level::LoadLevelResources()
    {
        ECS::Registry* ecs = ECS::Registry::Get();

        auto& view = ecs->view<ECS::ModelRendererComponent>();

        for (ECS::Entity entity : view)
        {
            ECS::ModelRendererComponent& mr = view.get<ECS::ModelRendererComponent>(entity);
            // Assign model pointed by the model renderer.
            if (Graphics::Model::ModelExists(mr.m_modelPath))
                mr.m_modelID = Graphics::Model::GetModel(mr.m_modelPath).GetID();
        }

        auto& meshView = ecs->view<ECS::MeshRendererComponent>();

        for (ECS::Entity entity : meshView)
        {
            ECS::MeshRendererComponent& meshRenderer = ecs->get<ECS::MeshRendererComponent>(entity);
            if (Graphics::Material::MaterialExists(meshRenderer.m_materialPath))
                meshRenderer.m_materialID = Graphics::Material::GetMaterial(meshRenderer.m_materialPath).GetID();
            else
            {
                Graphics::Material& mat     = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultLit.mat");
                meshRenderer.m_materialID   = mat.GetID();
                meshRenderer.m_materialPath = mat.GetPath();
            }

            if (Graphics::Model::ModelExists(meshRenderer.m_modelPath))
                meshRenderer.m_modelID = Graphics::Model::GetModel(meshRenderer.m_modelPath).GetID();
            else
            {
                meshRenderer.m_modelID   = -1;
                meshRenderer.m_modelPath = "";
            }
        }

        auto& viewSprites = ecs->view<ECS::SpriteRendererComponent>();

        for (ECS::Entity entity : viewSprites)
        {
            ECS::SpriteRendererComponent& sprite = viewSprites.get<ECS::SpriteRendererComponent>(entity);

            if (Graphics::Material::MaterialExists(sprite.m_materialPaths))
            {
                Graphics::Material& mat = Graphics::Material::GetMaterial(sprite.m_materialPaths);
                sprite.m_materialID     = mat.GetID();
            }
            else
            {
                Graphics::Material& mat = Graphics::Material::GetMaterial("Resources/Engine/Materials/DefaultSprite.mat");
                sprite.m_materialID     = mat.GetID();
                sprite.m_materialPaths  = mat.GetPath();
            }
        }

        auto& viewPhysics = ecs->view<ECS::PhysicsComponent>();

        for (ECS::Entity entity : viewPhysics)
        {
            ECS::PhysicsComponent& phy = viewPhysics.get<ECS::PhysicsComponent>(entity);

            auto* storage = Resources::ResourceStorage::Get();
            if (storage->Exists<Physics::PhysicsMaterial>(phy.m_material.m_sid))
            {
                phy.m_material.m_value = storage->GetResource<Physics::PhysicsMaterial>(phy.m_material.m_sid);
            }
            else
            {
                Physics::PhysicsMaterial* mat = storage->GetResource<Physics::PhysicsMaterial>("Resources/Engine/Physics/Materials/DefaultPhysicsMaterial.linaphymat");
                phy.m_material.m_value        = mat;
                phy.m_material.m_sid          = mat->GetSID();
            }
        }

        Graphics::RenderEngineBackend* renderEngine = Graphics::RenderEngineBackend::Get();

        if (Graphics::Material::MaterialExists(m_levelData.m_skyboxMaterialPath))
        {
            Graphics::Material& mat = Graphics::Material::GetMaterial(m_levelData.m_skyboxMaterialPath);
            renderEngine->SetSkyboxMaterial(&mat);
        }
        else
            renderEngine->SetSkyboxMaterial(nullptr);

        renderEngine->GetLightingSystem()->SetAmbientColor(m_levelData.m_ambientColor);
    }

    void Level::SetSkyboxMaterial()
    {
        Graphics::RenderEngineBackend* renderEngine = Graphics::RenderEngineBackend::Get();

        if (Graphics::Material::MaterialExists(m_levelData.m_skyboxMaterialPath))
        {
            renderEngine->SetSkyboxMaterial(&Graphics::Material::GetMaterial(m_levelData.m_skyboxMaterialPath));
        }
        else
            renderEngine->SetSkyboxMaterial(nullptr);
    }

} // namespace Lina::World
