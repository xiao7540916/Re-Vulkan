#pragma once

#include "Horizon/Core/HorizonCommon.h"
#include "Horizon/RenderBackend/RenderBackendConfig.h"
#include "Horizon/RenderBackend/RenderFormats.h"

namespace Horizon
{
    enum
	{
		MaxPushConstantsSize = 256,
		MaxSimultaneousColorAttachmentCount = 8,
    };

	/** These flags indicate the usage of the resource, and hint the device what pipeline stages the resource will be bound to. */
    enum class ResourceBindFlags : uint32_t
    {
        None            = 0,
        VertexBuffer    = (1 << 0),
        IndexBuffer     = (1 << 1),
        UniformBuffer   = (1 << 2),
        ShaderResource  = (1 << 3),
        InputAttachemt  = (1 << 4),
        RenderTarget    = (1 << 5),
        DepthStencil    = (1 << 6),
    };
    HORIZON_ENUM_CLASS_OPERATORS(ResourceBindFlags);

	enum class AttachmentClearFlags
	{
		DontCare        = 0,
		ColorDepthClear = (1 << 0),
		ColorDepthLoad  = (1 << 1),
		StencilClear    = (1 << 2),
		StencilLoad     = (1 << 3),
	};

	HORIZON_ENUM_CLASS_OPERATORS(AttachmentClearFlags);

	class RenderBackend;
	class RenderDevice;
	class RenderContext;
	class ComputeContext;
	class CommandBuffer;
	class CommandQueue;
}