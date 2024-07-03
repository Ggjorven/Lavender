#pragma once

#include <Lavender/Core/Core.hpp>
#include <Lavender/Utils/Utils.hpp>

#include <Lavender/Renderer/Image.hpp>

#include <Lavender/UI/UI.hpp>

#include <Lavender/ECS/Entity.hpp>
#include <Lavender/ECS/Components.hpp>

#include "Panels/Entities.hpp"

namespace Lavender::UI
{

	enum class ComponentUsage
	{
		None = 0, Opened = BIT(0), Reset = BIT(1), Remove = BIT(2)
	};
	DEFINE_BITWISE_OPS(ComponentUsage)

	class Components
	{
	public:
		Components(Ref<Entities> entities);
		virtual ~Components();

		void RenderUI();

		static Ref<Components> Create(Ref<Entities> entities);

	public:
		ComponentUsage BeginComponent(const std::string& name = "Unnamed Component", Ref<Image2D> icon = nullptr);

		template<typename TComponent>
		inline void RenderComponent(Entity& entity) { APP_LOG_ERROR("Trying to render {0}, but has not been implemented.", ComponentToString<TComponent>()); }

		template<>
		inline void RenderComponent<TagComponent>(Entity& entity);
		template<>
		inline void RenderComponent<TransformComponent>(Entity& entity);
		template<>
		inline void RenderComponent<MeshComponent>(Entity& entity);
		template<>
		inline void RenderComponent<PointLightComponent>(Entity& entity);
		template<>
		inline void RenderComponent<ScriptComponent>(Entity& entity);
		template<>
		inline void RenderComponent<CameraComponent>(Entity& entity);

	private:
		void InitStyles();
		void InitIcons();

	private:
		Ref<Entities> m_EntitiesRef = nullptr;
		
		UI::StyleList m_Styles = {};
		UI::StyleList m_Colours = {};

		// Window Data
		uint32_t m_Width = 0u;
		uint32_t m_Height = 0u;
		glm::uvec2 m_Position = { 0u, 0u };

		// Images
		Ref<Image2D> m_DotsIcon = nullptr;
		Ref<Image2D> m_PlusIcon = nullptr;
	};

}