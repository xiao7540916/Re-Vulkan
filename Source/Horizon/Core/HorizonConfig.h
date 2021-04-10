#pragma once

namespace Horizon
{
#ifndef HORIZON_DEBUG_MODE
	#if defined(_DEBUG) || defined(DEBUG)
		#define HORIZON_DEBUG_MODE
	#else
		#define HORIZON_DEBUG_MODE
	#endif
#endif

#define HORIZON_ENGINE_NAME "Horizon Engine"

#define HORIZON_ENGINE_VERSION 0

}