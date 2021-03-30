#pragma once

#include "HorizonConfig.h"

namespace Horizon
{

#define HORIZON_PLATFORM_WIN32 1
#define HORIZON_PLATFORM_LINUX 2

#define HORIZON_COMPILER_MSVC 1
#define HORIZON_COMPILER_GNUC 2

#if defined(__WIN32__) || defined(_WIN32)
	#define HORIZON_PLATFORM HORIZON_PLATFORM_WIN32
#endif

#if !defined(NOMINMAX) && defined(HORIZON_PLATFORM_WIN32)
	#define NOMINMAX
#endif

#if defined(_MSC_VER)
	#define HORIZON_COMPILER HORIZON_COMPILER_MSVC
#else
	#pragma error "Unknown compiler."
#endif

/** forceinline */
#if HORIZON_COMPILER == HORIZON_COMPILER_MSVC
	#define FORCEINLINE __forceinline
#endif

#if defined(_MSC_VER)
	#define HORIZON_DISABLE_WARNINGS() __pragma(warning(push, 0))
	#define HORIZON_ENABLE_WARNINGS() __pragma(warning(pop))
#endif

}