#pragma once

#include "VulkanCommon.h"

namespace Horizon
{
	class Fence
	{
	public:
        inline FenceManager* GetOwner();
		inline VkFence GetHandle() const { return mHandle; }
		inline bool IsSignaled() const { return mState == State::Signaled; }
	protected:
        friend class FenceManager;
        Fence(Device* device, bool signaled);
        ~Fence();
		enum class State
		{
            Unsignaled,
			Signaled,
		};
		State mState;
        Device* mDevice;
        VkFence mHandle;
	};

    class FenceManager
    {
    public:

        FenceManager(Device* device);
        ~FenceManager();

        void Init();
        void Destroy();

        Fence* AllocateFence(bool signaled = false);

        bool IsFenceSignaled(Fence* fence);

        bool WaitForFence(Fence* fence, uint64 timeInNanoseconds = UINT64_MAX);

        void ResetFence(Fence* fence);

        /** Free a fence immediately. */
        void FreeFence(Fence*& fence);

        /** Wait before freeing a fence. */
        void WaitAndFreeFence(Fence*& fence, uint64 timeInNanoseconds = UINT64_MAX);

    private:

        Device* mDevice;

        Vector<Fence*> mActiveFences;
        Vector<Fence*> mFreeFences;
    };
}
