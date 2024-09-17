#include "pch.h"
#include <boost/algorithm/string.hpp>
#include <cstdint>
#include <glad/glad.h>
#include <string>
#include <vector>

#include "Shader.h"
#include "ShaderReloader.h"

namespace Doodle
{

static ShaderPropertyType GetShaderPropertyType(uint32_t glType)
{
    switch (glType)
    {
    case GL_FLOAT:
        return ShaderPropertyType::Float;
    case GL_FLOAT_VEC2:
        return ShaderPropertyType::Float2;
    case GL_FLOAT_VEC3:
        return ShaderPropertyType::Float3;
    case GL_FLOAT_VEC4:
        return ShaderPropertyType::Float4;
    case GL_FLOAT_MAT3:
        return ShaderPropertyType::Mat3;
    case GL_FLOAT_MAT4:
        return ShaderPropertyType::Mat4;
    case GL_INT:
        return ShaderPropertyType::Int;
    case GL_INT_VEC2:
        return ShaderPropertyType::Int2;
    case GL_INT_VEC3:
        return ShaderPropertyType::Int3;
    case GL_INT_VEC4:
        return ShaderPropertyType::Int4;
    case GL_BOOL:
        return ShaderPropertyType::Bool;
    case GL_SAMPLER_2D:
        return ShaderPropertyType::Sampler2D;
    case GL_SAMPLER_CUBE:
        return ShaderPropertyType::SamplerCube;
    default:
        return ShaderPropertyType::None;
    }
}

class OpenGLShader : public Shader
{

public:
    OpenGLShader(const std::string &filepath) : m_reloader(filepath, *this), m_filepath(filepath)
    {
        ReadShaderFromFile(filepath);
        Renderer::Submit([this, filepath]() {
            CompileAndUploadShader();
            DOO_CORE_TRACE("OpenGLShader <{0}> created: {1}", m_rendererID, filepath);
            PrintActiveUniforms();
            PrintActiveUniformBlocks();
        });
    }

    ~OpenGLShader()
    {
        Renderer::Submit([this]() {
            glDeleteProgram(m_rendererID);
            DOO_CORE_TRACE("OpenGLShader <{0}> destroyed", m_rendererID);
        });
    }

    std::string GetPath() const override
    {
        return m_filepath;
    }

    void Reload() override
    {
        ReadShaderFromFile(m_filepath);
        Renderer::Submit([this]() {
            glDeleteProgram(m_rendererID);
            CompileAndUploadShader();
            DOO_CORE_TRACE("OpenGLShader <{0}> reloaded", m_rendererID);
            PrintActiveUniforms();
            PrintActiveUniformBlocks();
        });
    }

    void Bind() override
    {
        Renderer::Submit([this]() {
            glUseProgram(m_rendererID);
            DOO_CORE_TRACE("OpenGLShader <{0}> bound", m_rendererID);
        });
    }

    uint32_t GetRendererID() const override
    {
        return m_rendererID;
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
                DOO_CORE_ASSERT(currentType == "vertex" || currentType == "fragment" || currentType == "pixel" ||
                                    currentType == "geometry" || currentType == "compute",
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

        int isCompiled = 0;
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
        int maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);
        DOO_CORE_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);
    }

    void LinkProgram(GLuint program, const std::vector<GLuint> &shaderRendererIDs)
    {
        glLinkProgram(program);
        int isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            LogProgramError(program);
            glDeleteProgram(program);
            for (auto id : shaderRendererIDs)
                glDeleteShader(id);
            return;
        }

        int numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

        int uniformMaxLength = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

        int count = -1;
        GLenum type = 0;
        GLsizei length;
        int location;
        std::vector<GLchar> uniformName(uniformMaxLength);
        m_properties.clear();
        for (int i = 0; i < numUniforms; i++)
        {
            glGetActiveUniform(program, i, uniformMaxLength, &length, &count, &type, uniformName.data());
            std::string name(uniformName.begin(), uniformName.begin() + length);
            location = glGetUniformLocation(program, name.c_str());
            if (location == -1)
                continue;
            ShaderProperty property;
            property.Name = name;
            property.Type = GetShaderPropertyType(type);
            m_properties.push_back(property);
            m_uniformsCache[name] = location;
            auto arrayPos = name.find('[');
            if (arrayPos != std::string::npos)
            {
                name = name.substr(0, arrayPos);
                m_uniformsCache[name] = location;
            }
        }

