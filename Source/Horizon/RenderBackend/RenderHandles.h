#pragma once

namespace Horizon
{

#define RENDER_BACKEND_NULL_HANDLE 0

	class RenderResourceHandle
	{
	public:
		using Handle = uint64_t;
		RenderResourceHandle() : mHandle(RENDER_BACKEND_NULL_HANDLE) {}
		virtual ~RenderResourceHandle() = default;
		RenderResourceHandle(Handle handle) : mHandle(handle) {}
		operator bool() const { return mHandle != RENDER_BACKEND_NULL_HANDLE; }
		operator Handle() const { return mHandle; }
		bool operator==(const RenderResourceHandle& rhs) const { return mHandle == rhs.mHandle; }
		bool operator!=(const RenderResourceHandle& rhs) const { return mHandle != rhs.mHandle; }
		RenderResourceHandle& operator=(const Handle & handle) { mHandle = handle; return *this; }
		RenderResourceHandle& operator=(const RenderResourceHandle& rhs) = default;
	private:
		Handle mHandle;
	};

	template<typename T>
	class RenderHandle : public RenderResourceHandle
	{
	public:
		RenderHandle() = default;
		RenderHandle(RenderResourceHandle::Handle handle) : RenderResourceHandle(handle) {}
	};

	class Viewport;
	class Buffer;
	class Texture;

	using RenderViewportHandle = RenderHandle<Viewport>;
	using RenderBufferHandle = RenderHandle<Buffer>;
	using RenderTextureHandle = RenderHandle<Texture>;
}
