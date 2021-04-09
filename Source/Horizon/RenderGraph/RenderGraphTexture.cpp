#include "RenderGraphTexture.h"

#include "Horizon/RenderBackend/RenderDevice.h"

namespace Horizon
{
    void RenderGraphTexture::Create(RenderDevice* device, const Description& desc, ResourceBindFlags usage)
    {
        assert(handle == RENDER_BACKEND_NULL_HANDLE);
        // TODO
    }

    void RenderGraphTexture::Destroy(RenderDevice* device)
    {
        if (handle) 
        {
            // TODO
            handle = RENDER_BACKEND_NULL_HANDLE;
        }
    }
}