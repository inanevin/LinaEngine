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

#ifndef GUINodeDockPreview_HPP
#define GUINodeDockPreview_HPP

#include "GUI/Nodes/GUINode.hpp"
#include "GUI/Nodes/Docking/GUINodeDockArea.hpp"
#include "Core/EditorCommon.hpp"
#include "Data/HashMap.hpp"

namespace LinaGX
{
	class Input;
}

namespace Lina::Editor
{
	class GUINodeDockPreview : public GUINode
	{
	public:
		GUINodeDockPreview(GUIDrawerBase* drawer, int drawOrder);
		virtual ~GUINodeDockPreview() = default;

		virtual void Draw(int threadID) override;

		inline DockSplitType GetCurrentSplitType() const
		{
			return m_currentHoveredSplit;
		}

		inline void Reset()
		{
			m_currentHoveredSplit = DockSplitType::None;
		}

		inline void SetIsOuterPreview(bool isOuter)
		{
			m_isOuter = isOuter;
		}

		inline bool GetIsActive() const
		{
			return m_isActive;
		}

	protected:
		HashMap<DockSplitType, float> m_scales;
		bool						  m_isActive			= false;
		bool						  m_isOuter				= false;
		DockSplitType				  m_currentHoveredSplit = DockSplitType::None;
		LinaGX::Input*				  m_input				= nullptr;
	};
} // namespace Lina::Editor

#endif
