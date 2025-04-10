// USES LearnOpenGL's Shader class
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../util/GLHelper.hpp"

class Shader
{
    std::string ReadFile(const char* shaderPath)
    {
        std::string code;
#ifndef UNIT_TESTING
        std::ifstream shaderFile;
        // ensure ifstream objects can throw exceptions:
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            shaderFile.open(shaderPath);
            std::stringstream shaderStream;
            // read file's buffer contents into streams
            shaderStream << shaderFile.rdbuf();
            // close file handlers
            shaderFile.close();
            // convert stream into string
            code = shaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
#endif
        return code;
    }
public:
    unsigned int ID;

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode = ReadFile(vertexPath);
        const char* vertexCodeStr = vertexCode.c_str();
        // 2. compile shaders
        // vertex shader
        unsigned int vertex = glCallR(glCreateShader(GL_VERTEX_SHADER));
        glCall(glShaderSource(vertex, 1, &vertexCodeStr, NULL));
        glCall(glCompileShader(vertex));
        checkCompileErrors(vertex, "VERTEX");

        ID = glCallR(glCreateProgram());
        glCall(glAttachShader(ID, vertex));
        glCall(glLinkProgram(ID));
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glCall(glDeleteShader(vertex));
    }

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode = ReadFile(vertexPath);
        std::string fragmentCode = ReadFile(fragmentPath);

        const char* vertexCodeStr = vertexCode.c_str();
        const char* fragmentCodeStr = fragmentCode.c_str();
        // 2. compile shaders
        // vertex shader
        unsigned int vertex = glCallR(glCreateShader(GL_VERTEX_SHADER));
        glCall(glShaderSource(vertex, 1, &vertexCodeStr, NULL));
        glCall(glCompileShader(vertex));
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        unsigned int fragment = glCallR(glCreateShader(GL_FRAGMENT_SHADER));
        glCall(glShaderSource(fragment, 1, &fragmentCodeStr, NULL));
        glCall(glCompileShader(fragment));
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCallR(glCreateProgram());
        glCall(glAttachShader(ID, vertex));
        glCall(glAttachShader(ID, fragment));
        glCall(glLinkProgram(ID));
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glCall(glDeleteShader(vertex));
        glCall(glDeleteShader(fragment));
    }

    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* geoPath, const char* fragmentPath)
    {
        std::string vertexCode = ReadFile(vertexPath);
        std::string geoCode = ReadFile(geoPath);
        std::string fragmentCode = ReadFile(fragmentPath);
        
        const char* vertexCodeStr = vertexCode.c_str();
        const char* geoCodeStr = geoCode.c_str();
        const char* fragmentCodeStr = fragmentCode.c_str();

        // 2. compile shaders
        // vertex shader
        unsigned int vertex = glCallR(glCreateShader(GL_VERTEX_SHADER));
        glCall(glShaderSource(vertex, 1, &vertexCodeStr, NULL));
        glCall(glCompileShader(vertex));
        checkCompileErrors(vertex, "VERTEX");
        // geometry Shader
        unsigned int geometry = glCallR(glCreateShader(GL_GEOMETRY_SHADER));
        glCall(glShaderSource(geometry, 1, &geoCodeStr, NULL));
        glCall(glCompileShader(geometry));
        checkCompileErrors(geometry, "GEOMETRY");
        // fragment Shader
        unsigned int fragment = glCallR(glCreateShader(GL_FRAGMENT_SHADER));
        glCall(glShaderSource(fragment, 1, &fragmentCodeStr, NULL));
        glCall(glCompileShader(fragment));
        checkCompileErrors(fragment, "FRAGMENT");
        // shader Program
        ID = glCallR(glCreateProgram());
        glCall(glAttachShader(ID, vertex));
        glCall(glAttachShader(ID, geometry));
        glCall(glAttachShader(ID, fragment));
        glCall(glLinkProgram(ID));
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glCall(glDeleteShader(vertex));
        glCall(glDeleteShader(geometry));
        glCall(glDeleteShader(fragment));
    }

    // delete shader
    ~Shader() {
        glCall(glDeleteProgram(ID));
    }

    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    {
        glCall(glUseProgram(ID));
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value));
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glCall(glUniform1i(glGetUniformLocation(ID, name.c_str()), value));
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glCall(glUniform1f(glGetUniformLocation(ID, name.c_str()), value));
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glCall(glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]));
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glCall(glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y));
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glCall(glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]));
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glCall(glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z));
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glCall(glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]));
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glCall(glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w));
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glCall(glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glCall(glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glCall(glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]));
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
#ifndef UNIT_TESTING
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
            if (!success)
            {
                glCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glCall(glGetProgramiv(shader, GL_LINK_STATUS, &success));
            if (!success)
            {
                glCall(glGetProgramInfoLog(shader, 1024, NULL, infoLog));
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
#endif
    }
};