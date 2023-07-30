/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

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
Class: Common



Timestamp: 3/26/2022 10:36:46 AM
*/

#pragma once

#ifndef LinaVGCommon_HPP
#define LinaVGCommon_HPP

// Headers here.
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace LinaVG
{
#define LINAVG_STRING  std::string
#define LINAVG_MAP     std::unordered_map
#define LINAVG_VEC     std::vector
#define LINAVG_MEMCPY  std::memcpy
#define LINAVG_MEMMOVE std::memmove
#define LINAVG_MALLOC  std::malloc
#define LINAVG_FREE    std::free
#define LVG_RAD2DEG    57.2957f
#define LVG_DEG2RAD    0.0174533f
#define LINAVG_API     // TODO

    typedef unsigned int Index;
    typedef unsigned int BackendHandle;
    class LinaVGFont;

    /// <summary>
    /// Color vector, range 0.0f - 1.0f
    /// </summary>
    LINAVG_API struct Vec4
    {
        Vec4(){};
        Vec4(float x, float y, float z, float w)
            : x(x), y(y), z(z), w(w){};
        Vec4(const Vec4& v)
        {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            this->w = v.w;
        }

        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;
    };

    LINAVG_API enum class GradientType
    {
        Horizontal   = 0,
        Vertical     = 1,
        Radial       = 2,
        RadialCorner = 3
    };

    LINAVG_API struct Vec4Grad
    {
        Vec4Grad(){};
        Vec4Grad(const Vec4& c1)
            : start(c1), end(c1){};

        Vec4Grad(const Vec4& c1, const Vec4& c2)
            : start(c1), end(c2){};

        Vec4         start        = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
        Vec4         end          = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
        GradientType gradientType = GradientType::Horizontal;
        float        radialSize   = 1.0f;
    };

    LINAVG_API struct Vec2
    {
        Vec2(){};
        Vec2(float x, float y)
            : x(x), y(y){};
        Vec2(const Vec2& v)
        {
            x = v.x;
            y = v.y;
        }

        float x = 0.0f;
        float y = 0.0f;
    };

    LINAVG_API struct ThicknessGrad
    {
        ThicknessGrad(){};
        ThicknessGrad(float start)
            : start(start), end(start){};
        ThicknessGrad(float start, float end)
            : start(start), end(end){};

        float start = 1.0f;
        float end   = 1.0f;
    };

    typedef float Thickness;

    /// <summary>
    /// Custom array for fast-handling vertex & index buffers for vector drawing operations.
    /// Inspired by Dear ImGui's ImVector
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename T>
    class Array
    {
    public:
        T*                        m_data     = nullptr;
        int                       m_size     = 0;
        int                       m_lastSize = 0;
        int                       m_capacity = 0;
        typedef T                 value_type;
        typedef value_type*       iterator;
        typedef const value_type* const_iterator;
        Array()
        {
            m_size = m_capacity = m_lastSize = 0;
            m_data                           = nullptr;
        }

        Array(const Array<T>& other)
        {
            resize(other.m_size);
            LINAVG_MEMCPY(m_data, other.m_data, size_t(other.m_size) * sizeof(T));
        }

        // inline Array(const Array<T>& src)
        // {
        //     m_size = m_capacity = m_lastSize = 0;
        //     m_data = NULL;
        //     operator        =(src);
        // }

        inline Array<T>& operator=(const Array<T>& other)
        {
            clear();
            resize(other.m_size);
            LINAVG_MEMCPY(m_data, other.m_data, size_t(other.m_size) * sizeof(T));
            return *this;
        }
        ~Array()
        {
            clear();
        }

        inline void from(const Array& t)
        {
            clear();
            for (int i = 0; i < t.m_size; i++)
                push_back(t.m_data[i]);
        }

        inline void clear()
        {
            if (m_data)
            {
                m_size = m_capacity = m_lastSize = 0;
                LINAVG_FREE(m_data);
                m_data = nullptr;
            }
        }

        inline bool isEmpty()
        {
            return m_size == 0;
        }

        inline int sizeBytes() const
        {
            return m_size * (int)sizeof(T);
        }

        inline T* begin()
        {
            return m_data;
        }

        inline T* end()
        {
            return m_data + m_size;
        }

        inline int growCapacity(int sz) const
        {
            const int newCapacity = m_capacity ? (m_capacity + m_capacity / 2) : 8;
            return newCapacity > sz ? newCapacity : sz;
        }

        inline void resize(int newSize)
        {
            if (newSize > m_capacity)
                reserve(growCapacity(newSize));
            m_size = newSize;
        }

        inline void resize(int newSize, const T& v)
        {
            if (newSize > m_capacity)
                reserve(growCapacity(newSize));
            if (newSize > m_size)
                for (int n = m_size; n < newSize; n++)
                    LINAVG_MEMCPY(&m_data[n], &v, sizeof(v));
            m_size = newSize;
        }

        inline void shrink(int size)
        {
            _ASSERT(size <= m_size);
            m_size = size;
        }

        inline void reserve(int newCapacity)
        {
            if (newCapacity < m_capacity)
                return;
            T* newData = (T*)LINAVG_MALLOC((size_t)newCapacity * sizeof(T));

            if (m_data)
            {
                if (newData != 0)
                    LINAVG_MEMCPY(newData, m_data, (size_t)m_size * sizeof(T));
                LINAVG_FREE(m_data);
            }
            m_data     = newData;
            m_capacity = newCapacity;
        }

        inline void checkGrow()
        {
            if (m_size == m_capacity)
                reserve(growCapacity(m_size + 1));
        }

        inline T* push_back(const T& v)
        {
            checkGrow();
            auto s = sizeof(v);
            LINAVG_MEMCPY(&m_data[m_size], &v, s);
            m_size++;
            return last();
        }

        inline T* erase(const T* it)
        {
            _ASSERT(it >= m_data && it < m_data + m_size);
            const ptrdiff_t off = it - m_data;
            std::memmove(m_data + off, m_data + off + 1, ((size_t)m_size - (size_t)off - 1) * sizeof(T));
            m_size--;
            return m_data + off;
        }

        inline T* last()
        {
            return &m_data[m_size - 1];
        }

        inline T& last_ref()
        {
            return m_data[m_size - 1];
        }

        inline T& operator[](int i)
        {
            _ASSERT(i >= 0 && i < m_capacity);
            return m_data[i];
        }

        inline const T& operator[](int i) const
        {
            _ASSERT(i >= 0 && i < m_capacity);
            return m_data[i];
        }

        inline int findIndex(const T& t) const
        {
            for (int i = 0; i < m_size; i++)
            {
                if (m_data[i] == t)
                    return i;
            }

            return -1;
        }

        inline T* findAddr(const T& v)
        {
            T*       data     = m_data;
            const T* data_end = m_data + m_size;
            while (data < data_end)
                if (*data == v)
                    break;
                else
                    ++data;
            return data;
        }

        inline void swap(int start, int end)
        {
            _ASSERT(start > -1 && start < m_size && end > -1 && end < m_size);
            T temp        = m_data[start];
            m_data[start] = m_data[end];
            m_data[end]   = temp;
        }
    };

    LINAVG_API enum class OutlineDrawDirection
    {
        Outwards,
        Inwards,
        Both
    };

    struct StyleOptions;

    LINAVG_API struct OutlineOptions
    {
        static OutlineOptions FromStyle(const StyleOptions& opts, OutlineDrawDirection drawDir);

        /// <summary>
        /// Outline m_thickness.
        /// </summary>
        float thickness = 0.0f;

        /// <summary>
        /// Determines where to draw the outline, has no effect on filled shapes and lines.
        /// </summary>
        OutlineDrawDirection drawDirection = OutlineDrawDirection::Outwards;

        /// <summary>
        /// Outline m_color, you can set this to 2 different colors & define a gradient type, or construct with a single m_color for flat shading.
        /// </summary>
        Vec4Grad color = Vec4Grad(Vec4(1, 1, 1, 1));

        /// <summary>
        /// Set this to a texture handle you've created on your program to draw a texture on the outline. Set to 0 to clear.
        /// </summary>
        BackendHandle textureHandle = 0;

        /// <summary>
        /// Defines the texture repetition.
        /// </summary>
        Vec2 textureUVTiling = Vec2(1.0f, 1.0f);

        /// <summary>
        /// Defines the texture offset.
        /// </summary>
        Vec2 textureUVOffset = Vec2(0.0f, 0.0f);
    };

    enum class TextAlignment
    {
        Left = 0,
        Center,
        Right
    };

    struct CharacterInfo
    {
        float x     = 0.0f;
        float y     = 0.0f;
        float sizeX = 0.0f;
        float sizeY = 0.0f;
    };

    LINAVG_API struct LineInfo
    {
        /// <summary>
        /// First character index of the line that corresponds to a character in TextOutData.characterInfo
        /// </summary>
        unsigned int startCharacterIndex = 0;

        /// <summary>
        /// Last character index of the line that corresponds to a character in TextOutData.characterInfo
        /// </summary>
        unsigned int endCharacterIndex = 0;

        /// <summary>
        /// Start position x of this line.
        /// </summary>
        float posX = 0.0f;

        /// <summary>
        /// Start position y of this line.
        /// </summary>
        float posY = 0.0f;
    };

    LINAVG_API struct TextOutData
    {
        /// <summary>
        /// Upon drawing a text, this vector contains position and size information for each character.
        /// </summary>
        Array<CharacterInfo> characterInfo;

        /// <summary>
        /// If wrapped text, contains information about each line, empty if not wrapped.
        /// </summary>
        Array<LineInfo> lineInfo;

        void Clear()
        {
            characterInfo.clear();
            lineInfo.clear();
        }

        void Shrink()
        {
            characterInfo.shrink(0);
            lineInfo.shrink(0);
        }
    };

    /// <summary>
    /// Text styling, DrawText will render the given text as normal or via signed-distance-field (SDF) methods.
    /// This depends on the font handle given with options (or default font if not-provided).
    /// If you DrawText() using a font handle which was generated with SDF option, it's gonna use SDF rendering.
    /// m_thickness, softness, drop shadow and outline options are only available on SDF rendering.
    /// </summary>
    LINAVG_API struct TextOptions
    {
        TextOptions(){};
        TextOptions(const TextOptions& opts)
        {
            font             = opts.font;
            color            = opts.color;
            textScale        = opts.textScale;
            dropShadowColor  = opts.dropShadowColor;
            dropShadowOffset = opts.dropShadowOffset;
            alignment        = opts.alignment;
            spacing          = opts.spacing;
            newLineSpacing   = opts.newLineSpacing;
            wrapWidth        = opts.wrapWidth;
            framebufferScale = opts.framebufferScale;
        }

        bool CheckColors(const Vec4& c1, const Vec4& c2)
        {
            return c1.x == c2.x && c1.y == c2.y && c1.z == c2.z && c1.w == c2.w;
        }

        bool IsSame(const TextOptions& opts)
        {
            if (font != opts.font)
                return false;

            if (color.gradientType != opts.color.gradientType)
                return false;

            if (color.gradientType == GradientType::Radial || color.gradientType == GradientType::RadialCorner)
            {
                if (color.radialSize != opts.color.radialSize)
                    return false;
            }

            if (!CheckColors(color.start, color.end))
                return false;

            if (dropShadowOffset.x != 0.0f || dropShadowOffset.y != 0.0f)
            {
                if (!CheckColors(dropShadowColor, opts.dropShadowColor))
                    return false;
            }

            return alignment == opts.alignment && textScale == opts.textScale && spacing == opts.spacing && newLineSpacing == opts.newLineSpacing && wrapWidth == opts.wrapWidth;
        }

        /// <summary>
        /// Font to use while drawing this text. Handles are achieved through LoadFont() method.
        /// </summary>
        LinaVGFont* font = nullptr;

        /// <summary>
        /// Text m_color, only flat m_color, horizontal or vertical gradients are supported.
        /// </summary>
        Vec4Grad color = Vec4Grad(Vec4(1.0f, 1.0f, 1.0f, 1.0f));

        /// <summary>
        /// Text alignment.
        /// </summary>
        TextAlignment alignment = TextAlignment::Left;

        /// <summary>
        /// Multiplies the text vertices, !it is not recommended to change text size based on this scale!
        /// Rather try to load the same font with bigger sizes.
        /// </summary>
        float textScale = 1.0f;

        /// <summary>
        /// Used as an additional scale on outline and AA thickness.
        /// Normally you can set this to whatever your OS' display scale is, e.g. OS scaling factor on high-dpi monitors.
        /// </summary>
        float framebufferScale = 1.0f;

        /// <summary>
        /// Defines extra spacing between each letter.
        /// </summary>
        float spacing = 0.0f;

        /// <summary>
        /// Spacing used if the text is word-wrapped and dropped to a new line.
        /// </summary>
        float newLineSpacing = 5.0f;

        /// <summary>
        /// Text will wrap at, e.g. go to a new line when it reaches = position.x + wrapWidth
        /// </summary>
        float wrapWidth = 0.0f;

        /// <summary>
        /// Drop shadow m_color, lol.
        /// </summary>
        Vec4 dropShadowColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

        /// <summary>
        /// Defines how far the drop shadow is rendered, in screen-units.
        /// Set to 0.0f, 0.0f to disable drop-shadows.
        /// </summary>
        Vec2 dropShadowOffset = Vec2(0.0f, 0.0f);
    };

    LINAVG_API struct SDFTextOptions : public TextOptions
    {
        SDFTextOptions()
            : TextOptions(){};
        SDFTextOptions(const SDFTextOptions& opts)
        {
            font                   = opts.font;
            color                  = opts.color;
            textScale              = opts.textScale;
            dropShadowColor        = opts.dropShadowColor;
            dropShadowOffset       = opts.dropShadowOffset;
            sdfDropShadowThickness = opts.sdfDropShadowThickness;
            sdfOutlineColor        = opts.sdfOutlineColor;
            sdfOutlineThickness    = opts.sdfOutlineThickness;
            sdfSoftness            = opts.sdfSoftness;
            sdfThickness           = opts.sdfThickness;
            sdfDropShadowSoftness  = opts.sdfDropShadowSoftness;
            framebufferScale       = opts.framebufferScale;
        }

        /// <summary>
        /// 0.0f - 1.0f range, defines how strongly the text is extruded.
        /// !NOTE! 0.5 is default m_thickness, e.g. renders the text like no SDF. 0.0 goes to invisible, while 1.0 gets closer to max
        /// m_thickness.
        /// </summary>
        float sdfThickness = 0.5f;

        /// <summary>
        /// Defines text blurring/smoothing. Usually 0.0f - 1.0f is a good range.
        /// </summary>
        float sdfSoftness = 0.2f;

        /// <summary>
        /// 0.0f - 1.0f range, defines how strongly the outline is extruded.
        /// </summary>
        float sdfOutlineThickness = 0.0f;

        /// <summary>
        /// Well, outline m_color.
        /// </summary>
        Vec4 sdfOutlineColor = Vec4(1, 1, 1, 1);

        /// <summary>
        /// 0.0f - 1.0f range, defines how strongly the drop shadow is extruded.
        /// </summary>
        float sdfDropShadowThickness = 0.0f;

        /// <summary>
        /// Defines drop shadow blurring/smoothing. 0.0f - 1.0f range
        /// </summary>
        float sdfDropShadowSoftness = 0.02f;

        /// <summary>
        /// Flips the alpha mask in-out, can be used to create create cut-out font rendering, e.g. letter is transparent, surrounding
        /// quad is colored.
        /// </summary>
        bool flipAlpha = false;
    };
    /// <summary>
    /// Style options used to draw various effects around the target shape.
    /// </summary>
    LINAVG_API struct StyleOptions
    {

        StyleOptions(){};
        StyleOptions(const StyleOptions& opts)
        {
            color            = opts.color;
            thickness        = opts.thickness;
            rounding         = opts.rounding;
            aaMultiplier     = opts.aaMultiplier;
            framebufferScale = opts.framebufferScale;

            onlyRoundTheseCorners.from(opts.onlyRoundTheseCorners);
            outlineOptions  = opts.outlineOptions;
            textureHandle   = opts.textureHandle;
            textureUVTiling = opts.textureUVTiling;
            textureUVOffset = opts.textureUVOffset;
            isFilled        = opts.isFilled;
            aaEnabled       = opts.aaEnabled;
        }

        /// <summary>
        /// Color for the shape, you can set this to 2 different colors & define a gradient type, or construct with a single m_color for flat shading.
        /// </summary>
        Vec4Grad color = Vec4Grad(Vec4(1, 1, 1, 1));

        /// <summary>
        /// While drawing lines -> can make a straight line or a line with varying m_thickness based on start & end (only for single line API, e.g. DrawLine()).
        /// While drawing non-filled shapes -> only start m_thickness is used.
        /// While drawing filled shapes, this has no effect.
        /// </summary>
        ThicknessGrad thickness = ThicknessGrad(1.0f);

        /// <summary>
        /// Always pass between 0.0f and 1.0f
        /// Used for:
        /// - Rounding the corners of the shapes, e.g. rect, triangle
        /// - Line caps rounding
        /// - Line joints rounding
        /// </summary>
        float rounding = 0.0f;

        /// <summary>
        /// Enable/disable AA outlines for this shape.
        /// </summary>
        bool aaEnabled = false;

        /// <summary>
        /// Antialiasing multiplier for the shapes drawn with this style options.
        /// </summary>
        float aaMultiplier = 1.0f;

        /// <summary>
        /// Used as an additional scale on outline and AA thickness.
        /// Normally you can set this to whatever your OS' display scale is, e.g. OS scaling factor on high-dpi monitors.
        /// </summary>
        float framebufferScale = 1.0f;

        /// <summary>
        /// If rounding is to be applied, you can fill this array to only apply rounding to specific corners of the shape (only for shapes, not lines).
        /// </summary>
        Array<int> onlyRoundTheseCorners;

        /// <summary>
        /// Outline details.
        /// </summary>
        OutlineOptions outlineOptions;

        /// <summary>
        /// Set this to a texture handle you've created on your program to draw a texture on the shape/line. Set to 0 to clear.
        /// </summary>
        BackendHandle textureHandle = 0;

        /// <summary>
        /// Defines the texture repetition.
        /// </summary>
        Vec2 textureUVTiling = Vec2(1.0f, 1.0f);

        /// <summary>
        /// Defines the texture offset.
        /// </summary>
        Vec2 textureUVOffset = Vec2(0.0f, 0.0f);

        /// <summary>
        /// Fills inside the target shape, e.g. rect, tris, convex, circles, ngons, has no effect on lines.
        /// </summary>
        bool isFilled = true;
    };

    struct Vertex
    {
        Vec2 pos;
        Vec2 uv;
        Vec4 col;
    };

    LINAVG_API struct Configuration
    {
        /// <summary>
        /// Used as an additional scale on outline and AA thickness.
        /// Normally you can set this to whatever your OS' display scale is, e.g. OS scaling factor on high-dpi monitors.
        /// All Text and Style options also have their own/local framebuffer scale, which is multiplied by this value.
        /// </summary>
        float globalFramebufferScale = 1.0f;

        /// <summary>
        /// Used as an additional scale on AA thickness.
        /// All Style options also have their own/local framebuffer scale, which is multiplied by this value.
        /// </summary>
        float globalAAMultiplier = 1.0f;

        /// <summary>
        /// If the angle between two lines exceed this limit fall-back to bevel joints from miter joints.
        /// This is because miter joins the line points on intersection, ang with a very small angle (closer to 180) intersections get close to infinity.
        /// </summary>
        float miterLimit = 150;

        /// <summary>
        /// Maximum size a font texture atlas can have, all atlasses are square, so this is used for both width and height.
        /// Increase if you are loading a lot of characters or fonts with big sizes (e.g. 100)
        /// You can use Internal::DrawDebugFontAtlas to visualize the atlas target font belongs to.
        /// </summary>
        unsigned int maxFontAtlasSize = 768;

        /// <summary>
        /// Every interval ticks system will garbage collect all vertex and index buffers, meaning that will clear all the arrays.
        /// On other ticks, arrays are simply resized to 0, avoiding re-allocations on the next frame.
        /// Set to 0 for instant flush on buffers at the end of every frame.
        /// </summary>
        int gcCollectInterval = 600;

        /// <summary>
        /// This amount of default buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// </summary>
        int defaultBufferReserve = 10;

        /// <summary>
        /// This amount of gradient buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// </summary>
        int gradientBufferReserve = 5;

        /// <summary>
        /// This amount of texture buffers are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// </summary>
        int textureBufferReserve = 5;

        /// <summary>
        /// This amount of text buffers for text rendering are reserved upon Renderer initialization. Saves time from allocating/deallocating buffers in runtime.
        /// General idea is that each font you load create a new buffer, so you can reserve the same amount of fonts you plan on using on your application.
        /// </summary>
        int textBuffersReserve = 10;

        /// <summary>
        /// Set this to your own function to receive error callbacks from LinaVG.
        /// </summary>
        std::function<void(const LINAVG_STRING&)> errorCallback;

        /// <summary>
        /// Set this to your own function to receive log callbacks from LinaVG.
        /// </summary>
        std::function<void(const LINAVG_STRING&)> logCallback;

        /// <summary>
        /// For debugging purposes, sets to draw polygon/wireframe mode.
        /// Unused while using custom backends, you can fill with your own data.
        /// </summary>
        bool debugWireframeEnabled = false;

        /// <summary>
        /// For debugging purposes, current count of the trianlges being drawn.
        /// Unused while using custom backends, you can fill with your own data.
        /// </summary>
        int debugCurrentTriangleCount = 0;

        /// <summary>
        /// For debugging purposes, current count of the vertices sent to backend buffers.
        /// Unused while using custom backends, you can fill with your own data.
        /// </summary>
        int debugCurrentVertexCount = 0;

        /// <summary>
        /// For debugging purposes, current draw calls.
        /// Unused while using custom backends, you can fill with your own data.
        /// </summary>
        int debugCurrentDrawCalls = 0;

        /// <summary>
        /// For debugging purposes, zooms the rendering ortho projection.
        /// Unused while using custom backends, you can fill with your own data.
        /// </summary>
        float debugOrthoProjectionZoom = 1.0f;

        /// <summary>
        /// For debugging purposes, offsets the rendering ortho projection.
        /// Unused while using custom backends, you can fill with your own data.
        /// </summary>
        Vec2 debugOrthoOffset = Vec2(0.0f, 0.0f);

        /// <summary>
        /// Enabling caching allows faster text rendering in exchange for more memory consumption.
        /// On average, more than 200-300 DrawTextNormal commands per frame is a valid reason to enable caching.
        /// </summary>
        bool textCachingEnabled = false;

        /// <summary>
        /// Initial reserve for normal text cache, will grow if needed.
        /// </summary>
        int textCacheReserve = 300;

        /// <summary>
        /// Enabling caching allows faster text rendering in exchange for more memory consumption.
        /// Caching on SDF texts are usually unnecessary since amount of SDF texts rendered shouldn't be too much.
        /// </summary>
        bool textCachingSDFEnabled = false;

        /// <summary>
        /// Initial reserve for SDF text cache, will grow if needed.
        /// </summary>
        int textCacheSDFReserve = 20;

        /// <summary>
        /// Every this amount of ticks the text caches will be cleared up to prevent memory bloating.
        /// </summary>
        int textCacheExpireInterval = 3000;
    };

    /// <summary>
    /// Main configurations for LinaVG API, contains settings for debug options, line joint angles and AA.
    /// </summary>
    extern LINAVG_API Configuration Config;

    enum class DrawBufferType
    {
        Default,
        Gradient,
        Textured,
        SimpleText,
        SDFText,
    };

    enum class DrawBufferShapeType
    {
        DropShadow,
        Shape,
        Outline,
        AA,
    };

    struct DrawBuffer
    {
        DrawBuffer(){};
        DrawBuffer(int drawOrder, DrawBufferType type, DrawBufferShapeType shapeType, BackendHandle clipPosX, BackendHandle clipPosY, BackendHandle clipSizeX, BackendHandle clipSizeY)
            : m_drawOrder(drawOrder), m_drawBufferType(type), m_shapeType(shapeType)
        {
            this->clipPosX  = clipPosX;
            this->clipPosY  = clipPosY;
            this->clipSizeX = clipSizeX;
            this->clipSizeY = clipSizeY;
        };

        Array<Vertex>       m_vertexBuffer;
        Array<Index>        m_indexBuffer;
        int                 m_drawOrder      = -1;
        DrawBufferType      m_drawBufferType = DrawBufferType::Default;
        DrawBufferShapeType m_shapeType      = DrawBufferShapeType::Shape;
        BackendHandle       clipPosX         = 0;
        BackendHandle       clipPosY         = 0;
        BackendHandle       clipSizeX        = 0;
        BackendHandle       clipSizeY        = 0;

        bool IsClipDifferent(BackendHandle clipPosX, BackendHandle clipPosY, BackendHandle clipSizeX, BackendHandle clipSizeY)
        {
            return (this->clipPosX != clipPosX || this->clipPosY != clipPosY || this->clipSizeX != clipSizeX || this->clipSizeY != clipSizeY);
        }

        inline void Clear()
        {
            m_vertexBuffer.clear();
            m_indexBuffer.clear();
        }

        inline void ShrinkZero()
        {
            m_vertexBuffer.shrink(0);
            m_indexBuffer.shrink(0);
        }

        inline void PushVertex(const Vertex& v)
        {
            m_vertexBuffer.push_back(v);
        }

        inline void PushIndex(Index i)
        {
            m_indexBuffer.push_back(i);
        }

        inline Vertex* LastVertex()
        {
            return m_vertexBuffer.last();
        }
    };

    struct GradientDrawBuffer : public DrawBuffer
    {
        GradientDrawBuffer(){};
        GradientDrawBuffer(const Vec4Grad& g, int drawOrder, DrawBufferShapeType shapeType, BackendHandle clipPosX, BackendHandle clipPosY, BackendHandle clipSizeX, BackendHandle clipSizeY)
            : m_isAABuffer(shapeType == DrawBufferShapeType::AA), m_color(g), DrawBuffer(drawOrder, DrawBufferType::Gradient, shapeType, clipPosX, clipPosY, clipSizeX, clipSizeY){};

        bool     m_isAABuffer = false;
        Vec4Grad m_color      = Vec4(1, 1, 1, 1);
    };

    struct TextureDrawBuffer : public DrawBuffer
    {
        TextureDrawBuffer(){};
        TextureDrawBuffer(BackendHandle h, const Vec2& tiling, const Vec2& offset, const Vec4& tint, int drawOrder, DrawBufferShapeType shapeType, BackendHandle clipPosX, BackendHandle clipPosY, BackendHandle clipSizeX, BackendHandle clipSizeY)
            : m_isAABuffer(shapeType == DrawBufferShapeType::AA), m_tint(tint), m_textureHandle(h), m_textureUVTiling(tiling), m_textureUVOffset(offset),
              DrawBuffer(drawOrder, DrawBufferType::Textured, shapeType, clipPosX, clipPosY, clipSizeX, clipSizeY){};

        bool          m_isAABuffer      = false;
        BackendHandle m_textureHandle   = 0;
        Vec2          m_textureUVTiling = Vec2(1.0f, 1.0f);
        Vec2          m_textureUVOffset = Vec2(0.0f, 0.0f);
        Vec4          m_tint            = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
    };

    struct SimpleTextDrawBuffer : public DrawBuffer
    {
        SimpleTextDrawBuffer(){};
        SimpleTextDrawBuffer(BackendHandle glyphHandle, int drawOrder, bool isDropShadow, BackendHandle clipPosX, BackendHandle clipPosY, BackendHandle clipSizeX, BackendHandle clipSizeY)
            : m_textureHandle(glyphHandle), m_isDropShadow(isDropShadow),
              DrawBuffer(drawOrder, DrawBufferType::SimpleText, isDropShadow ? DrawBufferShapeType::DropShadow : DrawBufferShapeType::Shape, clipPosX, clipPosY, clipSizeX, clipSizeY){};

        BackendHandle m_textureHandle = 0;
        bool          m_isDropShadow  = false;
    };

    struct SDFTextDrawBuffer : public DrawBuffer
    {
        SDFTextDrawBuffer(){};
        SDFTextDrawBuffer(BackendHandle glyphHandle, int drawOrder, const SDFTextOptions& opts, bool isDropShadow, BackendHandle clipPosX, BackendHandle clipPosY, BackendHandle clipSizeX, BackendHandle clipSizeY)
            : m_textureHandle(glyphHandle), m_thickness(opts.sdfThickness),
              m_softness(opts.sdfSoftness), m_outlineThickness(opts.sdfOutlineThickness),
              m_outlineColor(opts.sdfOutlineColor), m_flipAlpha(opts.flipAlpha), m_isDropShadow(isDropShadow),
              DrawBuffer(drawOrder, DrawBufferType::SDFText, isDropShadow ? DrawBufferShapeType::DropShadow : DrawBufferShapeType::Shape, clipPosX, clipPosY, clipSizeX, clipSizeY){};

        bool          m_isDropShadow     = false;
        bool          m_flipAlpha        = false;
        float         m_thickness        = 0.0f;
        float         m_softness         = 0.0f;
        float         m_outlineThickness = 0.0f;
        Vec4          m_outlineColor     = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
        BackendHandle m_textureHandle    = 0;
    };

} // namespace LinaVG

#endif
