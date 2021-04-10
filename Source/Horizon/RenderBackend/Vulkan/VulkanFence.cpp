#include "VulkanFence.h"
#include "VulkanDevice.h"

namespace Horizon
{
	Fence::Fence(Device* device, bool signaled)
		: mState(signaled ? State::Signaled : State::Unsignaled)
		, mDevice(device)
		, mHandle(VK_NULL_HANDLE)
	{
		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;
		VK_CHECK(vkCreateFence(mDevice->GetHandle(), &fenceInfo, VULKAN_ALLOCATION_CALLBACKS, &mHandle));
	}

	Fence::~Fence()
	{
		vkDestroyFence(mDevice->GetHandle(), mHandle, VULKAN_ALLOCATION_CALLBACKS);
	}

	inline FenceManager* Fence::GetOwner()
	{ 
		return &mDevice->GetFenceManager();
	}

	FenceManager::FenceManager(Device* device)
		: mDevice(device)
	{
	}

	FenceManager::~FenceManager()
	{
		assert(mActiveFences.size() == 0LL);
	}

	void FenceManager::Init()
	{
		assert(mDevice->GetHandle() != VK_NULL_HANDLE);
	}

	void FenceManager::Destroy()
	{
		assert(mActiveFences.size() == 0LL && "Not all fences are signaled.");

		// Destroy all idle fences.
		for (Fence* fence : mFreeFences)
		{
			delete fence;
		}
	}

	Fence* FenceManager::AllocateFence(bool signaled)
	{
		if (!mFreeFences.empty())
		{
			Fence* fence = mFreeFences.back();
			mFreeFences.pop_back();
			mActiveFences.push_back(fence);
			if (signaled)
			{
				fence->mState = Fence::State::Signaled;
			}
			return fence;
		}
		Fence* fence = new Fence(mDevice, signaled);
		mActiveFences.push_back(fence);
		return fence;
	}

	bool FenceManager::IsFenceSignaled(Fence* fence)
	{
		assert(std::find(mActiveFences.begin(), mActiveFences.end(), fence) != mActiveFences.end());

		if (fence->IsSignaled())
		{
			return true;
		}

		VkResult result = vkGetFenceStatus(mDevice->GetHandle(), fence->mHandle);
		switch (result)
		{
		case VK_SUCCESS:
			fence->mState = Fence::State::Signaled;
			return true;
		case VK_NOT_READY:
			break;
		default:
			VK_CHECK_RESULT(result);
			break;
		}
		return false;
	}

	bool FenceManager::WaitForFence(Fence* fence, uint64 timeInNanoseconds)
	{
		assert(std::find(mActiveFences.begin(), mActiveFences.end(), fence) != mActiveFences.end());

		if (fence->IsSignaled())
		{
			return true;
		}

		VkResult result = vkWaitForFences(mDevice->GetHandle(), 1, &fence->mHandle, VK_TRUE, timeInNanoseconds);
		switch (result)
		{
		case VK_SUCCESS:
			fence->mState = Fence::State::Signaled;
			return true;
		case VK_TIMEOUT:
			break;
		default:
			VK_CHECK_RESULT(result);
			break;
		}
		return false;
	}

	void FenceManager::ResetFence(Fence* fence)
	{
		assert(std::find(mActiveFences.begin(), mActiveFences.end(), fence) != mActiveFences.end());

		if (IsFenceSignaled(fence))
		{
			VK_CHECK(vkResetFences(mDevice->GetHandle(), 1, &fence->mHandle));
			fence->mState = Fence::State::Unsignaled;
		}
	}

	void FenceManager::FreeFence(Fence*& fence)
	{
		assert(std::find(mActiveFences.begin(), mActiveFences.end(), fence) != mActiveFences.end());

		ResetFence(fence);
		mActiveFences.erase(std::find(mActiveFences.begin(), mActiveFences.end(), fence));
		mFreeFences.push_back(fence);
		fence = nullptr;
	}

	void FenceManager::WaitAndFreeFence(Fence*& fence, uint64 timeInNanoseconds)
	{
		assert(std::find(mActiveFences.begin(), mActiveFences.end(), fence) != mActiveFences.end());

		if (!IsFenceSignaled(fence))
		{
			WaitForFence(fence, timeInNanoseconds);
		}
		ResetFence(fence);
		mActiveFences.erase(std::find(mActiveFences.begin(), mActiveFences.end(), fence));
		mFreeFences.push_back(fence);
		fence = nullptr;
	}
}
