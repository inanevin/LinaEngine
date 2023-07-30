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
Class: Drawer



Timestamp: 3/24/2022 10:57:37 PM
*/

#pragma once

#ifndef LinaVGDrawer_HPP
#define LinaVGDrawer_HPP

// Headers here.
#include "Common.hpp"

namespace LinaVG
{
    class LinaVGFont;

    struct LineTriangle
    {
        int m_indices[3];
    };

    struct TextPart
    {
        LINAVG_STRING m_str  = "";
        Vec2          m_size = Vec2(0.0f, 0.0f);
    };

    struct Line
    {
        Array<Vertex>       m_vertices;
        Array<int>          m_upperIndices;
        Array<int>          m_lowerIndices;
        Array<LineTriangle> m_tris;
        bool                m_hasMidpoints       = false;
        int                 m_lineCapVertexCount = 0;

        Line& operator=(const Line& t)
        {
            m_vertices.m_data = nullptr;
            m_vertices.m_size = m_vertices.m_capacity = m_vertices.m_lastSize = 0;
            m_tris.m_data                                                     = nullptr;
            m_tris.m_size = m_tris.m_capacity = m_tris.m_lastSize = 0;
            m_lowerIndices.m_data                                 = nullptr;
            m_lowerIndices.m_size = m_lowerIndices.m_capacity = m_lowerIndices.m_lastSize = 0;
            m_upperIndices.m_data                                                         = nullptr;
            m_upperIndices.m_size = m_upperIndices.m_capacity = m_upperIndices.m_lastSize = 0;
            m_vertices.from(t.m_vertices);
            m_tris.from(t.m_tris);
            m_lowerIndices.from(t.m_lowerIndices);
            m_upperIndices.from(t.m_upperIndices);
            m_hasMidpoints       = t.m_hasMidpoints;
            m_lineCapVertexCount = t.m_lineCapVertexCount;
            return *this;
        }

        Line() = default;
    };

    struct SimpleLine
    {
        Vec2 m_points[4];
    };

    LINAVG_API enum class LineJointType
    {
        Miter,
        Bevel,
        BevelRound,
        VtxAverage,
    };

    LINAVG_API enum class LineCapDirection
    {
        None,
        Left,
        Right,
        Both
    };

    /// <summary>
    /// Draws a bezier curve defined by the start, end and control points.
    /// </summary>
    /// <param name="p0">Start point.</param>
    /// <param name="p1">Control point 1.</param>
    /// <param name="p2">Control point 2.</param>
    /// <param name="p3">End point.</param>
    /// <param name="style">Style options.</param>
    /// <param name="cap">Puts a line cap to either first, last or both ends.</param>
    /// <param name="jointType">Determines how to join the lines.</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    /// <param name="segments"> Determines the smoothness of the curve. 0-100 range, 100 smoothest. </param>
    LINAVG_API void DrawBezier(int thread, const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, LineJointType jointType = LineJointType::Miter, int drawOrder = 0, int segments = 50);
    LINAVG_API void DrawBezier(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, LineJointType jointType = LineJointType::Miter, int drawOrder = 0, int segments = 50);

    /// <summary>
    /// Draws a single point.
    /// </summary>
    LINAVG_API void DrawPoint(int thread, const Vec2& p1, const Vec4& col);
    LINAVG_API void DrawPoint(const Vec2& p1, const Vec4& col);

