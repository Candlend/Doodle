set_project("RhyEngine")
set_version("0.1.0")
add_rules("mode.debug", "mode.release")
add_requires("spdlog", "imgui", "cereal", "nlohmann_json")

if is_os("windows") then
    add_defines("RHY_PLATFORM_WINDOWS")
elseif is_os("linux") then
    add_defines("RHY_PLATFORM_LINUX")
elseif is_os("macosx") then
    add_defines("RHY_PLATFORM_MACOS")
end



set_optimize("fastest")
set_languages("c++20")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
add_rules("plugin.vsxmake.autoupdate")

-- 在构建后调用创建符号链接的函数
after_build(function (target)
    os.cp("assets/", target:targetdir())
    os.cp("config/", target:targetdir())
end)

target("RhyEngine")
    set_kind("shared")
    add_defines("RHY_BUILD_DLL")

    -- 添加源文件
    add_files("RhyEngine/src/**.cpp")
    add_headerfiles("RhyEngine/src/**.h")
    add_includedirs("RhyEngine/src")
    
    -- 添加预编译头
    set_pcxxheader("RhyEngine/src/pch.h")

    -- 添加依赖库
    add_packages("spdlog", "imgui", "cereal", "nlohmann_json")
    
target("Sandbox")
    set_kind("binary")

    -- 添加源文件
    add_files("Sandbox/src/**.cpp")

    -- 添加依赖库
    add_deps("RhyEngine")
    add_includedirs("RhyEngine/src")

    add_packages("spdlog", "imgui", "cereal")
