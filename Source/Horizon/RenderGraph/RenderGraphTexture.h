#pragma once

#include "Horizon/Core/HorizonCommon.h"
#include "Horizon/RenderBackend/RenderHandles.h"
#include "Horizon/RenderBackend/RenderBackendCommon.h"

namespace Horizon
{
	struct RenderGraphTexture
	{
        struct Description
        {
            enum class Type
            {
                Texture1D,
                Texture2D,
                Texture3D,
                TextureCube,
            };
            uint32 width = 1;
            uint32 height = 1;
            uint32 depth = 1;
            uint32 mipLevels = 1;
            uint32 arrayLayers = 1;
            uint32 samples = 0;
            Type type = Type::Texture2D;
            Format format = Format::RGBA32Float;
        };

        struct SubresourceDescription
        {
            uint32 level = 0;
            uint32 layer = 0;
        };

        /// Render handle.
        RenderTextureHandle handle = RENDER_BACKEND_NULL_HANDLE;

        void Create(RenderDevice* device, const Description& desc, ResourceBindFlags flags);
        void Destroy(RenderDevice* device);

        /** Generates the Description for a subresource from its parent Description and its SubresourceDescription. */
        static Description GenerateDescriptionForSubresource(const Description& parentDesc, const SubresourceDescription& subDesc);
	};
}