        int numUniformBlocks = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);

        int uniformBlockMaxLength = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockMaxLength);

        count = -1;
        type = 0;
        std::vector<GLchar> uniformBlockName(uniformBlockMaxLength);
        for (int i = 0; i < numUniformBlocks; i++)
        {
            glGetActiveUniformBlockName(program, i, uniformBlockMaxLength, &length, uniformBlockName.data());
            std::string name(uniformBlockName.begin(), uniformBlockName.begin() + length);
            location = glGetUniformBlockIndex(program, name.c_str());
            if (location == -1)
                continue;
            m_uniformBlocksCache[name] = location;
        }

        for (auto id : shaderRendererIDs)
            glDetachShader(program, id);
    }

    void LogProgramError(GLuint program)
    {
        int maxLength = 0;
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
        if (type == "geometry")
            return GL_GEOMETRY_SHADER;
        if (type == "compute")
            return GL_COMPUTE_SHADER;
        return GL_NONE;
    }

    uint32_t GetUniformLocation(const std::string &name) override
    {
        auto it = m_uniformsCache.find(name);
        if (it == m_uniformsCache.end())
        {
            return -1;
        }
        else
        {
            return it->second;
        }
    }

    uint32_t GetUniformBlockIndex(const std::string &name) override
    {
        auto it = m_uniformBlocksCache.find(name);
        if (it == m_uniformBlocksCache.end())
        {
            return -1;
        }
        else
        {
            return it->second;
        }
    }

    uint32_t GetUniformBlockBinding(const std::string &name) override
    {
        uint32_t index = GetUniformBlockIndex(name);
        if (index == -1)
            return -1;

        int blockBinding;
        glGetActiveUniformBlockiv(m_rendererID, index, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
        return blockBinding;
    }

    void BindUniformBlock(const std::string &name, uint32_t binding) override
    {
        uint32_t index = GetUniformBlockIndex(name);
        if (index == -1)
            return;

        glUniformBlockBinding(m_rendererID, index, binding);
    }

    void PrintActiveUniforms() override
    {
        int numUniforms = 0;
        glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORMS, &numUniforms);

        int uniformMaxLength = 0;
        glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

        std::ostringstream oss;
        int count = -1;
        GLenum type = 0;
        GLsizei length;
        int location;
        std::vector<GLchar> uniformName(uniformMaxLength);
        oss << "Active uniforms for shader <" << m_rendererID << ">:" << std::endl;
        for (int i = 0; i < numUniforms; i++)
        {
            glGetActiveUniform(m_rendererID, i, uniformMaxLength, &length, &count, &type, uniformName.data());
            std::string name(uniformName.begin(), uniformName.begin() + length);
            location = glGetUniformLocation(m_rendererID, name.c_str());
            if (location == -1)
                continue;
            oss << "    <" << location << "> " << name << std::endl;
        }
        DOO_CORE_INFO(oss.str());
    }

    std::vector<ShaderProperty> GetProperties() override
    {
        return m_properties;
    }

    void PrintActiveUniformBlocks() override
    {
        int numUniformBlocks = 0;
        glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);

        int uniformBlockMaxLength = 0;
        glGetProgramiv(m_rendererID, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &uniformBlockMaxLength);

        std::ostringstream oss;
        GLsizei length;
        std::vector<GLchar> uniformBlockName(uniformBlockMaxLength);
        oss << "Active uniform blocks for shader <" << m_rendererID << ">:" << std::endl;
        for (int i = 0; i < numUniformBlocks; i++)
        {
            int blockBinding;
            int blockDataSize;
            glGetActiveUniformBlockName(m_rendererID, i, uniformBlockMaxLength, &length, uniformBlockName.data());
            glGetActiveUniformBlockiv(m_rendererID, i, GL_UNIFORM_BLOCK_BINDING, &blockBinding);
            glGetActiveUniformBlockiv(m_rendererID, i, GL_UNIFORM_BLOCK_DATA_SIZE, &blockDataSize);
            std::string name(uniformBlockName.begin(), uniformBlockName.begin() + length);
            int location = glGetUniformBlockIndex(m_rendererID, name.c_str());
            if (location == -1)
                continue;
            oss << "    <" << location << "> " << name << ", binding=" << blockBinding << ", size=" << blockDataSize
                << std::endl;
        }
        DOO_CORE_INFO(oss.str());
    }

    void SetUniform1i(const std::string &name, int v) override
    {
        SetUniform(name, glUniform1i, v);
    }

    void SetUniform2i(const std::string &name, int v1, int v2) override
    {
        SetUniform(name, glUniform2i, v1, v2);
    }

    void SetUniform3i(const std::string &name, int v1, int v2, int v3) override
    {
        SetUniform(name, glUniform3i, v1, v2, v3);
    }

    void SetUniform4i(const std::string &name, int v1, int v2, int v3, int v4) override
    {
        SetUniform(name, glUniform4i, v1, v2, v3, v4);
    }

    void SetUniform1f(const std::string &name, float v) override
    {
        SetUniform(name, glUniform1f, v);
    }

    void SetUniform2f(const std::string &name, float v1, float v2) override
    {
        SetUniform(name, glUniform2f, v1, v2);
    }

    void SetUniform3f(const std::string &name, float v1, float v2, float v3) override
    {
        SetUniform(name, glUniform3f, v1, v2, v3);
    }

    void SetUniform4f(const std::string &name, float v1, float v2, float v3, float v4) override
    {
        SetUniform(name, glUniform4f, v1, v2, v3, v4);
    }

    void SetUniform1iv(const std::string &name, int count, int *v) override
    {
        SetUniform(name, [=](int location) { glUniform1iv(location, count, v); });
    }

    void SetUniform2iv(const std::string &name, int count, int *v) override
    {
        SetUniform(name, [=](int location) { glUniform2iv(location, count, v); });
    }

    void SetUniform3iv(const std::string &name, int count, int *v) override
    {
        SetUniform(name, [=](int location) { glUniform3iv(location, count, v); });
    }

    void SetUniform4iv(const std::string &name, int count, int *v) override
    {
        SetUniform(name, [=](int location) { glUniform4iv(location, count, v); });
    }

    void SetUniform1fv(const std::string &name, int count, float *v) override
    {
        SetUniform(name, [=](int location) { glUniform1fv(location, count, v); });
    }

    void SetUniform2fv(const std::string &name, int count, float *v) override
    {
        SetUniform(name, [=](int location) { glUniform2fv(location, count, v); });
    }

    void SetUniform3fv(const std::string &name, int count, float *v) override
    {
        SetUniform(name, [=](int location) { glUniform3fv(location, count, v); });
    }

    void SetUniform4fv(const std::string &name, int count, float *v) override
    {
        SetUniform(name, [=](int location) { glUniform4fv(location, count, v); });
    }

    void SetUniformMatrix2f(const std::string &name, const glm::mat2 &mat) override
    {
        SetUniform(name, glUniformMatrix2fv, 1, GL_FALSE, &mat[0][0]);
    }

    void SetUniformMatrix3f(const std::string &name, const glm::mat3 &mat) override
    {
        SetUniform(name, glUniformMatrix3fv, 1, GL_FALSE, &mat[0][0]);
    }

    void SetUniformMatrix4f(const std::string &name, const glm::mat4 &mat) override
    {
        SetUniform(name, glUniformMatrix4fv, 1, GL_FALSE, &mat[0][0]);
    }

    void SetUniformTexture(const std::string &name, std::shared_ptr<Texture> texture) override
    {
        SetUniform(name, glUniformHandleui64ARB, texture->GetTextureHandle());
    }

    std::string m_filepath;
    std::vector<ShaderProperty> m_properties;
    std::unordered_map<std::string, uint32_t> m_uniformsCache;
    std::unordered_map<std::string, uint32_t> m_uniformBlocksCache;
    uint32_t m_rendererID;
    std::string m_shaderSource;
    ShaderReloader m_reloader;
};

std::shared_ptr<Shader> Shader::Create(const std::string &filepath)
{
    return std::make_shared<OpenGLShader>(filepath);
}

} // namespace Doodle
