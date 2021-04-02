project "VulkanBase"
    location ("%{wks.location}/Source/Tutorials/VulkanBase")
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    
    targetdir ("%{wks.location}/Build/Bin/%{prj.name}")
    objdir ("%{wks.location}/Build/Bin/%{prj.name}")

    links
    {
        "Engine",
        "%{libdir.vulkan}",
    }

    files
    {
        "%{wks.location}/Source/Tutorials/%{prj.name}/**.h",
        "%{wks.location}/Source/Tutorials/%{prj.name}/**.c",
        "%{wks.location}/Source/Tutorials/%{prj.name}/**.hpp", 
		"%{wks.location}/Source/Tutorials/%{prj.name}/**.cpp",
    }

    includedirs
    {
        "%{includedir.Engine}",
        "%{includedir.vulkan}",
        "%{includedir.spdlog}",
        "%{includedir.vma}",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"