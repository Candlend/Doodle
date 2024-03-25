set_project("RhyEngine")
set_version("0.1.0")
add_rules("mode.debug", "mode.release")
add_requires("spdlog", "imgui docking", "cereal", "nlohmann_json", "glfw", "glad")

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

function traverse_directory(path)
    add_headerfiles(path .. "/**.h")
    add_includedirs(path)
    for _, dir in ipairs(os.dirs(path .. "/*")) do
        add_headerfiles(dir .. "/**.h")
        add_includedirs(dir)
    end
end

target("RhyEngine")
    set_kind("shared")
    add_defines("RHY_BUILD_DLL")

    -- 添加源文件
    add_files("RhyEngine/src/**.cpp")
    -- 添加所有子目录到包含路径
    traverse_directory("RhyEngine/src")
    
    -- 添加预编译头
    set_pcxxheader("RhyEngine/src/pch.h")

    -- 添加依赖库
    add_packages("spdlog", "imgui", "cereal", "nlohmann_json", "glfw", "glad")

    if is_mode("debug") then
        set_optimize("none")
    end
    
target("Sandbox")
    set_kind("binary")

    -- 添加源文件
    add_files("Sandbox/src/**.cpp")

    -- 添加依赖库
    add_deps("RhyEngine")
    traverse_directory("RhyEngine/src")

    add_packages("spdlog", "imgui", "cereal")
