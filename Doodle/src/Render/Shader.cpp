#include "pch.h"
#include <glad/glad.h>
#include <memory>

#include "Shader.h"
#include <boost/algorithm/string.hpp>

namespace Doodle
{

class OpenGLShader : public Shader
{

public:
    explicit OpenGLShader(const std::string &filepath)
    {
        ReadShaderFromFile(filepath);
        std::function func = [this]() { CompileAndUploadShader(); };
        Renderer::Get().Submit(func);
    }

    virtual void Bind() override
    {
        std::function func = [this]() { glUseProgram(m_rendererID); };
        Renderer::Get().Submit(func);
    }

private:
    void ReadShaderFromFile(const std::string &filepath)
    {
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            m_shaderSource.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        }
        else
        {
            DOO_CORE_WARN("Could not read shader file {0}", filepath);
        }
    }

    void CompileAndUploadShader()
    {
        auto shaderSources = ParseShaderSources(m_shaderSource);
        GLuint program = glCreateProgram();
        std::vector<GLuint> shaderRendererIDs;

        for (const auto &kv : shaderSources)
        {
            GLuint shaderRendererID = CompileShader(kv.first, kv.second);
            if (shaderRendererID)
            {
                shaderRendererIDs.push_back(shaderRendererID);
                glAttachShader(program, shaderRendererID);
            }
        }

        LinkProgram(program, shaderRendererIDs);
        m_rendererID = program;
    }

    std::unordered_map<GLenum, std::string> ParseShaderSources(const std::string &source)
    {
        std::unordered_map<GLenum, std::string> shaderSources;
        const std::string TYPE_TOKEN = "#type";
        std::vector<std::string> lines;
        boost::split(lines, source, boost::is_any_of("\n"));
        std::string currentType;
        std::string currentSource;

        for (const auto &line : lines)
        {
            std::string trimmedLine = boost::algorithm::trim_copy(line);
            if (boost::starts_with(trimmedLine, TYPE_TOKEN))
            {
                if (!currentType.empty())
                {
                    shaderSources[ShaderTypeFromString(currentType)] = currentSource;
                    currentSource.clear();
                }
                currentType = trimmedLine.substr(TYPE_TOKEN.length());
                boost::algorithm::trim(currentType); // 去掉前后的空格
                DOO_CORE_ASSERT(currentType == "vertex" || currentType == "fragment" || currentType == "pixel",
                                "Invalid shader type specified");
            }
            else if (!currentType.empty())
            {
                currentSource += trimmedLine + "\n"; // 保留换行符
            }
        }
        if (!currentType.empty())
        {
            shaderSources[ShaderTypeFromString(currentType)] = currentSource;
        }

        return shaderSources;
    }

    GLuint CompileShader(GLenum type, const std::string &source)
    {
        GLuint shaderRendererID = glCreateShader(type);
        const GLchar *sourceCstr = source.c_str();
        glShaderSource(shaderRendererID, 1, &sourceCstr, 0);
        glCompileShader(shaderRendererID);

        GLint isCompiled = 0;
        glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            LogShaderError(shaderRendererID);
            glDeleteShader(shaderRendererID);
            return 0; // Indicate failure
        }

        return shaderRendererID;
    }

    void LogShaderError(GLuint shaderID)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);
        DOO_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);
    }

    void LinkProgram(GLuint program, const std::vector<GLuint> &shaderRendererIDs)
    {
        glLinkProgram(program);
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            LogProgramError(program);
            glDeleteProgram(program);
            for (auto id : shaderRendererIDs)
                glDeleteShader(id);
            return;
        }

        for (auto id : shaderRendererIDs)
            glDetachShader(program, id);
    }

    void LogProgramError(GLuint program)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        DOO_CORE_ERROR("Shader linking failed:\n{0}", &infoLog[0]);
    }

    static GLenum ShaderTypeFromString(const std::string &type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;
        return GL_NONE;
    }

    RendererID m_rendererID;
    std::string m_shaderSource;
};

std::unique_ptr<Shader> Shader::Create(const std::string &filepath)
{
    return std::make_unique<OpenGLShader>(filepath);
}

} // namespace Doodle
