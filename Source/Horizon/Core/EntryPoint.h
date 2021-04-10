#pragma once

#include "Horizon/Core/Global.h"
#include "Horizon/Core/Application.h"

int main(int argc, char** argv)
{
	Horizon::Global::Init();
	auto app = Horizon::CreateApplication(argc, argv);
	if (app != nullptr)
	{
		app->Init();
		app->Run();
		app->Shutdown();
		delete app;
		Horizon::Global::Shutdown();
		return EXIT_SUCCESS;
	}
	Horizon::Global::Shutdown();
	return EXIT_FAILURE;
}
