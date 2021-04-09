#pragma once

#include "Horizon/Core/HorizonCommon.h"

namespace Horizon
{
	
#define RENDER_GRAPH_NULL_HANDLE (uint64(0))

	class RenderGraphHandleBase
	{
	public:
		using Handle = uint64;
		RenderGraphHandleBase() : mHandle(RENDER_GRAPH_NULL_HANDLE) {}
		virtual ~RenderGraphHandleBase() = default;
		RenderGraphHandleBase(Handle handle) : mHandle(handle) {}
		operator bool() const { return mHandle != RENDER_GRAPH_NULL_HANDLE; }
		operator Handle() const { return mHandle; }
		bool operator==(const RenderGraphHandleBase& rhs) const { return mHandle == rhs.mHandle; }
		bool operator!=(const RenderGraphHandleBase& rhs) const { return mHandle != rhs.mHandle; }
		RenderGraphHandleBase& operator=(const Handle& handle) { mHandle = handle; return *this; }
		RenderGraphHandleBase& operator=(const RenderGraphHandleBase& rhs) = default;
	private:
		Handle mHandle;
	};

	template<typename T>
	class RenderGraphHandle : public RenderGraphHandleBase
	{
	public:
		RenderGraphHandle() = default;
		RenderGraphHandle(RenderGraphHandleBase::Handle handle) : RenderGraphHandleBase(handle) {}
	};
}
