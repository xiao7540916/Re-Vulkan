#pragma once

#include "Horizon/Core/HorizonCommon.h"
#include "Horizon/RenderBackend/RenderBackendCommon.h"

#include "RenderGraphHandle.h"
#include "RenderGraphTexture.h"

namespace Horizon
{
    struct RenderGraphRenderPass
    {
        struct Description
        {
            struct AttachmentDescription
            {
                float clearColorValue[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
                float clearDepthValue = 1.0f;
                uint32 clearStencilValue = 0;
                AttachmentClearFlags clearFlags = AttachmentClearFlags::DontCare;
            };
            uint32 count = 0;
            AttachmentDescription attachmentDescriptions[MaxSimultaneousColorAttachmentCount + 1];
            RenderGraphHandle<RenderGraphTexture> colorAttachments[MaxSimultaneousColorAttachmentCount] = {};
            RenderGraphHandle<RenderGraphTexture> depthStencilAttachment = RENDER_GRAPH_NULL_HANDLE;
        };
    };
}