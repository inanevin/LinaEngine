/*
Class: TextureDrawer



Timestamp: 1/9/2022 11:54:02 PM
*/

#pragma once

#ifndef TextureDrawer_HPP
#define TextureDrawer_HPP

// Headers here.

namespace Lina
{
    class Vector2;

    namespace Graphics
    {
     class Texture;   
    }
}
namespace Lina::Editor
{
    class TextureDrawer
    {

    public:
        TextureDrawer()  = default;
        ~TextureDrawer() = default;

        static void DrawTextureSettings(Graphics::Texture*& texture, float paneWidth);
        static void DrawTexture(Graphics::Texture* texture, const Vector2& bgMin, const Vector2& bgMax);

    private:
    };
} // namespace Lina

#endif
