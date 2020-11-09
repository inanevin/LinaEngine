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

/*
Class: FPSDemoLevel



Timestamp: 11/3/2020 12:39:34 AM
*/

#pragma once

#ifndef FPSDemoLevel_HPP
#define FPSDemoLevel_HPP

#include "World/Level.hpp"
#include "Player.hpp"
#include "Rendering/RenderTarget.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Renderbuffer.hpp"
#include "FPSDemo/FPSDemoComponentDrawer.hpp"

namespace LinaEngine
{
	namespace ECS
	{
		class ECSRegistry;
	}

	class FPSDemoLevel : public LinaEngine::World::Level
	{
		
	public:
		
		FPSDemoLevel() {};
		~FPSDemoLevel() {};
	

		virtual bool Install(bool loadFromFile, const std::string& path, const std::string& levelName) override;
		virtual void Initialize() override;
		virtual void Tick(bool isInPlayMode, float delta) override;
		virtual void Uninstall() override;

	private:

		void PreDraw();
		void CustomDraw();

	private:

		Player m_player;
		ECS::ECSRegistry* m_registry = nullptr;
	
		bool m_isInPlayMode = false;
		Graphics::RenderTarget m_portalRT;
		Graphics::Texture m_portalTexture;
		Graphics::RenderBuffer m_portalBuffer;
		Graphics::SamplerParameters m_portalRTParams;

		FPSDemoComponentDrawer m_componentDrawer;

		bool m_processOnce = false;
	};
}

#endif
