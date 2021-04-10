#pragma once

#include "Horizon/RenderBackend/RenderDevice.h"

#include "VulkanCommon.h"
#include "VulkanMemory.h"
#include "VulkanFence.h"

namespace Horizon
{
    using VulkanDeviceExtensions = Vector<const char*>;

    class VulkanPhysicalDeviceFeatures
    {
    public:
        void Add(void* feature)
        {
            if (!mFeatureStructs.empty())
            {
                auto* header = reinterpret_cast<FeatureStructHeader*>(mFeatureStructs.back());
                header->pNext = feature;
            }
            mFeatureStructs.emplace_back(feature);;
        }
        void* GetEntry() const { return mFeatureStructs.empty() ? nullptr : mFeatureStructs[0]; }
    private:
        struct FeatureStructHeader
        {
            VkStructureType sType;
            void* pNext;
        };
        Vector<void*> mFeatureStructs;
    };

    class VulkanBackend;

    class Device : public RenderDevice
    {
    public:

        /** The number of queue familys */
        static const uint32 kQueueFamilyCount = (uint32)QueueFamily::Count;

        /** The index of graphics queue should be 0. */
        static_assert((uint32)QueueFamily::Graphics == 0, "Default graphics queue index should be 0.");

        Device(VulkanBackend* backend, VkInstance instance, VkPhysicalDevice gpu);
        ~Device();

        void Init(const VulkanDeviceExtensions& extensions, const VulkanPhysicalDeviceFeatures& features);
        void Destroy();

        void WaitIdle();

        bool IsDeviceExtensionEnabled(const char* extension);

        RenderViewportHandle CreateRenderViewport(void* window) override final;

        void ResizeViewport(RenderViewportHandle handle, uint32 width, uint32 height, bool isFullscreen) override final;

        inline VkInstance GetInstanceHandle() const { return mInstance; }
        inline VkDevice GetHandle() const { return mHandle; }
        inline VkPhysicalDevice GetPhysicalDeviceHandle() const { return mGpu; }
        inline VmaAllocator GetMemoryAllocatorHandle() const { return mMemoryAllocator; }

        inline FenceManager& GetFenceManager() { return mFenceManager; }

        bool WaitForCommandBuffer(CommandBuffer* cb, uint64 timeInNanoseconds = UINT64_MAX);

        inline uint32 GetQueueFamilyIndex(QueueFamily family) const { return mQueueFamilyIndices[(uint32)family]; }
        inline uint32 GetGraphicsQueueFamilyIndex() const { return mQueueFamilyIndices[(uint32)QueueFamily::Graphics]; }
        inline uint32 GetComputeQueueFamilyIndex() const { return mQueueFamilyIndices[(uint32)QueueFamily::Compute]; }
        inline uint32 GetTransferQueueFamilyIndex() const { return mQueueFamilyIndices[(uint32)QueueFamily::Transfer]; }

        inline CommandQueue* GetCommandQueue(QueueFamily family, uint32 index) const { return mCommandQueues[(uint32)family][index]; }
        inline CommandQueue* GetGraphicsQueue(uint32 index = 0) const { return mCommandQueues[(uint32)QueueFamily::Graphics][index]; }
        inline CommandQueue* GetComputeQueue(uint32 index = 0) const { return mCommandQueues[(uint32)QueueFamily::Compute][index]; }
        inline CommandQueue* GetTransferQueue(uint32 index = 0) const { return mCommandQueues[(uint32)QueueFamily::Transfer][index]; }

        inline uint32 GetVendorID() const { return mGpuProperties.vendorID; }
        inline const VkPhysicalDeviceLimits& GetPhysicalDeviceLimits() const { return mGpuProperties.limits; }
        inline const VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() const { return mGpuProperties; }
        inline const VkPhysicalDeviceFeatures2& GetPhysicalDeviceFeatures() const { return mGpuFeatures; }

#if defined(VULKAN_ENABLE_DEBUG_MARKER)
        class DebugMarker
        {
        public:
            void SetObjectName(uint64 object, VkDebugReportObjectTypeEXT objectType, const char* name);
            void SetObjectTag(uint64 object, VkDebugReportObjectTypeEXT objectType, uint64 tagName, uint64 tagSize, const void* tag);
            void Begin(VkCommandBuffer cb, const char* pMarkerName, const float color[4]);
            void Insert(VkCommandBuffer cb, const char* pMarkerName, const float color[4]);
            void End(VkCommandBuffer cb);
        private:
            friend class Device;
            bool initialized = false;
            VkDevice device = VK_NULL_HANDLE;
            PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectName = VK_NULL_HANDLE;
            PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTag = VK_NULL_HANDLE;
            PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBegin = VK_NULL_HANDLE;
            PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsert = VK_NULL_HANDLE;
            PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEnd = VK_NULL_HANDLE;
        };
        inline const DebugMarker& GetDebugMarker() const { return mDebugMarker; }
#endif // VULKAN_ENABLE_DEBUG_MARKER

    private:

        void InitPhysicalDevice_Internal();
        void CreteaLogicalDevice_Internal(const VulkanDeviceExtensions& extensions, const VulkanPhysicalDeviceFeatures& features);
        void CreateMemoryAllocator_Internal();

        VulkanBackend* mBackend;

        VkInstance          mInstance;
        VkPhysicalDevice    mGpu;
        VkDevice            mHandle;
        VmaAllocator        mMemoryAllocator;

        FenceManager mFenceManager;

        /// The number of queues that need to be created. At least one graphics queue is required.
        /// We should create at most one queue per queue family for now.
        uint32 mQueueCount[(uint32)QueueFamily::Count] = { 1, 1, 1 };
        uint32 mQueueFamilyIndices[kQueueFamilyCount];
        Vector<CommandQueue*> mCommandQueues[kQueueFamilyCount];
        Vector<VkQueueFamilyProperties> mQueueFamilyProperties;

        bool mSeparateGraphicsComputeQueue = true;

        Vector<const char*> mEnabledDeviceExtensions;
        Vector<const char*> mEnabledValidationLayers;

        VkPhysicalDeviceProperties mGpuProperties;
        VkPhysicalDeviceFeatures2 mGpuFeatures;

        Vector<Viewport*> mViewports;

#if defined(VULKAN_ENABLE_DEBUG_MARKER)
        DebugMarker mDebugMarker;
        void SetupDebugMarker();
#endif // VULKAN_ENABLE_DEBUG_MARKER
    };
}
