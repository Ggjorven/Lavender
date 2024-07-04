#include "Viewport.hpp"

#include "Lavender/Core/Application.hpp"
#include "Lavender/Core/Input/Input.hpp"
#include "Lavender/Utils/Profiler.hpp"
#include "Lavender/Utils/Math.hpp"

#include "Lavender/Renderer/Renderer.hpp"
#include "Lavender/Renderer/CommandBuffer.hpp"

#include "Lavender/WorkSpace/Scene.hpp"
#include "Lavender/WorkSpace/Project.hpp"
#include "Lavender/WorkSpace/EngineTracker.hpp"

#include "Lavender/UI/UI.hpp"
#include "Lavender/UI/Style.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lavender::UI
{

	Viewport::Viewport(Ref<Entities> entities)
		: m_EntitiesRef(entities)
	{
		InitStyles();
		InitResources();
	}

	Viewport::~Viewport()
	{
	}

	void Viewport::RenderUI()
	{
		APP_PROFILE_SCOPE("Viewport::UI");

		UI::ScopedStyleList styles = UI::ScopedStyleList({
			{ UI::StyleType::WindowPadding, { 0.0f, 0.0f } }
		});

		UI::BeginWindow("Viewport", UI::WindowFlags::NoCollapse | UI::WindowFlags::NoDecoration | UI::WindowFlags::NoBackground | UI::WindowFlags::NoMove | UI::WindowFlags::NoTitleBar | UI::WindowFlags::NoTabBar);
		{
			m_TopLeftCursorPos = { ImGui::GetCursorPos().x, ImGui::GetCursorPos().y };
			
			auto imWindow = ImGui::GetCurrentWindow();
			auto& position = imWindow->Pos;
			auto& size = imWindow->Size;
			if ((uint32_t)size.x != Track::Viewport::Width || (uint32_t)size.y != Track::Viewport::Height)
				Resize((uint32_t)size.x, (uint32_t)size.y);

			auto& mainWindow = Application::Get().GetWindow();
			Track::Viewport::Position.x = (uint32_t)position.x - mainWindow.GetPositionX();
			Track::Viewport::Position.y = (uint32_t)position.y - mainWindow.GetPositionY();
		}
		{
			auto region = ImGui::GetContentRegionAvail();
			ImGui::Image(Scene::Get()->GetRenderer()->GetResources().Shading.Attachment->GetTextureID(), { region.x, region.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f });
		}
		{
			RenderTopUI();
		}
		ImGui::End();
	}

	void Viewport::Resize(uint32_t width, uint32_t height)
	{
		// Set size
		Track::Viewport::Width = width;
		Track::Viewport::Height = height;

		// Resize
		Scene::Get()->GetRenderer()->Resize(width, height);
	}

	Ref<Viewport> Viewport::Create(Ref<Entities> entities)
	{
		return RefHelper::Create<Viewport>(entities);
	}

	void Viewport::InitStyles()
	{
		m_Styles = UI::StyleList({
			{ UI::StyleType::FrameRounding, 4.0f }
		});

		m_Colours = UI::StyleList({
			{ UI::StyleColourType::WindowBg, UI::Colours::BackgroundDark },
			//{ UI::StyleColourType::Header, UI::Colours::BackgroundPopup },
			//{ UI::StyleColourType::HeaderHovered, UI::Colours::LightTint },
			//{ UI::StyleColourType::HeaderActive, UI::Colours::LighterTint },
		});
	}

	void Viewport::InitResources()
	{
		ImageSpecification specs = {};
		specs.Usage = ImageUsage::File;
		specs.Layout = ImageLayout::ShaderRead;
		specs.Flags = ImageUsageFlags::Sampled | ImageUsageFlags::Colour;
		specs.CreateUIImage = true;

		{
			specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/Play.png";
			m_PlayButton = Image2D::Create(specs);

			specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/Stop.png";
			m_StopButton = Image2D::Create(specs);
		}
		{
			specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/MoveTool.png";
			m_MoveButton = Image2D::Create(specs);

			specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/ScaleTool.png";
			m_ScaleButton = Image2D::Create(specs);

			specs.Path = Track::Lavender::Directory / "Editor/Resources/Images/RotateTool.png";
			m_RotateButton = Image2D::Create(specs);
		}
	}

	void Viewport::RenderTopUI()
	{
		m_Styles.Push();
		m_Colours.Push();

		UI::SetCursorPos(m_TopLeftCursorPos);

		// Buttons
		{
			Ref<Image2D> playButton = ((Project::Get()->GetState() == WorkSpace::State::Editor) ? m_PlayButton : m_StopButton);
			UI::ShiftCursorX(((float)Track::Viewport::Width / 2.0f) - ((32.0f / 2.0f)));
			UI::ShiftCursorY(1.0f);

			// Play/Stop Button
			if (ImGui::ImageButton((ImTextureID)playButton->GetTextureID(), { 32.0f, 32.0f }))
			{
				if (Project::Get()->GetScript())
				{
					Project::Get()->GetScript()->Unload();
				}

				// TODO: Make usable on all platforms
				m_EditorGame.Start(Track::Lavender::Directory / "Editor/Resources/Editor/Game" / ConfigurationToString(Track::Lavender::Config) / "EditorGame.exe", std::filesystem::relative(Project::Get()->GetInfo().Path, Track::Lavender::Directory).string());

				//Project::Get()->SwitchState();
			}
			if (m_EditorGame.IsRunning()) APP_LOG_TRACE("Running");


			// ImGuizmo buttons
			{
				static const ImVec2 guizmoButtonSize = { 24.0f, 24.0f };
				static const float paddingBetween = 7.0f;

				UI::ScopedStyleList styles = UI::StyleList({
					{ UI::StyleColourType::Button, { 0.0f, 0.0f, 0.0f, 0.0f } },
					{ UI::StyleColourType::ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f } },
					{ UI::StyleColourType::ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f } }
				});

				UI::SetCursorPos({ m_TopLeftCursorPos.x + Track::Viewport::Width - (guizmoButtonSize.x + paddingBetween) * 3.0f, m_TopLeftCursorPos.y + 1.0f});
				if (ImGui::ImageButton((ImTextureID)m_MoveButton->GetTextureID(), guizmoButtonSize)) m_Operation = ImGuizmo::OPERATION::TRANSLATE;

				UI::SetCursorPos({ m_TopLeftCursorPos.x + Track::Viewport::Width - (guizmoButtonSize.x + paddingBetween) * 2.0f, m_TopLeftCursorPos.y + 1.0f});
				if (ImGui::ImageButton((ImTextureID)m_ScaleButton->GetTextureID(), guizmoButtonSize)) m_Operation = ImGuizmo::OPERATION::SCALE;

				UI::SetCursorPos({ m_TopLeftCursorPos.x + Track::Viewport::Width - (guizmoButtonSize.x + paddingBetween), m_TopLeftCursorPos.y + 1.0f});
				if (ImGui::ImageButton((ImTextureID)m_RotateButton->GetTextureID(), guizmoButtonSize)) m_Operation = ImGuizmo::OPERATION::ROTATE;
			}
		}

		// ImGuizmo
		if (!(Project::Get()->GetState() == WorkSpace::State::Runtime))
		{
			auto scene = Scene::Get();
			auto camera = scene->GetActiveCamera();

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			auto imWindow = ImGui::GetCurrentWindow();
			ImGuizmo::SetRect(imWindow->Pos.x, imWindow->Pos.y, imWindow->Size.x, imWindow->Size.y);

			if (m_EntitiesRef->m_SelectedEntity != UUID::Empty)
			{
				Entity& entity = scene->GetRegistry(Project::Get()->GetState()).GetEntity(m_EntitiesRef->m_SelectedEntity);

				if (entity.HasComponent<TransformComponent>())
				{
					TransformComponent& transform = entity.GetComponent<TransformComponent>();

					glm::mat4 model = {};
					Math::ComposeTransform(model, transform.Position, glm::radians(transform.Rotation), transform.Size);

					// ImGuizmo is made for OpenGL so we have to transform it back
					glm::mat4 projection = camera->GetProjectionMatrix();
					if constexpr (RendererSpecification::API == RenderingAPI::Vulkan)
						projection[1][1] *= -1;

					// Snap values
					bool snap = Input::IsKeyPressed(Key::LeftControl);
					float snapValue = 0.1f;
					if (m_Operation == ImGuizmo::OPERATION::ROTATE) 
						snapValue = 1.0f;
					float snapValues[3] = { snapValue, snapValue, snapValue };

					// Actually start
					ImGuizmo::Manipulate(glm::value_ptr(camera->GetViewMatrix()), glm::value_ptr(projection), m_Operation, m_Mode, glm::value_ptr(model), nullptr, snap ? snapValues : nullptr);
					if (ImGuizmo::IsUsing())
					{
						glm::vec3 translation = {};
						glm::quat rotation = {};
						glm::vec3 scale = {};
						Math::DecomposeTransform(model, translation, rotation, scale);

						switch (m_Operation)
						{
						case ImGuizmo::OPERATION::TRANSLATE:
						{
							transform.Position = translation;
							break;
						}
						case ImGuizmo::OPERATION::ROTATE: // TODO: Implement
						{
							APP_LOG_TRACE("TODO");
							break;
						}
						case ImGuizmo::OPERATION::SCALE:
						{
							transform.Size = scale;
							break;
						}
						}
					}
				}
			}
		}

		m_Styles.Pop();
		m_Colours.Pop();
	}

}