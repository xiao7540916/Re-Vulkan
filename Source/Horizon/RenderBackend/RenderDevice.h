#pragma once

#include "Horizon/RenderBackend/RenderBackendCommon.h"
#include "Horizon/RenderBackend/RenderHandles.h"

namespace Horizon
{
    class RenderDevice
    {
    public:
        RenderDevice() = default;
        virtual ~RenderDevice() = default;
        virtual void ResizeViewport(RenderViewportHandle handle, uint32 width, uint32 height, bool isFullscreen = false) = 0;
        virtual RenderViewportHandle CreateRenderViewport(void* window) = 0;
        virtual void Flush() {}
        virtual void BeginDebugMarker(CommandBuffer* cb, const char* name, const float color[4]) {}
        virtual void InsertDebugMarker(CommandBuffer* cb, const char* name, const float color[4]) {}
        virtual void EndDebugMarker(CommandBuffer* cb) {}
    };
}