#include "Core.h"
#include "pch.h"
#include <cstdint>
#include <glad/glad.h>
#include <memory>

#include "Shader.h"
#include <boost/algorithm/string.hpp>

namespace Doodle
{

#define IMPLEMENT_UNIFORM(suffix, input, glInput)                                                                      \
    void SetUniform##suffix##input override                                                                            \
    {                                                                                                                  \
        GLint location = GetUniformLocation(name);                                                                     \
        if (location != -1)                                                                                            \
            glUniform##suffix(glInput);                                                                                \
    }

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

        GLint numUniforms = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

        GLint uniformMaxLength = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);

        GLint count = -1;
		GLenum type = 0;
		GLsizei length;
		GLint location;
		std::vector<GLchar> uniformName(uniformMaxLength);
		for(GLint i = 0; i < numUniforms; i++) {
			glGetActiveUniform(program, i, uniformMaxLength, &length, &count, &type, uniformName.data());
			std::string name(uniformName.begin(), uniformName.begin()+length);
			location = glGetUniformLocation(program, name.c_str());
			if (location == -1) continue; 
			m_uniformsCache[name] = location;
			auto arrayPos = name.find('[');
			if(arrayPos!=std::string::npos){
				name = name.substr(0, arrayPos);
				m_uniformsCache[name] = location;
			}
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

    GLint GetUniformLocation(const std::string &name)
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

    void SetUniform1i(const std::string &name, int v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform1i(location, v);
    }

    void SetUniform2i(const std::string &name, int v1, int v2) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform2i(location, v1, v2);
    }

    void SetUniform3i(const std::string &name, int v1, int v2, int v3) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform3i(location, v1, v2, v3);
    }

    void SetUniform4i(const std::string &name, int v1, int v2, int v3, int v4) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform4i(location, v1, v2, v3, v4);
    }

    void SetUniform1f(const std::string &name, float v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform1f(location, v);
    }

    void SetUniform2f(const std::string &name, float v1, float v2) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform2f(location, v1, v2);
    }

    void SetUniform3f(const std::string &name, float v1, float v2, float v3) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform3f(location, v1, v2, v3);
    }

    void SetUniform4f(const std::string &name, float v1, float v2, float v3, float v4) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform4f(location, v1, v2, v3, v4);
    }

    void SetUniform1iv(const std::string &name, int count, int *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform1iv(location, count, v);
    }

    void SetUniform2iv(const std::string &name, int count, int *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform2iv(location, count, v);
    }

    void SetUniform3iv(const std::string &name, int count, int *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform3iv(location, count, v);
    }

    void SetUniform4iv(const std::string &name, int count, int *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform4iv(location, count, v);
    }

    void SetUniform1fv(const std::string &name, int count, float *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform1fv(location, count, v);
    }

    void SetUniform2fv(const std::string &name, int count, float *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform2fv(location, count, v);
    }

    void SetUniform3fv(const std::string &name, int count, float *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform3fv(location, count, v);
    }

    void SetUniform4fv(const std::string &name, int count, float *v) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniform4fv(location, count, v);
    }

    void SetUniformMatrix2f(const std::string &name, const glm::mat2 &mat) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    void SetUniformMatrix3f(const std::string &name, const glm::mat3 &mat) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    void SetUniformMatrix4f(const std::string &name, const glm::mat4 &mat) override
    {
        GLint location = GetUniformLocation(name);
        if (location != -1)
            glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    }

    std::unordered_map<std::string, GLint> m_uniformsCache;
    uint32_t m_rendererID;
    std::string m_shaderSource;
};

std::unique_ptr<Shader> Shader::Create(const std::string &filepath)
{
    return std::make_unique<OpenGLShader>(filepath);
}

} // namespace Doodle