    /// <summary>
    /// Draws a line between two points.
    /// </summary>
    /// <param name="p1">Start point.</param>
    /// <param name="p2">End point.</param>
    /// <param name="style">Style options.</param>
    /// <param name="cap">Puts a line cap to either first, last or both ends.</param>
    /// <param name="rotateAngle">Rotates the whole line by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    LINAVG_API void DrawLine(int thread, const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, float rotateAngle = 0.0f, int drawOrder = 0);
    LINAVG_API void DrawLine(const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws multiple lines defined by the given path.
    /// </summary>
    /// <param name="points">Line path, needs to be at least 3 points.</param>
    /// <param name="count">Total number of points in the given path.</param>
    /// <param name="style">Style options.</param>
    /// <param name="cap">Puts a line cap to either first, last or both ends.</param>
    /// <param name="jointType">Determines how to join the lines.</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    LINAVG_API void DrawLines(int thread, Vec2* points, int count, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, LineJointType jointType = LineJointType::Miter, int drawOrder = 0);
    LINAVG_API void DrawLines(Vec2* points, int count, StyleOptions& style, LineCapDirection cap = LineCapDirection::None, LineJointType jointType = LineJointType::Miter, int drawOrder = 0);

    /// <summary>
    /// Draws a texture in the given pos.
    /// </summary>
    /// <param name="textureHandle">Texture handle, should be a valid texture loaded into the target backend device.</param>
    /// <param name="pos">Center position of the draw rect.</param>
    /// <param name="size">Size of the draw rect, should be same aspect ratio to prevent stretching.</param>
    /// <param name="rotateAngle">Rotates the whole image by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    /// <param name="uvTiling">Texture tiling.</param>
    /// <param name="uvOffset">Texture UV offset.</param>
    /// <param name="uvTL">Top-left UV coordinates, default is top-left to bottom-right, 0,0 to 1,1.</param>
    /// <param name="uvBR">Bottom-right UV coordinates, default is top-left to bottom-right, 0,0 to 1,1.</param>
    LINAVG_API void DrawImage(int thread, BackendHandle textureHandle, const Vec2& pos, const Vec2& size, Vec4 tint = Vec4(1, 1, 1, 1), float rotateAngle = 0.0f, int drawOrder = 0, Vec2 uvTiling = Vec2(1, 1), Vec2 uvOffset = Vec2(0, 0), Vec2 uvTL = Vec2(0, 0), Vec2 uvBR = Vec2(1, 1));
    LINAVG_API void DrawImage(BackendHandle textureHandle, const Vec2& pos, const Vec2& size, Vec4 tint = Vec4(1, 1, 1, 1), float rotateAngle = 0.0f, int drawOrder = 0, Vec2 uvTiling = Vec2(1, 1), Vec2 uvOffset = Vec2(0, 0), Vec2 uvTL = Vec2(0, 0), Vec2 uvBR = Vec2(1, 1));

    /// <summary>
    /// Your points for the triangle must follow the given parameter order -- left, right and top edges.
    /// If you are drawing odd triangles, e.g. maxAngle > 90, rounding of the triangle might perform poorly on sharp edges.
    /// </summary>
    /// <param name="top"> Top corner. </param>
    /// <param name="right"> Bottom right corner. </param>
    /// <param name="left"> Bottom left corner. </param>
    /// <param name="style">Style options.</param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    LINAVG_API void DrawTriangle(int thread, const Vec2& top, const Vec2& right, const Vec2& left, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);
    LINAVG_API void DrawTriangle(const Vec2& top, const Vec2& right, const Vec2& left, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a filled rectangle between min and max with the given style options and rotation angle.
    /// </summary>
    /// <param name="min">Top left corner of the rectangle.</param>
    /// <param name="max">Bottom right corner of the rectangle.</param>
    /// <param name="style">Style options.</param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    LINAVG_API void DrawRect(int thread, const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);
    LINAVG_API void DrawRect(const Vec2& min, const Vec2& max, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a convex polygon with N corners. !Rounding options do not apply to NGons!
    /// </summary>
    /// <param name="center">Shape center.</param>
    /// <param name="radius">Shape radius.</param>
    /// <param name="n">Segment count.</param>
    /// <param name="style">Style options.</param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    LINAVG_API void DrawNGon(int thread, const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);
    LINAVG_API void DrawNGon(const Vec2& center, float radius, int n, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a convex shape defined by the set of points. All points must be unique.
    /// </summary>
    /// <param name="points">Path to follow while drawing the shape. It's users' responsibility to ensure this is a convex shape.</param>
    /// <param name="size">Total number of given points.</param>
    /// <param name="style">Style options.</param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    LINAVG_API void DrawConvex(int thread, Vec2* points, int size, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);
    LINAVG_API void DrawConvex(Vec2* points, int size, StyleOptions& style, float rotateAngle = 0.0f, int drawOrder = 0);

    /// <summary>
    /// Draws a filled circle with the given radius and center.
    /// You can change the start and end angles to create a filled semi-circle or a filled arc.
    /// It's recommended to send angles always in the range of 0.0f - 360.0f, clock-wise. This method will try to auto-convert if not.
    /// !Rounding options have no effect.!
    /// </summary>
    /// <param name="center">Center of the shape.</param>
    /// <param name="radius">Radius of the shape.</param>
    /// <param name="style">Style options.</param>
    /// <param name="segments">Defines the smoothness of the circle, default 36. Clamped to 6-360, 360 being perfect circle. Higher the segment cause more weight on the performance. 18-54 is a good range for balance. Always recommended to use segments that leave no remainder when 360 is divided by it. </param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="startAngle">Use start and end angle to draw semi-circles or arcs. Leave empty (0.0f, 360.0f) for complete circles.</param>
    /// <param name="endAngle">Use start and end angle to draw semi-circles or arcs. Leave empty (0.0f, 360.0f) for complete circles.</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    /// <returns></returns>
    LINAVG_API void DrawCircle(int thread, const Vec2& center, float radius, StyleOptions& style, int segments = 36, float rotateAngle = 0.0f, float startAngle = 0.0f, float endAngle = 360.0f, int drawOrder = 0);
    LINAVG_API void DrawCircle(const Vec2& center, float radius, StyleOptions& style, int segments = 36, float rotateAngle = 0.0f, float startAngle = 0.0f, float endAngle = 360.0f, int drawOrder = 0);

#ifdef LINAVG_TEXT_SUPPORT

    /// <summary>
    /// Draws the given text at position. The font used in Text Options must be loaded as normal font, e.g. non-sdf.
    /// Given position will be upper-left corner of the text. Use CalculateTextSize to offset the text, e.g. center.
    /// </summary>
    /// <param name="text">Text to draw.</param>
    /// <param name="position">Screen-space position.</param>
    /// <param name="style">Style options.</param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    /// <param name="skipCache">Even if text caching is enabled globally, setting this to true will skip it for this call. Best used for dynamically changing text such as number counters. </param>
    /// <param name="outData"> Fill character related information if not nullptr. </param>
    /// <returns></returns>
    LINAVG_API void DrawTextNormal(int thread, const char* text, const Vec2& position, const TextOptions& opts, float rotateAngle = 0.0f, int drawOrder = 0, bool skipCache = false, TextOutData* outData = nullptr);
    LINAVG_API void DrawTextNormal(const char* text, const Vec2& position, const TextOptions& opts, float rotateAngle = 0.0f, int drawOrder = 0, bool skipCache = false, TextOutData* outData = nullptr);

    /// <summary>
    /// Draws the given text at position as an SDF text, which produces a lot more high-quality results than normal text, regardless
    /// of scaling & rotation. Also SDF texts has a lot more styling options, such as outlines and more paramaterized drop shadows.
    /// The font used in Text Options must be loaded as an SDF-font.
    /// Given position will be upper-left corner of the text. Use CalculateTextSize to offset the text, e.g. center.
    /// </summary>
    /// <param name="text">Text to draw.</param>
    /// <param name="position">Screen-space position.</param>
    /// <param name="style">Style options.</param>
    /// <param name="rotateAngle">Rotates the whole shape by the given angle (degrees).</param>
    /// <param name="drawOrder">Shapes with lower draw order is drawn first, resulting at the very bottom Z layer.</param>
    /// <param name="skipCache">Even if text caching is enabled globally, setting this to true will skip it for this call. Best used for dynamically changing text such as number counters. </param>
    /// <param name="outData"> Fill character related information if not nullptr. </param>
    /// <returns></returns>
    LINAVG_API void DrawTextSDF(int thread, const char* text, const Vec2& position, const SDFTextOptions& opts, float rotateAngle = 0.0f, int drawOrder = 0, bool skipCache = false, TextOutData* outData = nullptr);
    LINAVG_API void DrawTextSDF(const char* text, const Vec2& position, const SDFTextOptions& opts, float rotateAngle = 0.0f, int drawOrder = 0, bool skipCache = false, TextOutData* outData = nullptr);

    /// <summary>
    /// Returns a Vec2 containing max width and height this text will occupy.
    /// Takes spacing and wrapping into account.
    /// </summary>
    /// <param name="text">Text, lol.</param>
    /// <param name="opts">Style options used to draw the text.</param>
    /// <returns></returns>
    LINAVG_API Vec2 CalculateTextSize(int thread, const char* text, TextOptions& opts);
    LINAVG_API Vec2 CalculateTextSize(const char* text, TextOptions& opts);

    /// <summary>
    /// Returns a Vec2 containing max width and height this text will occupy.
    /// Takes spacing and wrapping into account.
    /// </summary>
    /// <param name="text">Text, lol.</param>
    /// <param name="opts">Style options used to draw the text.</param>
    /// <returns></returns>
    LINAVG_API Vec2 CalculateTextSize(int thread, const char* text, SDFTextOptions& opts);
    LINAVG_API Vec2 CalculateTextSize(const char* text, SDFTextOptions& opts);

#endif

    namespace Internal
    {
        enum class OutlineCallType
        {
            Normal,
            AA,
            OutlineAA
        };

        // No rounding, vertical or horizontal gradient
        void FillRect_NoRound_VerHorGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& colorTL, const Vec4& colorTR, const Vec4& colorBR, const Vec4& colorBL, StyleOptions& opts, int drawOrder);

        // No rounding, single color
        void FillRect_NoRound_SC(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& color, StyleOptions& opts, int drawOrder);

        // No rounding, radial gradient
        void FillRect_NoRound_RadialGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& startcolor, const Vec4& endColor, StyleOptions& opts, int drawOrder);

        // Rounding
        void FillRect_Round(int thread, DrawBuffer* buf, Array<int>& roundedCorners, float rotateAngle, const Vec2& min, const Vec2& max, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder);

        // Fill rect impl.
        void FillRectData(int thread, Vertex* vertArray, bool hasCenter, const Vec2& min, const Vec2& max);

        // No rounding, vertical or horizontal gradient
        void FillTri_NoRound_VerHorGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& colorLeft, const Vec4& colorRight, const Vec4& colorTop, StyleOptions& opts, int drawOrder);

        // No rounding, single color
        void FillTri_NoRound_SC(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& color, StyleOptions& opts, int drawOrder);

        // No rounding, radial gradient
        void FillTri_NoRound_RadialGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& startColor, const Vec4& endColor, StyleOptions& opts, int drawOrder);

        // Rounding
        void FillTri_Round(int thread, DrawBuffer* buf, Array<int>& onlyRoundCorners, float rotateAngle, const Vec2& p3, const Vec2& p2, const Vec2& p1, const Vec4& col, float rounding, StyleOptions& opts, int drawOrder);

        // Fill rect impl.
        void FillTriData(int thread, Vertex* vertArray, bool hasCenter, bool calculateUV, const Vec2& p1, const Vec2& p2, const Vec2& p3);

        // No rounding, single color
        void FillNGon_SC(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& color, StyleOptions& opts, int drawOrder);

        // No rounding, horizontal or vertical gradient
        void FillNGon_VerHorGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder);

