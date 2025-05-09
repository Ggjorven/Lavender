#include <Lavender/Core/Application.hpp>
#include <Lavender/Entrypoint.hpp>

#include "TestLayer.hpp"

class Sandbox : public Lavender::Application
{
public:
	Sandbox(const Lavender::ApplicationSpecification& appInfo)
		: Lavender::Application(appInfo)
	{
		AddLayer(new TestLayer());
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

	appInfo.RenderSpecs.FramesInFlight = 3;

	return new Sandbox(appInfo);
}