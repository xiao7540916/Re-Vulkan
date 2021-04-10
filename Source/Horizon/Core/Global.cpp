#include "Horizon/Core/Global.h"
#include "Horizon/Log/LogManager.h"

namespace Horizon
{
	namespace Global
	{
		void Init()
		{
			LogManager::Init();

			LOG_TRACE("Horizon Engine ( Debug Version )");
			LOG_TRACE("Initializing...");
		}

		void Shutdown()
		{
			LOG_TRACE("Shutting down...");
		}
	}
}