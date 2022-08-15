#include "Panels/PreviewPanel.hpp"
#include "Core/EditorCommon.hpp"
#include "Widgets/WidgetsUtility.hpp"
#include "Core/EditorApplication.hpp"
#include "Core/RenderEngine.hpp"
#include "ECS/Components/EntityDataComponent.hpp"
#include "Rendering/Model.hpp"
#include "Core/Engine.hpp"
#include "Drawers/TextureDrawer.hpp"
#include "Drawers/ModelDrawer.hpp"
#include "Drawers/MaterialDrawer.hpp"

namespace Lina::Editor
{
    PreviewPanel::~PreviewPanel()
    {
        delete m_shaderPreviewMaterial;
    }

    void PreviewPanel::Initialize(const char* id, const char* icon)
    {
        EditorPanel::Initialize(id, icon);
        m_leftPaneWidth    = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMinWidth = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMaxWidth = 600 * GUILayer::Get()->GetDPIScale();

        m_primitives.push_back("Resources/Engine/Meshes/Primitives/Sphere.fbx");
        m_primitives.push_back("Resources/Engine/Meshes/Primitives/Cube.fbx");
        m_primitives.push_back("Resources/Engine/Meshes/Primitives/Cylinder.fbx");
        m_primitives.push_back("Resources/Engine/Meshes/Primitives/Capsule.fbx");
        m_primitives.push_back("Resources/Engine/Meshes/Primitives/Plane.fbx");
        m_primitives.push_back("Resources/Engine/Meshes/Primitives/Quad.fbx");

        m_shaderPreviewMaterial = new Graphics::Material();
        m_shaderPreviewMaterial->SetShader(Graphics::RenderEngine::Get()->GetDefaultLitShader());
    }
    void PreviewPanel::Draw()
    {
        if (m_show)
        {
            if (Begin())
            {
                m_rightPaneSize = Vector2(ImGui::GetWindowWidth() - m_leftPaneWidth, ImGui::GetWindowHeight());
                ImGui::BeginChild("previewPanel_leftPane", ImVec2(m_leftPaneWidth, 0));

                if (m_previewType == PreviewType::Texture)
                    TextureDrawer::DrawTextureSettings(m_targetTexture, m_leftPaneWidth);
                else if (m_previewType == PreviewType::Model)
                    ModelDrawer::DrawModelSettings(m_targetModel, m_leftPaneWidth);
                else if (m_previewType == PreviewType::Material)
                    MaterialDrawer::DrawMaterialSettings(m_targetMaterial, m_leftPaneWidth);

                ImGui::EndChild();

                ImGui::SameLine();

                ImGui::BeginChild("previewPanel_rightPane");

                DrawPreviewArea();

                ImGui::EndChild();

                WidgetsUtility::VerticalResizeDivider(true, &m_resizeDividerPressedPos, &m_leftPaneWidth, m_leftPaneMinWidth, m_leftPaneMaxWidth, &m_lockWindowPos, &m_draggingVerticalDivider);

                End();
            }
        }
    }
    void PreviewPanel::Open()
    {
        m_previewCameraPosition = Vector3(0, 5, -10);
        m_previewCameraRotation = Quaternion::LookAt(m_previewCameraPosition, Vector3(0, 0, 0), Vector3::Up);
        m_mouseAngles.x         = m_previewCameraRotation.GetEuler().y;
        m_mouseAngles.y         = m_previewCameraRotation.GetEuler().x;
        EditorPanel::Open();
    }

    void PreviewPanel::SetTargetModel(Graphics::Model* model)
    {
        m_targetModel      = model;
        m_title            = "Model Preview: " + model->GetPath();
        m_previewType      = PreviewType::Model;
        m_leftPaneWidth    = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMinWidth = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMaxWidth = 800 * GUILayer::Get()->GetDPIScale();
        Open();
    }

    void PreviewPanel::SetTargetTexture(Graphics::Texture* texture)
    {
        m_targetTexture    = texture;
        m_title            = "Texture Preview: " + texture->GetPath();
        m_previewType      = PreviewType::Texture;
        m_leftPaneWidth    = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMinWidth = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMaxWidth = 600 * GUILayer::Get()->GetDPIScale();
        Open();
    }

    void PreviewPanel::SetTargetMaterial(Graphics::Material* mat)
    {
        m_targetMaterial           = mat;
        m_title                    = "Material Preview: " + mat->GetPath();
        m_previewType              = PreviewType::Material;
        m_targetModel              = Resources::ResourceStorage::Get()->GetResource<Graphics::Model>("Resources/Engine/Meshes/Primitives/Sphere.fbx");
        m_currentSelectedPrimitive = 0;
        m_leftPaneWidth            = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMinWidth         = 420 * GUILayer::Get()->GetDPIScale();
        m_leftPaneMaxWidth         = 600 * GUILayer::Get()->GetDPIScale();
        Open();
    }

