#include <Lavender/Core/Application.hpp>
#include <Lavender/Entrypoint.hpp>

#include "EditorLayer.hpp"

class Sandbox : public Lavender::Application
{
public:
	Sandbox(const Lavender::ApplicationSpecification& appInfo)
		: Lavender::Application(appInfo)
	{
		AddLayer(new EditorLayer());
	}
};



// ----------------------------------------------------------------
//                    Set Application specs here...
// ----------------------------------------------------------------
Lavender::Application* Lavender::CreateApplication(int argc, char* argv[])
{
	ApplicationSpecification appInfo = {};
	appInfo.WindowSpecs.Name = "Custom";
	appInfo.WindowSpecs.VSync = false;

	return new Sandbox(appInfo);
}