#include <Lavender/Core/Application.hpp>
#include <Lavender/Entrypoint.hpp>

#include "RuntimeLayer.hpp"

class Sandbox : public Lavender::Application
{
public:
	Sandbox(const Lavender::ApplicationSpecification& appInfo)
		: Lavender::Application(appInfo)
	{
		AddLayer(new RuntimeLayer());
	}
};



// ----------------------------------------------------------------
//                    Set Application specs here...
// ----------------------------------------------------------------
Lavender::Application* Lavender::CreateApplication(int argc, char* argv[])
{
	ApplicationSpecification appInfo = {};
	appInfo.WindowSpecs.Name = "Runtime | Initializing...";
	appInfo.WindowSpecs.Width = 1280;
	appInfo.WindowSpecs.Height = 720;
	appInfo.WindowSpecs.VSync = false;

	appInfo.EnableUI = false;

	return new Sandbox(appInfo);
}