        // No rounding, radial gradient
        void FillNGon_RadialGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int n, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder);

        // Fill NGon imp
        void FillNGonData(int thread, Array<Vertex>&, bool hasCenter, const Vec2& center, float radius, int n);

        // Single color
        void FillCircle_SC(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& color, float startAngle, float endAngle, StyleOptions& opts, int drawOrder);

        // Vertical or horizontal gradinet.
        void FillCircle_VerHorGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, float startAngle, float endAngle, StyleOptions& opts, int drawOrder);

        // Radial gradient
        void FillCircle_RadialGra(int thread, DrawBuffer* buf, float rotateAngle, const Vec2& center, float radius, int segments, const Vec4& colorStart, const Vec4& colorEnd, float startAngle, float endAngle, StyleOptions& opts, int drawOrder);

        // Fill circle impl
        void FillCircleData(int thread, Array<Vertex>& v, bool hasCenter, const Vec2& center, float radius, int segments, float startAngle, float endAngle);

        // Single color
        void FillConvex_SC(int thread, DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& color, StyleOptions& opts, int drawOrder);

        // Vertical horizontal gradient
        void FillConvex_VerHorGra(int thread, DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, bool isHor, StyleOptions& opts, int drawOrder);

        // Radial gradient.
        void FillConvex_RadialGra(int thread, DrawBuffer* buf, float rotateAngle, Vec2* points, int size, const Vec2& center, const Vec4& colorStart, const Vec4& colorEnd, StyleOptions& opts, int drawOrder);

        /// Triangle bounding box.
        void GetTriangleBoundingBox(const Vec2& p1, const Vec2& p2, const Vec2& p3, Vec2& outMin, Vec2& outMax);

        /// Triangulates & fills the index array given a start and end vertex index.
        void ConvexFillVertices(int startIndex, int endIndex, Array<Index>& indices, bool skipLastTriangle = false);

        /// Fills convex shapes without the assumption of a center vertex. Used for filling outer areas of non-filled shapes.
        void ConvexExtrudeVertices(DrawBuffer* buf, const StyleOptions& style, const Vec2& center, int startIndex, int endIndex, float thickness, bool skipEndClosing = false);

        /// Fill the point array with points to draw an arc with a direction hint.
        void GetArcPoints(Array<Vec2>& points, const Vec2& p1, const Vec2& p2, Vec2 directionHintPoint = Vec2(-1.0f, -1.0f), float radius = 0.0f, float segments = 36, bool flip = false, float angleOffset = 0.0f);

        /// Rotates all the vertices in the given range via their vertex average center.
        void RotateVertices(Array<Vertex>& vertices, const Vec2& center, int startIndex, int endIndex, float angle);

        /// Rotates all points by a center.
        void RotatePoints(Vec2* points, int size, const Vec2& center, float angle);

        /// <summary>
        /// Fills bounding box information for given points/vertices.
        /// </summary>
        void GetConvexBoundingBox(Vec2* points, int size, Vec2& outMin, Vec2& outMax);
        void GetConvexBoundingBox(Vertex* points, int size, Vec2& outMin, Vec2& outMax);
        void GetConvexBoundingBox(DrawBuffer* buf, int startIndex, int endIndex, Vec2& outMin, Vec2& outMax);

        /// <summary>
        /// Auto-calculate UVs for the vertices defined by start & end indices using 2D bounding-box.
        /// </summary>
        void CalculateVertexUVs(DrawBuffer* buf, int startIndex, int endIndex);

        // Angle increment based on rounding value.
        float GetAngleIncrease(float rounding);

        /// <summary>
        /// Returns the direction vector going from the center of the arc towards it's middle angle.
        /// </summary>
        Vec2 GetArcDirection(const Vec2& center, float radius, float startAngle, float endAngle);

        /// <summary>
        /// Returns the average center position for the given vertices.
        /// </summary>
        Vec2 GetVerticesCenter(DrawBuffer* buf, int startIndex, int endIndex);

        /// Line calculation methods.
        void       CalculateLine(Line& line, const Vec2& p1, const Vec2& p2, StyleOptions& style, LineCapDirection lineCapToAdd);
        void       JoinLines(Line& line1, Line& line2, StyleOptions& opts, LineJointType joinType, bool mergeUpperVertices);
        void       DrawSimpleLine(int thread, SimpleLine& line, StyleOptions& style, float rotateAngle, int drawOrder);
        void       CalculateLineUVs(Line& line);
        SimpleLine CalculateSimpleLine(const Vec2& p1, const Vec2& p2, StyleOptions& style);

        /// <summary>
        /// Draws an outline (or AA) around the vertices given, following the specific draw order via index array.
        /// Used for semi-circles, arcs, lines and alike.
        /// </summary>
        /// <returns></returns>
        DrawBuffer* DrawOutlineAroundShape(int thread, DrawBuffer* sourceBuffer, StyleOptions& opts, int* indicesOrder, int vertexCount, float defThickness, bool ccw = false, int drawOrder = 0, OutlineCallType = OutlineCallType::Normal);

        /// <summary>
        /// Draws an outline (or AA) around a convex shape.
        /// Vertex count need to be without including the center vertex.
        /// </summary>
        /// <param name="sourceBuffer"></param>
        /// <param name="vertexCount"></param>
        /// <param name="opts"></param>
        /// <returns></returns>
        DrawBuffer* DrawOutline(int thread, DrawBuffer* sourceBuffer, StyleOptions& opts, int vertexCount, bool skipEnds = false, int drawOrder = 0, OutlineCallType = OutlineCallType::Normal, bool reverseDrawDir = false);

