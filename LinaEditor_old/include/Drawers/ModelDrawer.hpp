/*
Class: ModelDrawer



Timestamp: 1/9/2022 11:54:18 PM
*/

#pragma once

#ifndef ModelDrawer_HPP
#define ModelDrawer_HPP

// Headers here.


namespace Lina
{
    class Vector2;
    class Matrix;

    namespace Graphics
    {
        class Model;
        class ModelNode;
    }
} // namespace Lina

namespace Lina::Editor
{
    class ModelDrawer
    {

    public:
        ModelDrawer() = default;
        ~ModelDrawer() = default;

        static void DrawModelSettings(Graphics::Model*& model, float leftPaneSize);
        static void DisplayNode(Graphics::ModelNode* node);
        static void DrawModel(Graphics::Model* model, Matrix& matrix, const Vector2& bgMin, const Vector2& bgMax);

    private:
    };
} // namespace Lina::Editor

#endif
