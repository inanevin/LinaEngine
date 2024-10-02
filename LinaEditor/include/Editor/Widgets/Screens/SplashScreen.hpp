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

#include "Core/GUI/Widgets/Widget.hpp"
#include "Core/Resources/ResourceManagerListener.hpp"
#include "Common/Data/Mutex.hpp"

namespace Lina
{
	class Icon;
	class Texture;
	class Text;
	class DirectionalLayout;
}; // namespace Lina

namespace Lina::Editor
{
	class SplashScreen : public Widget, public ResourceManagerListener
	{
	public:
		SplashScreen()			= default;
		virtual ~SplashScreen() = default;

		static constexpr float BAR_INTERP_SPEED = 12.0f;

		virtual void Construct() override;
		virtual void Destruct() override;
		virtual void Tick(float delta) override;
		virtual void Draw() override;

		virtual void OnResourcesPreLoaded(int32 taskID, const ResourceDefinitionList& idents) override;
		virtual void OnResourceLoaded(int32 taskID, const ResourceDef& ident) override;

	private:
		Mutex	 m_loadMtx;
		Texture* m_splashImage		   = nullptr;
		Text*	 m_versionText		   = nullptr;
		Text*	 m_infoText1		   = nullptr;
		Text*	 m_infoText2		   = nullptr;
		Text*	 m_loadingInfo		   = nullptr;
		Vector2	 m_logoTextureSize	   = Vector2::Zero;
		Vector2	 m_logoCenter		   = Vector2::Zero;
		Vector2	 m_logoDrawSize		   = Vector2::Zero;
		uint32	 m_totalResourceSize   = 0;
		uint32	 m_loadedResourceCount = 0;
		float	 m_progress			   = 0.0f;
		float	 m_loadingBarHeight	   = 0.0f;
	};

	LINA_WIDGET_BEGIN(SplashScreen, Hidden)
	LINA_CLASS_END(SplashScreen)

} // namespace Lina::Editor
