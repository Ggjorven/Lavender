#include <Lavender/Core/Application.hpp>
#include <Lavender/Entrypoint.hpp>

#include "CustomLayer.hpp"

class Sandbox : public Lavender::Application
{
public:
	Sandbox(const Lavender::AppInfo& appInfo)
		: Lavender::Application(appInfo)
	{
		AddLayer(new CustomLayer());
	}
};



// ----------------------------------------------------------------
//                    Set Application specs here...
// ----------------------------------------------------------------
Lavender::Application* Lavender::CreateApplication(int argc, char* argv[])
{
	AppInfo appInfo(argc, argv);

	appInfo.WindowProperties.Name = "Custom";
	appInfo.WindowProperties.VSync = false;

	return new Sandbox(appInfo);
}