#ifdef LINAVG_TEXT_SUPPORT

        /// <summary>
        /// Break down text into words, with each word having calculated size properties.
        /// </summary>
        void ParseTextIntoWords(Array<TextPart*>& arr, const char* text, LinaVGFont* font, float scale, float spacing);

        /// <summary>
        /// Converts the given words into a set of lines based on wrapping.
        /// </summary>
        void ParseWordsIntoLines(Array<TextPart*>& lines, const Array<TextPart*>& words, LinaVGFont* font, float scale, float spacing, float wrapWidth, float sdfThickness);

        /// <summary>
        /// Process, parse & draw text according to options.
        /// </summary>
        void ProcessText(DrawBuffer* buf, LinaVGFont* font, const char* text, const Vec2& pos, const Vec2& offset, const Vec4Grad& color, float spacing, bool isGradient, float scale, float wrapWidth, float rotateAngle, TextAlignment alignment, float newLineSpacing, float sdfThickness, TextOutData* outData);

        /// <summary>
        /// DrawText implementation.
        /// </summary>
        void DrawText(DrawBuffer* buf, LinaVGFont* font, const char* text, const Vec2& pos, const Vec2& offset, const Vec4Grad& color, float spacing, bool isGradient, float scale, TextOutData* outData);

        /// <summary>
        /// Returns the total text size for non-wrapped text.
        /// </summary>
        /// <returns></returns>
        Vec2 CalcTextSize(const char* text, LinaVGFont* font, float scale, float spacing, float sdfThickness);

        /// <summary>
        /// Max character offset from the quad corners for SDF characters.
        /// </summary>
        /// <returns></returns>
        Vec2 CalcMaxCharOffset(const char* text, LinaVGFont* font, float scale);

        /// <summary>
        /// Returns the total text size for wrapped text.
        /// </summary>
        Vec2 CalcTextSizeWrapped(const char* text, LinaVGFont*, float newLineSpacing, float wrapping, float scale, float spacing, float sdfThickness);

        /// <summary>
        /// For processing UTf8 texts.
        /// </summary>
        LINAVG_VEC<int32_t> GetUtf8Codepoints(const char* str);

        /// <summary>
        /// Parse text into wrapped lines.
        /// </summary>
        void WrapText(LINAVG_VEC<TextPart>& lines, LinaVGFont* font, const char* text, float spacing, float scale, float wrapWidth);
#endif

    }; // namespace Internal

} // namespace LinaVG

#endif
