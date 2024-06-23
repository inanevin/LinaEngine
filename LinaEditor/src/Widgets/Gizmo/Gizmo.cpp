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

#include "Editor/Widgets/Gizmo/Gizmo.hpp"
#include "Editor/Editor.hpp"
#include "Common/System/System.hpp"
#include "Core/World/WorldManager.hpp"
#include "Core/World/EntityWorld.hpp"
#include "Core/Components/CameraComponent.hpp"

namespace Lina::Editor
{
	void Gizmo::Construct()
	{
	}

	void Gizmo::Tick(float delta)
	{
	}

	void Gizmo::Draw()
	{

		auto*				 camera	   = Editor::Get()->GetSystem()->CastSubsystem<WorldManager>(SubsystemType::WorldManager)->GetMainWorld()->GetActiveCamera();
		Vector4				 position  = Vector4(0, 0, 0, 1);
		Vector4				 position2 = Vector4(0, 5, 0, 1);
		auto				 aq		   = camera->GetView() * position;
		auto				 aq2	   = camera->GetView() * position2;
		LinaVG::StyleOptions opts;
		opts.thickness = 0.25f;
		// m_lvg->DrawLine(LinaVG::Vec2(0,5), LinaVG::Vec2(5, 5), opts);
		// m_lvg->DrawBezier(LinaVG::Vec2(0, 5), LinaVG::Vec2(3, 0), LinaVG::Vec2(8, 12), LinaVG::Vec2(15, 5), opts);
	}
} // namespace Lina::Editor
