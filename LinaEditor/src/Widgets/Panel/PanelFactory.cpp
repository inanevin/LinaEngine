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

#include "Editor/Widgets/Panel/PanelFactory.hpp"
#include "Editor/Widgets/Panel/PanelEntities.hpp"
#include "Editor/Widgets/Panel/PanelWorld.hpp"
#include "Editor/Widgets/Panel/PanelPerformance.hpp"
#include "Editor/Widgets/Panel/PanelWidgetEditor.hpp"
#include "Editor/Widgets/Panel/PanelColorWheel.hpp"
#include "Editor/Widgets/Panel/PanelResourceBrowser.hpp"
#include "Editor/Widgets/Panel/PanelTextureViewer.hpp"
#include "Editor/Widgets/Panel/PanelFontViewer.hpp"
#include "Editor/Widgets/Panel/PanelModelViewer.hpp"
#include "Editor/Widgets/Panel/PanelShaderViewer.hpp"
#include "Editor/Widgets/Panel/PanelSamplerViewer.hpp"
#include "Editor/Widgets/Panel/PanelPhysicsMaterialViewer.hpp"
#include "Editor/Widgets/Panel/PanelMaterialViewer.hpp"
#include "Editor/Widgets/Panel/PanelGenericSelector.hpp"
#include "Editor/Widgets/Panel/PanelAudioViewer.hpp"
#include "Editor/Widgets/Panel/PanelLog.hpp"
#include "Editor/Widgets/Panel/PanelDetails.hpp"
#include "Core/GUI/Widgets/WidgetManager.hpp"

namespace Lina::Editor
{

	Panel* PanelFactory::CreatePanel(Widget* source, PanelType type, ResourceID subData)
	{
		Panel* panel = nullptr;

		switch (type)
		{
		case PanelType::Performance:
			panel = source->GetWidgetManager()->Allocate<PanelPerformance>("Performance");
			break;
		case PanelType::ResourceBrowser:
			panel = source->GetWidgetManager()->Allocate<PanelResourceBrowser>("Resource Browser");
			break;
		case PanelType::World:
			panel = source->GetWidgetManager()->Allocate<PanelWorld>("World");
			break;
		case PanelType::Entities:
			panel = source->GetWidgetManager()->Allocate<PanelEntities>("Entities");
			break;
		case PanelType::Log:
			panel = source->GetWidgetManager()->Allocate<PanelLog>("Log");
			break;
		case PanelType::WidgetEditor:
			panel = source->GetWidgetManager()->Allocate<PanelWidgetEditor>("Widget Editor");
			break;
		case PanelType::ColorWheel:
			panel = source->GetWidgetManager()->Allocate<PanelColorWheel>("Color Wheel");
			break;
		case PanelType::TextureViewer:
			panel = source->GetWidgetManager()->Allocate<PanelTextureViewer>("Texture");
			break;
		case PanelType::FontViewer:
			panel = source->GetWidgetManager()->Allocate<PanelFontViewer>("Font");
			break;
		case PanelType::ModelViewer:
			panel = source->GetWidgetManager()->Allocate<PanelModelViewer>("Model");
			break;
		case PanelType::ShaderViewer:
			panel = source->GetWidgetManager()->Allocate<PanelShaderViewer>("Shader");
			break;
		case PanelType::SamplerViewer:
			panel = source->GetWidgetManager()->Allocate<PanelSamplerViewer>("Sampler");
			break;
		case PanelType::PhysicsMaterialViewer:
			panel = source->GetWidgetManager()->Allocate<PanelPhysicsMaterialViewer>("PhyMat");
			break;
		case PanelType::MaterialViewer:
			panel = source->GetWidgetManager()->Allocate<PanelMaterialViewer>("Material");
			break;
		case PanelType::AudioViewer:
			panel = source->GetWidgetManager()->Allocate<PanelAudioViewer>("Audio");
			break;
		case PanelType::Details:
			panel = source->GetWidgetManager()->Allocate<PanelDetails>("Details");
			break;
            case PanelType::GenericSelector:
                panel = source->GetWidgetManager()->Allocate<PanelGenericSelector>("Selector");
            break;
		default:
			LINA_ASSERT(false, "");
			break;
		}

		panel->GetWidgetProps().drawBackground	 = true;
		panel->GetWidgetProps().outlineThickness = 0.0f;
		// panel->GetWidgetProps().outlineIsInner		 = true;
		panel->GetWidgetProps().rounding					  = 0.0f;
		panel->GetWidgetProps().colorBackground				  = Theme::GetDef().background2;
		panel->GetWidgetProps().dropshadow.enabled			  = true;
		panel->GetWidgetProps().dropshadow.direction		  = Direction::Top;
		panel->GetWidgetProps().dropshadow.isInner			  = true;
		panel->GetWidgetProps().dropshadow.color			  = Theme::GetDef().background0;
		panel->GetWidgetProps().dropshadow.color.w			  = 0.5f;
		panel->GetWidgetProps().dropshadow.drawOrderIncrement = 5;
		panel->GetWidgetProps().dropshadow.steps			  = 8;
		panel->SetSubdata(subData);
		panel->Initialize();
		return panel;
	}
} // namespace Lina::Editor
