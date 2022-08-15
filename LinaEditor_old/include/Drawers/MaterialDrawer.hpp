/*
Class: MaterialDrawer



Timestamp: 1/9/2022 11:54:11 PM
*/

#pragma once

#ifndef MaterialDrawer_HPP
#define MaterialDrawer_HPP

// Headers here.
namespace Lina
{
	class Vector2;

	namespace Graphics
    {
		class Material;
		class Model;
	}
}

namespace Lina::Editor
{
	class MaterialDrawer
	{
		
	public:
		
		MaterialDrawer() = default;
		~MaterialDrawer() = default;

		static void DrawMaterialSettings(Graphics::Material*& mat, float leftPaneSize);
        static void DrawMaterial(Graphics::Material* mat, Graphics::Model* previewModel, const Vector2& bgMin, const Vector2& bgMax);
	
	private:
	
	};
}

#endif
