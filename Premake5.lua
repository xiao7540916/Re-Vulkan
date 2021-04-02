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
-- Third party
includedir["ThirdParty"] = "%{wks.location}/Source/ThirdParty"
includedir["vulkan"] = "%{wks.location}/Source/ThirdParty/vulkan/include"
includedir["glfw"] = "%{wks.location}/Source/ThirdParty/glfw/include"
includedir["spdlog"] = "%{wks.location}/Source/ThirdParty/spdlog/include"
includedir["vma"] = "%{wks.location}/Source/ThirdParty/vma/include"
includedir["glm"] = "%{wks.location}/Source/ThirdParty/glm/include"
-- Engine
includedir["Engine"] = "%{wks.location}/Source/Engine"
-- Tutorials
includedir["VulkanBase"] = "%{wks.location}/Source/Tutorials/VulkanBase"
includedir["VulkanInstance"] = "%{wks.location}/Source/Tutorials/VulkanInstance"
includedir["VulkanDevice"] = "%{wks.location}/Source/Tutorials/VulkanDevice"
includedir["VulkanQueue"] = "%{wks.location}/Source/Tutorials/VulkanQueue"

libdir = {}
libdir["vulkan"] = "%{wks.location}/Binaries/ThirdParty/vulkan/vulkan-1.lib"

group "ThirdParty"
    include "Source/ThirdParty"
group ""

group "Tutorials"
    include "Source/Tutorials/VulkanBase"
    include "Source/Tutorials/VulkanInstance"
    include "Source/Tutorials/VulkanDevice"
    include "Source/Tutorials/VulkanQueue"
group ""

project "Engine"
    location ("%{wks.location}/Source/Engine")
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
        "%{includedir.Engine}",
        -- Third party
        "%{includedir.glfw}",
        "%{includedir.spdlog}",
        "%{includedir.vma}",
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
