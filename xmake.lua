set_project("Doodle")
set_version("0.1.0")
add_rules("mode.debug", "mode.release")
add_requires("fmt", "assimp", "boost", "stb", "spdlog", "nlohmann_json", "glfw", "glm", "entt", "magic_enum","yaml-cpp")
add_requires("reflect-cpp 2880c0b55b4317af46349ad5e517f9891da9af52")
add_requires("nativefiledialog-extended", {configs = {debug = true}})
add_requires("glad", {configs = {extensions = "GL_ARB_bindless_texture"}})
add_requires("imgui v1.91.0-docking", {configs = {glfw_opengl3 = true}})
add_requires("imnodes")
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

function build_doodle() 
    add_defines("DOO_BUILD_DLL")

    -- 添加源文件
    add_files("Doodle/src/**.cpp")
    -- 添加所有子目录到包含路径
    traverse_directory("Doodle/src")
    
    -- 添加预编译头
    set_pcxxheader("Doodle/src/pch.h")

    -- 添加依赖库
    add_packages("fmt", "assimp", "boost", "stb", "spdlog", "imgui", "imnodes", "nlohmann_json", "glfw", "glad", "glm", "entt", "nativefiledialog-extended", "magic_enum", "reflect-cpp", "yaml-cpp", "Watchdog")
    add_packages("pybind11")

    if is_mode("debug") then
        add_defines("DOO_ENABLE_ASSERTS")
        add_defines("DOO_HIDE_SPLASH")
        set_optimize("none")
    else 
        set_optimize("fastest")
    end

    add_defines("IMGUI_DEFINE_MATH_OPERATORS", "UNICODE")
    add_files("deps/ImGuizmo/*.cpp")
    add_headerfiles("deps/ImGuizmo/*.h")
    add_includedirs("deps/ImGuizmo")
end

target("Doodle")
    set_kind("shared")
    build_doodle()

target("doodle")
    add_deps("Doodle")
    add_defines("DOO_BUILD_PYTHON")
    add_files("Doodle/pybind11/**.cpp")
    add_rules("python.library", {soabi = true})
    add_packages("pybind11")

    build_doodle()
    
    after_build(function (target)
        local targetdir = target:targetdir()
        local pythondir = targetdir .. "/python"
        os.mkdir(pythondir)
        os.cp(target:targetfile(), pythondir .. "/doodle.pyd")
        os.cp("assets/", pythondir)
        os.cp("config/", pythondir)
        os.cp("python/*", pythondir)
    end)

    
target("DoodleEditor")
    set_kind("binary")

    -- 添加源文件
    add_files("DoodleEditor/src/**.cpp")

    -- 添加依赖库
    add_deps("Doodle")
    traverse_directory("Doodle/src")
    add_headerfiles("deps/ImGuizmo/*.h")
    add_includedirs("deps/ImGuizmo")
    
    add_packages("boost", "spdlog", "imgui", "imnodes", "imguizmo", "glm", "entt", "reflect-cpp", "yaml-cpp")

    if is_mode("debug") then
        add_defines("DOO_ENABLE_ASSERTS")
        add_defines("DOO_HIDE_SPLASH")
        set_optimize("none")
    else 
        set_optimize("fastest")
    end