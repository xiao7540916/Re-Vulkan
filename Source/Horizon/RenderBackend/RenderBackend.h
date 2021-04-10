#pragma once

#include "Horizon/RenderBackend/RenderBackendCommon.h"
#include "Horizon/RenderBackend/RenderHandles.h"

namespace Horizon
{
	struct ApplicationInfo
	{
		const char* pApplicationName;
		uint32 applicationVersion;
		const char* pEngineName;
		uint32 engineVersion;
	};

	struct RenderDeviceExtensionFlags
	{
		bool bindlessResources = false;
		bool realTimeRayTracing = false;
	};

	class RenderBackend
	{
	public:
		RenderBackend() = default;
		virtual ~RenderBackend() = default;
		virtual RenderDevice* CreateRenderDevice(RenderDeviceExtensionFlags extensions) = 0;
		virtual void DestroyRenderDevice(RenderDevice* device) = 0;
	};
}
