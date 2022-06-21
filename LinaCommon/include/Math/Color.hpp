/*
Class: Color

Represents a data structure for colors.

Timestamp: 10/26/2018 7:19:07 PM

*/

#pragma once

#ifndef Lina_Color_HPP
#define Lina_Color_HPP

namespace Lina
{
    class Color
    {

    public:
        Color(float rv = 1.0f, float gv = 1.0f, float bv = 1.0f, float av = 1.0f, bool is255 = false) : r(is255 ? rv / 255.0f : rv), g(is255 ? gv / 255.0f : gv), b(is255 ? bv / 255.0f : bv), a(is255 ? av / 255.0f : av){};
        
        float r, g, b, a = 1.0f;

        bool operator!=(const Color& rhs) const
        {
            return !(r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
        }

        bool operator==(const Color& rhs) const
        {
            return (r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a);
        }

        Color operator*(const float& rhs) const
        {
            return Color(r * rhs, g * rhs, b * rhs, a * rhs);
        }

    public:
        static Color Red;
        static Color Green;
        static Color LightBlue;
        static Color Blue;
        static Color DarkBlue;
        static Color Cyan;
        static Color Yellow;
        static Color Black;
        static Color White;
        static Color Purple;
        static Color Maroon;
        static Color Beige;
        static Color Brown;
        static Color Gray;

        template <class Archive> void serialize(Archive& archive)
        {
            archive(r, g, b, a); // serialize things by passing them to the archive
        }
    };

} // namespace Lina

#endif
