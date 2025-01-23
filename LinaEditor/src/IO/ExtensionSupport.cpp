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

#include "Editor/IO/ExtensionSupport.hpp"

#include "Core/Graphics/Resource/Material.hpp"
#include "Core/Graphics/Resource/Model.hpp"
#include "Core/Graphics/Resource/GUIWidget.hpp"
#include "Core/Graphics/Resource/Font.hpp"
#include "Core/Graphics/Resource/Texture.hpp"
#include "Core/Graphics/Resource/TextureSampler.hpp"
#include "Core/Graphics/Resource/Shader.hpp"
#include "Core/Audio/Audio.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Physics/PhysicsMaterial.hpp"
#include "Core/World/EntityTemplate.hpp"
#include "Common/Reflection/ReflectionSystem.hpp"

namespace Lina::Editor
{
	void ExtensionSupport::AddTypeInfoToReflection()
	{
		ReflectionSystem& ref = ReflectionSystem::Get();

		ref.Meta<Material>()->AddProperty<String>("DisplayName"_hs, "Material");
		ref.Meta<Material>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentPrimary2);
		ref.Meta<Material>()->AddProperty<String>("TypeAbbv"_hs, "M");

		ref.Meta<EntityTemplate>()->AddProperty<String>("DisplayName"_hs, "Entity Template");
		ref.Meta<EntityTemplate>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentOrange);
		ref.Meta<EntityTemplate>()->AddProperty<String>("TypeAbbv"_hs, "ET");

		ref.Meta<Model>()->AddProperty<String>("DisplayName"_hs, "Model");
		ref.Meta<Model>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentSecondary);
		ref.Meta<Model>()->AddProperty<String>("Extensions"_hs, "gltf, glb");
		ref.Meta<Model>()->AddProperty<String>("TypeAbbv"_hs, "MD");

		ref.Meta<GUIWidget>()->AddProperty<String>("DisplayName"_hs, "GUI Widget");
		ref.Meta<GUIWidget>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentDarkIndigo);
		ref.Meta<GUIWidget>()->AddProperty<String>("TypeAbbv"_hs, "G");

		ref.Meta<Font>()->AddProperty<String>("DisplayName"_hs, "Font");
		ref.Meta<Font>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentOrange);
		ref.Meta<Font>()->AddProperty<String>("Extensions"_hs, "otf, ttf");
		ref.Meta<Font>()->AddProperty<String>("TypeAbbv"_hs, "F");

		ref.Meta<Texture>()->AddProperty<String>("DisplayName"_hs, "Texture");
		ref.Meta<Texture>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentSuccess);
		ref.Meta<Texture>()->AddProperty<String>("Extensions"_hs, "png, jpg, jpeg");
		ref.Meta<Texture>()->AddProperty<String>("TypeAbbv"_hs, "T");

		ref.Meta<TextureSampler>()->AddProperty<String>("DisplayName"_hs, "Sampler");
		ref.Meta<TextureSampler>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentSuccess1);
		ref.Meta<TextureSampler>()->AddProperty<String>("TypeAbbv"_hs, "SM");

		ref.Meta<Shader>()->AddProperty<String>("DisplayName"_hs, "Shader");
		ref.Meta<Shader>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentCyan);
		ref.Meta<Shader>()->AddProperty<String>("Extensions"_hs, "linashader");
		ref.Meta<Shader>()->AddProperty<String>("TypeAbbv"_hs, "S");

		ref.Meta<Audio>()->AddProperty<String>("DisplayName"_hs, "Audio");
		ref.Meta<Audio>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentWarn);
		ref.Meta<Audio>()->AddProperty<String>("Extensions"_hs, "wav");
		ref.Meta<Audio>()->AddProperty<String>("TypeAbbv"_hs, "A");

		ref.Meta<EntityWorld>()->AddProperty<String>("DisplayName"_hs, "World");
		ref.Meta<EntityWorld>()->AddProperty<Color>("Color"_hs, Theme::GetDef().foreground0);
		ref.Meta<EntityWorld>()->AddProperty<String>("TypeAbbv"_hs, "W");

		ref.Meta<PhysicsMaterial>()->AddProperty<String>("DisplayName"_hs, "Phys Material");
		ref.Meta<PhysicsMaterial>()->AddProperty<Color>("Color"_hs, Theme::GetDef().accentYellowGold);
		ref.Meta<PhysicsMaterial>()->AddProperty<String>("TypeAbbv"_hs, "PM");
	}

} // namespace Lina::Editor
