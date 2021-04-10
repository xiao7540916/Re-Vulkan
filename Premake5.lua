workspace "Horizon"
	architecture "x64"
    targetdir "Build"

	configurations
	{
		"Debug",
		"Release",
	}

	flags
	{
		"MultiProcessorCompile",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includedir = {}
includedir["Source"] = "%{wks.location}/Source"
-- Third party
includedir["ThirdParty"] = "%{wks.location}/Source/ThirdParty"
includedir["vulkan"] = "%{wks.location}/Source/ThirdParty/vulkan/include"
includedir["vma"] = "%{wks.location}/Source/ThirdParty/vma/include"
includedir["glfw"] = "%{wks.location}/Source/ThirdParty/glfw/include"
includedir["spdlog"] = "%{wks.location}/Source/ThirdParty/spdlog/include"
includedir["glm"] = "%{wks.location}/Source/ThirdParty/glm/include"
-- Horizon Engine
includedir["Horizon"] = "%{wks.location}/Source/Horizon"
-- VulkanBackend
includedir["VulkanBase"] = "%{wks.location}/Source/VulkanBackend/VulkanBase"
includedir["VulkanInstance"] = "%{wks.location}/Source/VulkanBackend/VulkanInstance"
includedir["VulkanDevice"] = "%{wks.location}/Source/VulkanBackend/VulkanDevice"
includedir["VulkanQueue"] = "%{wks.location}/Source/VulkanBackend/VulkanQueue"
includedir["VulkanSwapchain"] = "%{wks.location}/Source/VulkanBackend/VulkanSwapchain"
includedir["VulkanCommandBuffer"] = "%{wks.location}/Source/VulkanBackend/VulkanCommandBuffer"

libdir = {}
libdir["vulkan"] = "%{wks.location}/Binaries/ThirdParty/vulkan/vulkan-1.lib"

group "ThirdParty"
    include "Source/ThirdParty"
group ""

group "VulkanBackend"
    include "Source/VulkanBackend/VulkanBase"
    include "Source/VulkanBackend/VulkanInstance"
    include "Source/VulkanBackend/VulkanDevice"
    include "Source/VulkanBackend/VulkanQueue"
    include "Source/VulkanBackend/VulkanSwapchain"
    include "Source/VulkanBackend/VulkanCommandBuffer"
group ""

group "Testing"
    include "Source/Testing"
group ""

project "Horizon"
    location ("%{wks.location}/Source/Horizon")
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/Build/Bin/%{prj.name}")
    objdir ("%{wks.location}/Build/Bin/%{prj.name}")

    flags 
    { 
        "NoPCH",
    }

    files
    {
        "%{wks.location}/Source/%{prj.name}/**.h",
        "%{wks.location}/Source/%{prj.name}/**.c",
        "%{wks.location}/Source/%{prj.name}/**.hpp", 
		"%{wks.location}/Source/%{prj.name}/**.cpp",
    }

    includedirs
    {
        "%{includedir.Source}",
        "%{includedir.Horizon}",
        -- Third party
        "%{includedir.vulkan}",
        "%{includedir.vma}",
        "%{includedir.glfw}",
        "%{includedir.spdlog}",
        "%{includedir.glm}",
    }

    links
    {
        "glfw",
        "%{libdir.vulkan}",
    }

    filter "system:windows"
        systemversion "latest"
        
    filter "configurations:Debug"
        defines "HORIZON_DEBUG_MODE"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "HORIZON_RELEASE_MODE"
        runtime "Release"
        optimize "on"
