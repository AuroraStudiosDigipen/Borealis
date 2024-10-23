
workspace "Borealis"
	architecture "x64"

	startproject "BorealisEditor"

	configurations
	{
		"Debug",
		"Release",
		"Distribution"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["GLFW"] = "Borealis/lib/GLFW/include"
	IncludeDir["GLAD"] = "Borealis/lib/GLAD/include"
	IncludeDir["ImGui"] = "Borealis/lib/imgui"
	IncludeDir["GLM"] = "Borealis/lib/glm"
	IncludeDir["ENTT"] = "Borealis/lib/ENTT/include"
	IncludeDir["YAML"] = "Borealis/lib/yaml-cpp/include"
	IncludeDir["ImGuizmo"] = "Borealis/lib/ImGuizmo"
	IncludeDir["FMOD"] = "Borealis/lib/FMOD"
	IncludeDir["Tracy"] = "Borealis/lib/tracy"
	IncludeDir["STBI"] = "Borealis/lib/stb_image"
	IncludeDir["JoltPhysics"] = "Borealis/lib/JoltPhysics"
	IncludeDir["Mono"] = "Borealis/lib/mono/include"
	IncludeDir["xproperty"] = "Borealis/lib/xproperty/include"
	IncludeDir["Gli"] = "Borealis/lib/gli"
	IncludeDir["RTTR"] = "Borealis/lib/RTTR/include"

	IncludeDir["assimp"] = "BorealisEditor/lib/assimp/include"
	IncludeDir["ImGuiNodeEditor"] = "BorealisEditor/lib/imgui-node-editor"
	IncludeDir["MSDF"] = "BorealisEditor/lib/MSDF/Include"

	IncludeDir["STBI_Compiler"] = "BorealisAssetCompiler/lib/stb_image"
  	IncludeDir["ISPC"] = "BorealisAssetCompiler/lib/ispc"
  	IncludeDir["compressonator"] = "BorealisAssetCompiler/lib/compressonator"
	IncludeDir["meshoptimizer"] = "BorealisAssetCompiler/lib/MeshOptimizer/Include"

	LibraryDir = {}
	LibraryDir["FMOD"] = "lib/FMOD/lib"
	LibraryDir["Mono_Debug"] = "lib/mono/lib/Deb"
	LibraryDir["Mono_Release"] = "lib/mono/lib/Rel"
	LibraryDir["MSDF_Debug"] = "lib/MSDF/Build/Debug"
	LibraryDir["MSDF_Release"] = "lib/MSDF/Build/Release"
	LibraryDir["meshoptimizer_Debug"] = "lib/MeshOptimizer/Build/Debug"
	LibraryDir["meshoptimizer_Release"] = "lib/MeshOptimizer/Build/Release"
	LibraryDir["GLAD_Debug"] = "lib/GLAD/lib/Deb"
	LibraryDir["GLAD_Release"] = "lib/GLAD/lib/Rel"
	LibraryDir["GLFW_Debug"] = "lib/GLFW/lib/Deb"
	LibraryDir["GLFW_Release"] = "lib/GLFW/lib/Rel"
	LibraryDir["Jolt_Debug"] = "lib/JoltPhysics/lib/Deb"
	LibraryDir["Jolt_Release"] = "lib/JoltPhysics/lib/Rel"
	LibraryDir["YAML_Debug"] = "lib/yaml-cpp/lib/Deb"
	LibraryDir["YAML_Release"] = "lib/yaml-cpp/lib/Rel"
	LibraryDir["IMGUI_Debug"] = "lib/IMGUI/lib/Deb"
	LibraryDir["IMGUI_Release"] = "lib/IMGUI/lib/Rel"
	LibraryDir["RTTR_Debug"] = "lib/RTTR/lib/Deb"
	LibraryDir["RTTR_Release"] = "lib/RTTR/lib/Rel"

	Library = {}
	Library["FMOD_Debug"] = "%{LibraryDir.FMOD}/fmodL_vc.lib"
	Library["FMOD_Release"] = "%{LibraryDir.FMOD}/fmod_vc.lib"
	Library["Mono_Debug"] = "%{LibraryDir.Mono_Debug}/mono-2.0-sgen.lib"
	Library["Mono_Release"] = "%{LibraryDir.Mono_Release}/mono-2.0-sgen.lib"

	Library["MSDF_Debug_atlas"] = "%{LibraryDir.MSDF_Debug}/msdf-atlas-gen.lib"
	Library["MSDF_Debug_core"] = "%{LibraryDir.MSDF_Debug}/msdfgen-core.lib"
	Library["MSDF_Debug_ext"] = "%{LibraryDir.MSDF_Debug}/msdfgen-ext.lib"
	Library["MSDF_Debug_FreeType"] = "%{LibraryDir.MSDF_Debug}/freetyped.lib"
	Library["MSDF_Debug_LibPNG"] = "%{LibraryDir.MSDF_Debug}/libpng16d.lib"
	Library["MSDF_Debug_LibBZ2"] = "%{LibraryDir.MSDF_Debug}/bz2d.lib"
	Library["MSDF_Debug_LibBrotli"] = "%{LibraryDir.MSDF_Debug}/brotlidec.lib"
	Library["MSDF_Debug_LibBrotliCommon"] = "%{LibraryDir.MSDF_Debug}/brotlicommon.lib"

	Library["MSDF_Release_atlas"] = "%{LibraryDir.MSDF_Release}/msdf-atlas-gen.lib"
	Library["MSDF_Release_core"] = "%{LibraryDir.MSDF_Release}/msdfgen-core.lib"
	Library["MSDF_Release_ext"] = "%{LibraryDir.MSDF_Release}/msdfgen-ext.lib"
	Library["MSDF_Release_FreeType"] = "%{LibraryDir.MSDF_Release}/freetype.lib"
	Library["MSDF_Release_LibPNG"] = "%{LibraryDir.MSDF_Release}/libpng16.lib"
	Library["MSDF_Release_LibBZ2"] = "%{LibraryDir.MSDF_Release}/bz2.lib"
	Library["MSDF_Release_LibBrotli"] = "%{LibraryDir.MSDF_Release}/brotlidec.lib"
	Library["MSDF_Release_LibBrotliCommon"] = "%{LibraryDir.MSDF_Release}/brotlicommon.lib"

	Library["meshoptimizer_Debug"] = "%{LibraryDir.meshoptimizer_Debug}/meshoptimizer.lib"
	Library["meshoptimizer_Release"] = "%{LibraryDir.meshoptimizer_Release}/meshoptimizer.lib"

	Library["GLAD_Debug"] = "%{LibraryDir.GLAD_Debug}/GLAD.lib"
	Library["GLAD_Release"] = "%{LibraryDir.GLAD_Release}/GLAD.lib"
	Library["GLFW_Debug"] = "%{LibraryDir.GLFW_Debug}/GLFW.lib"
	Library["GLFW_Release"] = "%{LibraryDir.GLFW_Release}/GLFW.lib"
	Library["Jolt_Debug"] = "%{LibraryDir.Jolt_Debug}/JoltPhysics.lib"
	Library["Jolt_Release"] = "%{LibraryDir.Jolt_Release}/JoltPhysics.lib"
	Library["YAML_Debug"] = "%{LibraryDir.YAML_Debug}/yaml-cpp.lib"
	Library["YAML_Release"] = "%{LibraryDir.YAML_Release}/yaml-cpp.lib"
	Library["IMGUI_Debug"] = "%{LibraryDir.IMGUI_Debug}/IMGUI.lib"
	Library["IMGUI_Release"] = "%{LibraryDir.IMGUI_Release}/IMGUI.lib"
	Library["RTTR_Debug"] = "%{LibraryDir.RTTR_Debug}/librttr_core.lib"
	Library["RTTR_Release"] = "%{LibraryDir.RTTR_Release}/librttr_core.lib"

	group "Dependencies"
		include "Borealis/lib/Tracy"
		include "BorealisEditor/lib/Assimp"
		include "BorealisEditor/lib/imgui-node-editor"
	group ""

	project "Borealis"
		location "Borealis"
		kind "StaticLib"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"
		systemversion "latest"

		targetdir ("build/" .. outputdir .. "/%{prj.name}")
		objdir ("build-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/**.hpp",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/lib/glm/glm/**.hpp",
			"%{prj.name}/lib/glm/glm/**.inl",
			"%{prj.name}/lib/imGuizmo/imGuizmo.h",
			"%{prj.name}/lib/imGuizmo/imGuizmo.cpp",
			"%{prj.name}/lib/stb_image/**.h",
			"%{prj.name}/lib/stb_image/**.cpp"
		}

		defines
		{
			"_CRT_SECURE_NO_WARNINGS",
			"YAML_CPP_STATIC_DEFINE",
			"JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
			"JPH_CROSS_PLATFORM_DETERMINISTIC",
			"JPH_ENABLE_ASSERTS",
			"TRACY_ENABLE"
		}

		includedirs
		{
			"%{prj.name}/inc",
			"%{prj.name}/lib/spdlog/include",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.GLAD}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.ENTT}",
			"%{IncludeDir.YAML}",
			"%{IncludeDir.ImGuizmo}",
			"%{IncludeDir.FMOD}",
			"%{IncludeDir.Tracy}",
			"%{IncludeDir.STBI}",
			"%{IncludeDir.JoltPhysics}",
			"%{IncludeDir.Mono}",
			"%{IncludeDir.xproperty}",
			"%{IncludeDir.Gli}",
			"%{IncludeDir.RTTR}"
		}


		links
		{
			"opengl32.lib",
			"Tracy",
		}


		defines
		{
			"BOREALIS_BUILD_DLL",
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"GLFW_INCLUDE_NONE",
			
		}

		pchheader "BorealisPCH.hpp"
		pchsource "Borealis/src/BorealisPCH.cpp"

		filter "files:Borealis/lib/ImGuizmo/**.cpp"
		flags {"NoPCH"}
		
		

		filter "configurations:Debug"
			defines "_DEB"
			symbols "On"
			runtime "Debug"
			links
			{
				"%{Library.FMOD_Debug}",
				"%{Library.Mono_Debug}",
				"%{Library.GLAD_Debug}",
				"%{Library.GLFW_Debug}",
				"%{Library.Jolt_Debug}",
				"%{Library.YAML_Debug}",
				"%{Library.IMGUI_Debug}",
				"%{Library.RTTR_Debug}"
			}
			postbuildcommands {
				"{COPY} \"engineResources\" \"../BorealisEditor/engineResources\""
			 }

		filter "configurations:Release"
			defines "_REL"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.FMOD_Release}",
				"%{Library.Mono_Release}",
				"%{Library.GLAD_Release}",
				"%{Library.GLFW_Release}",
				"%{Library.Jolt_Release}",
				"%{Library.YAML_Release}",
				"%{Library.IMGUI_Release}",
				"%{Library.RTTR_Release}"
			}
			postbuildcommands {
				"{COPY} \"engineResources\" \"../BorealisEditor/engineResources\""
			 }

		filter "configurations:Distribution"
			defines "_DIST"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.FMOD_Release}",
				"%{Library.Mono_Release}",
				"%{Library.GLAD_Release}",
				"%{Library.GLFW_Release}",
				"%{Library.Jolt_Release}",
				"%{Library.YAML_Release}",
				"%{Library.IMGUI_Release}",
				"%{Library.RTTR_Release}"
			}
			postbuildcommands {
				"{COPY} \"engineResources\" \"../BorealisEditor/engineResources\""
			 }

	project "BorealisEditor"
		location "BorealisEditor"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"
		systemversion "latest"

		targetdir ("build/" .. outputdir .. "/%{prj.name}")
		objdir ("build-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/inc/**.hpp",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"Borealis",
			"Borealis/inc",
			"Borealis/lib/spdlog/include",
			"%{IncludeDir.YAML}",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.ImGui}",
			"%{prj.name}/inc",
			"%{IncludeDir.ENTT}",
			"%{IncludeDir.ImGuizmo}",
			"%{IncludeDir.ImGuiNodeEditor}",
			"%{IncludeDir.assimp}",
			"%{IncludeDir.MSDF}",
		}

		defines
		{
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"YAML_CPP_STATIC_DEFINE",
			"JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
			"JPH_CROSS_PLATFORM_DETERMINISTIC",
			"JPH_ENABLE_ASSERTS"
		}

		links
		{
			"Borealis",
			"BorealisAssetCompiler",
			"ImGuiNodeEditor",
			"BorealisScriptCore",
			"BorealisRuntime",
			"Assimp"
		}

		filter "configurations:Debug"
			defines "_DEB"
			symbols "On"
			runtime "Debug"
			postbuildcommands {
				"{COPYFILE} \"../Borealis/lib/FMOD/dll/fmodL.dll\" \"$(TargetDir)\"",
				"{COPYFILE} \"../Borealis/lib/mono/dll/Deb/mono-2.0-sgen.dll\" \"$(TargetDir)\"",
				"{COPY} \"engineResources\" \"$(TargetDir)engineResources\"",
				"{COPY} \"resources\" \"$(TargetDir)resources\"",
				"{COPY} \"mono\" \"$(TargetDir)mono\"",
				"{COPY} \"settings\" \"$(TargetDir)settings\"",
				"{COPYFILE} \"BorealisAssetCompiler.exe\" \"$(TargetDir)BorealisAssetCompiler.exe\"",
				"{COPYFILE} \"BorealisRuntime.exe\" \"$(TargetDir)BorealisRuntime.exe\"",
				"{COPYFILE} \"imgui.ini\" \"$(TargetDir)imgui.ini\"",
				"{MKDIR} \"$(TargetDir)assets\""
			 }
			links
			{
				"%{Library.MSDF_Debug_atlas}",
				"%{Library.MSDF_Debug_core}",
				"%{Library.MSDF_Debug_ext}",
				"%{Library.MSDF_Debug_FreeType}",
				"%{Library.MSDF_Debug_LibPNG}",
				"%{Library.MSDF_Debug_LibBZ2}",
				"%{Library.MSDF_Debug_LibBrotli}",
				"%{Library.MSDF_Debug_LibBrotliCommon}",
				"Borealis/%{Library.YAML_Debug}",
				"Borealis/%{Library.Jolt_Debug}"
			}

		filter "configurations:Release"
			defines "_REL"
			optimize "On"
			runtime "Release"
			postbuildcommands {
				"{COPYFILE} \"../Borealis/lib/FMOD/dll/fmod.dll\" \"$(TargetDir)\"",
				"{COPYFILE} \"../Borealis/lib/mono/dll/Rel/mono-2.0-sgen.dll\" \"$(TargetDir)\"",
				"{COPY} \"engineResources\" \"$(TargetDir)engineResources\"",
				"{COPY} \"resources\" \"$(TargetDir)resources\"",
				"{COPY} \"mono\" \"$(TargetDir)mono\"",
				"{COPY} \"settings\" \"$(TargetDir)settings\"",
				"{COPYFILE} \"BorealisAssetCompiler.exe\" \"$(TargetDir)BorealisAssetCompiler.exe\"",
				"{COPYFILE} \"BorealisRuntime.exe\" \"$(TargetDir)BorealisRuntime.exe\"",
				"{COPYFILE} \"imgui.ini\" \"$(TargetDir)imgui.ini\"",
				"{MKDIR} \"$(TargetDir)assets\""
			 }
			 links
			{
				"%{Library.MSDF_Release_atlas}",
				"%{Library.MSDF_Release_core}",
				"%{Library.MSDF_Release_ext}",
				"%{Library.MSDF_Release_FreeType}",
				"%{Library.MSDF_Release_LibPNG}",
				"%{Library.MSDF_Release_LibBZ2}",
				"%{Library.MSDF_Release_LibBrotli}",
				"%{Library.MSDF_Release_LibBrotliCommon}",
				"Borealis/%{Library.YAML_Release}",
				"Borealis/%{Library.Jolt_Release}"
			}

		filter "configurations:Distribution"
			defines "_DIST"
			optimize "On"
			runtime "Release"
			postbuildcommands {
				"{COPYFILE} \"../Borealis/lib/FMOD/dll/fmod.dll\" \"$(TargetDir)\"",
				"{COPYFILE} \"../Borealis/lib/mono/dll/Rel/mono-2.0-sgen.dll\" \"$(TargetDir)\"",
				"{COPY} \"engineResources\" \"$(TargetDir)engineResources\"",
				"{COPY} \"resources\" \"$(TargetDir)resources\"",
				"{COPY} \"mono\" \"$(TargetDir)mono\"",
				"{COPY} \"settings\" \"$(TargetDir)settings\"",
				"{COPYFILE} \"BorealisAssetCompiler.exe\" \"$(TargetDir)BorealisAssetCompiler.exe\"",
				"{COPYFILE} \"BorealisRuntime.exe\" \"$(TargetDir)BorealisRuntime.exe\"",
				"{COPYFILE} \"imgui.ini\" \"$(TargetDir)imgui.ini\"",
				"{MKDIR} \"$(TargetDir)assets\""
			 }
			 links
			{
				"%{Library.MSDF_Release_atlas}",
				"%{Library.MSDF_Release_core}",
				"%{Library.MSDF_Release_ext}",
				"%{Library.MSDF_Release_FreeType}",
				"%{Library.MSDF_Release_LibPNG}",
				"%{Library.MSDF_Release_LibBZ2}",
				"%{Library.MSDF_Release_LibBrotli}",
				"%{Library.MSDF_Release_LibBrotliCommon}",
				"Borealis/%{Library.YAML_Release}",
				"Borealis/%{Library.Jolt_Release}"
			}

	project "BorealisScriptCore"
		location "BorealisScriptCore"
		kind "SharedLib"
		language "C#"
		dotnetframework "4.7.2"

		targetdir("BorealisEditor/Resources/Scripts/Core")
		objdir ("BorealisEditor/Resources/Scripts/Core/Intermediate")

		files
		{
			"%{prj.name}/src/**.cs",
			"%{prj.name}/props/**.cs"
		}

		links
		{
			"packages/Microsoft.CodeAnalysis.Common.4.11.0/lib/netstandard2.0/Microsoft.CodeAnalysis",
			"packages/Microsoft.CodeAnalysis.CSharp.4.11.0/lib/netstandard2.0/Microsoft.CodeAnalysis.CSharp",
			"packages/Microsoft.CodeAnalysis.Analyzers.3.3.4/analyzers/dotnet/cs/Microsoft.CodeAnalysis.Analyzers",
			"packages/Microsoft.CodeAnalysis.Analyzers.3.3.4/analyzers/dotnet/cs/Microsoft.CodeAnalysis.CSharp.Analyzers",
			"packages/System.Buffers.4.5.1/lib/netstandard2.0/System.Buffers",
			"packages/System.Collections.Immutable.8.0.0/lib/netstandard2.0/System.Collections.Immutable",
			"packages/System.Memory.4.5.5/lib/netstandard2.0/System.Memory",
			"packages/System.Reflection.Metadata.8.0.0/lib/netstandard2.0/System.Reflection.Metadata",
			"packages/System.Runtime.CompilerServices.Unsafe.6.0.0/lib/netstandard2.0/System.Runtime.CompilerServices.Unsafe"
		}

		filter "configurations:Debug"
			optimize "Off"
			symbols "Default"

		filter "configurations:Release"
			optimize "On"
			symbols "Default"

		filter "configurations:Distribution"
			optimize "Full"
			symbols "Off"

	project "BorealisRuntime"
		location "BorealisRuntime"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"
		systemversion "latest"

		targetdir("BorealisEditor")
		objdir ("BorealisEditor/Resources/Scripts/Core/Intermediate")

		files
		{
			"%{prj.name}/inc/**.hpp",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"Borealis",
			"Borealis/inc",
			"Borealis/lib/spdlog/include",
			"%{IncludeDir.GLM}",
			"%{IncludeDir.ImGui}",
			"%{prj.name}/inc",
			"%{IncludeDir.ENTT}"
		}

		defines
		{
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"JPH_FLOATING_POINT_EXCEPTIONS_ENABLED",
			"JPH_CROSS_PLATFORM_DETERMINISTIC",
			"JPH_ENABLE_ASSERTS"		
		}

		links
		{
			"Borealis"
		}

		filter "configurations:Debug"
			defines "_DEB"
			symbols "On"
			runtime "Debug"

		links
		{
			"Borealis/%{Library.Jolt_Debug}",
		}
		filter "configurations:Release"
			defines "_REL"
			optimize "On"
			runtime "Release"
		links
		{
			"Borealis/%{Library.Jolt_Release}",
		}

		filter "configurations:Distribution"
			defines "_DIST"
			optimize "On"
			runtime "Release"

	project "BorealisAssetCompiler"
		location "BorealisAssetCompiler"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++20"
		staticruntime "on"
		systemversion "latest"

		targetdir ("BorealisEditor")
		objdir ("build-int/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/inc/**.hpp",
			"%{prj.name}/src/**.cpp"
		}

		includedirs
		{
			"%{IncludeDir.YAML}",
			"%{IncludeDir.GLM}",
			"%{prj.name}/inc",
			"%{IncludeDir.assimp}",
			"%{IncludeDir.MSDF}",
			"%{IncludeDir.STBI_Compiler}",
			"%{IncludeDir.ISPC}",
			"%{IncludeDir.compressonator}",
			"%{IncludeDir.meshoptimizer}"
		}

		defines
		{
			"_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS",
			"YAML_CPP_STATIC_DEFINE"
		}

		libdirs
		{
			"BorealisAssetCompiler/lib/ispc/build",
			"BorealisAssetCompiler/lib/compressonator/build"
		}

		links
		{
			"Assimp",
			"ispc_texcomp",
			"Compressonator_MTd"
		}

		postbuildcommands {
			"{COPYFILE} \"../BorealisAssetCompiler/lib/ispc/dll/ispc_texcomp.dll\" \"$(TargetDir)\""
		 }

		filter "configurations:Debug"
			defines "_DEB"
			symbols "On"
			runtime "Debug"
			links
			{
				"%{Library.MSDF_Debug_atlas}",
				"%{Library.MSDF_Debug_core}",
				"%{Library.MSDF_Debug_ext}",
				"%{Library.MSDF_Debug_FreeType}",
				"%{Library.MSDF_Debug_LibPNG}",
				"%{Library.MSDF_Debug_LibBZ2}",
				"%{Library.MSDF_Debug_LibBrotli}",
				"%{Library.MSDF_Debug_LibBrotliCommon}",
				"%{Library.meshoptimizer_Debug}",
				"Borealis/%{Library.YAML_Debug}"
			}

		filter "configurations:Release"
			defines "_REL"
			optimize "On"
			runtime "Release"
			links
			{
				"%{Library.MSDF_Release_atlas}",
				"%{Library.MSDF_Release_core}",
				"%{Library.MSDF_Release_ext}",
				"%{Library.MSDF_Release_FreeType}",
				"%{Library.MSDF_Release_LibPNG}",
				"%{Library.MSDF_Release_LibBZ2}",
				"%{Library.MSDF_Release_LibBrotli}",
				"%{Library.MSDF_Release_LibBrotliCommon}",
				"%{Library.meshoptimizer_Release}",
				"Borealis/%{Library.YAML_Release}"
			}

		filter "configurations:Distribution"
			defines "_DIST"
			optimize "On"
			runtime "Release"
				links
			{
				"%{Library.MSDF_Release_atlas}",
				"%{Library.MSDF_Release_core}",
				"%{Library.MSDF_Release_ext}",
				"%{Library.MSDF_Release_FreeType}",
				"%{Library.MSDF_Release_LibPNG}",
				"%{Library.MSDF_Release_LibBZ2}",
				"%{Library.MSDF_Release_LibBrotli}",
				"%{Library.MSDF_Release_LibBrotliCommon}",
				"Borealis/%{Library.YAML_Release}"
			}
