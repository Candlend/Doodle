set_project("Doodle")
set_version("0.1.0")
add_rules("mode.debug", "mode.release")
add_requires("fmt", "assimp", "boost", "stb", "spdlog", "nlohmann_json", "glfw", "glad", "glm", "entt")
add_requires("imgui v1.91.0-docking", {configs = {glfw_opengl3 = true}})
add_requires("imnodes")
add_requires("imguizmo", {configs = {cxxflags = "-DIMGUI_HAS_VIEWPORT -DIMGUI_DEFINE_MATH_OPERATORS"}})
add_requireconfs("pybind11.python", {override = true, version = "3.10"})
add_requires("pybind11")
if is_os("windows") then
    add_defines("DOO_PLATFORM_WINDOWS")
elseif is_os("linux") then
    add_defines("DOO_PLATFORM_LINUX")
elseif is_os("macosx") then
    add_defines("DOO_PLATFORM_MACOS")
end

set_optimize("fastest")
set_languages("c++20")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

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

target("Doodle")
    set_kind("shared")
    add_defines("DOO_BUILD_DLL")

    -- 添加源文件
    add_files("Doodle/src/**.cpp")
    -- 添加所有子目录到包含路径
    traverse_directory("Doodle/src")
    
    -- 添加预编译头
    set_pcxxheader("Doodle/src/pch.h")

    -- 添加依赖库
    add_packages("fmt", "assimp", "boost", "stb", "spdlog", "imgui", "imnodes", "imguizmo", "nlohmann_json", "glfw", "glad", "glm", "entt")
    add_packages("pybind11")

    if is_mode("debug") then
        add_defines("DOO_ENABLE_ASSERTS")
        add_defines("DOO_HIDE_SPLASH")
        set_optimize("none")
    else 
        set_optimize("fastest")
    end

target("doodle")
    add_defines("DOO_BUILD_DLL")
    add_defines("DOO_BUILD_PYTHON")
    -- 添加源文件
    add_files("Doodle/src/**.cpp")
    add_files("Doodle/pybind11/**.cpp")
    -- 添加所有子目录到包含路径
    traverse_directory("Doodle/src")
    
    -- 添加预编译头
    set_pcxxheader("Doodle/src/pch.h")

    -- 添加依赖库
    add_packages("fmt", "assimp", "boost", "stb", "spdlog", "imgui", "imnodes", "imguizmo", "nlohmann_json", "glfw", "glad", "glm", "entt")

    if is_mode("debug") then
        add_defines("DOO_ENABLE_ASSERTS")
        add_defines("DOO_HIDE_SPLASH")
        set_optimize("none")
    else 
        set_optimize("fastest")
    end
    add_deps("Doodle")
    add_rules("python.library", {soabi = true})
    add_packages("pybind11")
    
    after_build(function (target)
        local targetdir = target:targetdir()
        local pythondir = targetdir .. "/python"
        os.mkdir(pythondir)
        os.cp(target:targetfile(), pythondir .. "/doodle.pyd")
        os.cp("assets/", pythondir)
        os.cp("config/", pythondir)
        os.cp("python/*", pythondir)
    end)
    
target("Sandbox")
    set_kind("binary")

    -- 添加源文件
    add_files("Sandbox/src/**.cpp")

    -- 添加依赖库
    add_deps("Doodle")
    traverse_directory("Doodle/src")

    add_packages("boost", "spdlog", "imgui", "imnodes", "imguizmo", "glm", "entt")

    if is_mode("debug") then
        add_defines("DOO_ENABLE_ASSERTS")
        add_defines("DOO_HIDE_SPLASH")
        set_optimize("none")
    else 
        set_optimize("fastest")
    end