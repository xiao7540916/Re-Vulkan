project "VulkanCommandBuffer"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("%{wks.location}/Build/Bin/%{prj.name}")
    objdir ("%{wks.location}/Build/Bin/%{prj.name}")

    links
    {
        "Horizon",
        "VulkanBase",
        "%{libdir.vulkan}",
    }

    files
    {
        "**.h",
        "**.c",
        "**.hpp", 
		"**.cpp",
    }

    includedirs
    {
        "%{includedir.Source}",
        "%{includedir.Horizon}",
        "%{includedir.vulkan}",
        "%{includedir.spdlog}",
        "%{includedir.vma}",
        "%{includedir.glm}",
        "%{includedir.VulkanBase}",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"