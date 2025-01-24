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

#include "EditorAction.hpp"
#include "Common/Data/String.hpp"
#include "Editor/World/CommonEditorWorld.hpp"
#include "Core/World/CommonWorld.hpp"
#include "Core/Physics/CommonPhysics.hpp"

namespace Lina
{
	class Entity;
	class EntityWorld;
}; // namespace Lina

namespace Lina::Editor
{
	class Editor;

	class EditorActionWorldSnappingChanged : public EditorAction
	{
	public:
		EditorActionWorldSnappingChanged()			= default;
		virtual ~EditorActionWorldSnappingChanged() = default;

		static EditorActionWorldSnappingChanged* Create(Editor* editor, EntityWorld* world, const WorldSnapSettings& prevSettings, const WorldSnapSettings& currentSettings);
		virtual void							 Execute(Editor* editor, ExecType type) override;

	private:
		WorldSnapSettings m_prevSettings	= {};
		WorldSnapSettings m_currentSettings = {};
		uint64			  m_worldId			= 0;
	};


	class EditorActionWorldGfxChanged : public EditorAction
	{
	public:
		EditorActionWorldGfxChanged()		   = default;
		virtual ~EditorActionWorldGfxChanged() = default;

		static EditorActionWorldGfxChanged* Create(Editor* editor, EntityWorld* world, const WorldGfxSettings& prevSettings, const WorldGfxSettings& currentSettings);
		virtual void						Execute(Editor* editor, ExecType type) override;

	private:
		WorldGfxSettings m_prevSettings	   = {};
		WorldGfxSettings m_currentSettings = {};
		uint64			 m_worldId		   = 0;
	};

	class EditorActionWorldPhysicsSettingsChanged : public EditorAction
	{
	public:
		EditorActionWorldPhysicsSettingsChanged()		   = default;
		virtual ~EditorActionWorldPhysicsSettingsChanged() = default;

		static EditorActionWorldPhysicsSettingsChanged* Create(Editor* editor, EntityWorld* world, const WorldPhysicsSettings& prevSettings, const WorldPhysicsSettings& currentSettings);
		virtual void									Execute(Editor* editor, ExecType type) override;

	private:
		WorldPhysicsSettings m_prevSettings	   = {};
		WorldPhysicsSettings m_currentSettings = {};
		uint64				 m_worldId		   = 0;
	};
} // namespace Lina::Editor