    void PreviewPanel::DrawPreviewArea()
    {
        const ImVec2 padding         = ImVec2(ImGui::GetStyle().ItemSpacing.x + 4, ImGui::GetStyle().ItemSpacing.y + 4);
        const ImVec2 cursorPosScreen = ImGui::GetCursorScreenPos();
        const ImVec2 cursorPos       = ImGui::GetCursorPos();
        const ImVec2 bgMin           = ImVec2(cursorPosScreen.x + padding.x - 4, cursorPosScreen.y + padding.x);
        const ImVec2 bgMax           = ImVec2(cursorPosScreen.x + m_rightPaneSize.x - padding.x * 2, cursorPosScreen.y + m_rightPaneSize.y - padding.x * 4.0f);
        const ImVec2 bgSize          = ImVec2(bgMax.x - bgMin.x, bgMax.y - bgMin.y);
        const ImVec4 bgCol           = ImGui::GetStyleColorVec4(ImGuiCol_PopupBg);
        ImGui::GetWindowDrawList()->AddRectFilled(bgMin, bgMax, ImGui::ColorConvertFloat4ToU32(bgCol), 3.0f);
        const Vector2 bgMinLina = Vector2(bgMin.x, bgMin.y);
        const Vector2 bgMaxLina = Vector2(bgMax.x, bgMax.y);

        auto* renderEngine         = Graphics::RenderEngine::Get();
        float currentAspect        = 0.0f;
        bool  previewCameraEnabled = m_previewType != PreviewType::Texture;

        if (previewCameraEnabled)
            BeginPreviewCamera(Vector2(bgSize.x, bgSize.y));

        // Draw the actual target.
        if (m_previewType == PreviewType::Texture)
            TextureDrawer::DrawTexture(m_targetTexture, bgMinLina, bgMaxLina);
        else if (m_previewType == PreviewType::Model)
            ModelDrawer::DrawModel(m_targetModel, Matrix::Identity(), bgMinLina, bgMaxLina);
        else if (m_previewType == PreviewType::Material)
            MaterialDrawer::DrawMaterial(m_targetMaterial, m_targetModel, bgMinLina, bgMaxLina);

        // Preview model selector.
        PreviewModelSelector(Vector2(cursorPos.x, cursorPos.y), m_rightPaneSize.y);

        if (previewCameraEnabled)
            EndPreviewCamera(Vector2(bgMin.x, bgMin.y), Vector2(bgMax.x, bgMax.y));
    }

    void PreviewPanel::BeginPreviewCamera(const Vector2& bgSize)
    {
        auto* renderEngine         = Graphics::RenderEngine::Get();
        float currentAspect        = 0.0f;
        bool  previewCameraEnabled = m_previewType != PreviewType::Texture;

        // Handle camera movement & rotation.
        auto& editorCamSystem = EditorApplication::Get()->GetCameraSystem();
        auto& data            = ECS::Registry::Get()->get<ECS::EntityDataComponent>(editorCamSystem.GetEditorCamera());
        WidgetsUtility::SaveEditorCameraBeforeSnapshot(bgSize.x / bgSize.y);

        if (ImGui::IsWindowHovered() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)))
            ImGui::SetWindowFocus();

        const float delta = (float)Engine::Get()->GetRawDelta();

        if (ImGui::IsWindowFocused())
            editorCamSystem.RotateBehaviour(delta, m_previewCameraRotation, m_mouseAngles);

        data.SetRotation(m_previewCameraRotation);

        if (ImGui::IsWindowFocused())
            editorCamSystem.MoveBehaviour(delta, m_mouseDragStart, m_previewCameraPosition, m_previewCameraRotation);

        data.SetPosition(m_previewCameraPosition);
    }
    void PreviewPanel::EndPreviewCamera(const Vector2& bgMinLina, const Vector2& bgMaxLina)
    {
        WidgetsUtility::ResetEditorCamera();

        // Draw tools.
        ImVec2       bgMin        = ImVec2(bgMinLina.x, bgMinLina.y);
        ImVec2       bgMax        = ImVec2(bgMaxLina.x, bgMaxLina.y);
        const ImRect confineSpace = ImRect(bgMin, bgMax);
        WidgetsUtility::TransformOperationTools("##modelpanel_transformops", confineSpace);
    }
    void PreviewPanel::PreviewModelSelector(const Vector2& cursorPosLina, float confineRectHeight)
    {
        const ImVec2 cursorPos = ImVec2(cursorPosLina.x, cursorPosLina.y);
        if (m_previewType != PreviewType::Texture && m_previewType != PreviewType::Model)
        {
            ImGui::SetCursorPosY(confineRectHeight - 80.0f * GUILayer::Get()->GetDPIScale());
            ImGui::SetCursorPosX((m_rightPaneSize.x - cursorPos.x) - 220.0f * GUILayer::Get()->GetDPIScale());
            const int selected = WidgetsUtility::SelectPrimitiveCombobox("##preview_panel_model", m_primitives, m_currentSelectedPrimitive, 12.0f);
            if (selected != m_currentSelectedPrimitive)
            {
                m_currentSelectedPrimitive = selected;
                m_targetModel              = Resources::ResourceStorage::Get()->GetResource<Graphics::Model>(m_primitives[m_currentSelectedPrimitive]);
            }
        }
    }
} // namespace Lina::Editor