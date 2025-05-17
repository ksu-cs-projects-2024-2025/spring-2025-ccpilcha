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
#include "game/GameContext.hpp"

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

    bool hasVertex = false, hasFragment = false, hasGeometry = false;
    std::string vertexPath, fragmentPath, geometryPath;
public:
    unsigned int ID;

    Shader(std::string vertexPath) :
        vertexPath(vertexPath)
    {
        hasVertex = true;
    }

    Shader(std::string vertexPath, std::string fragmentPath):
        vertexPath(vertexPath), fragmentPath(fragmentPath)
    {
        hasVertex = true;
        hasFragment = true;
    }

    Shader(std::string vertexPath, std::string geoPath, std::string fragmentPath):
        vertexPath(vertexPath), geometryPath(geoPath), fragmentPath(fragmentPath)
    {
        hasVertex = true;
        hasGeometry = true;
        hasFragment = true;
    }

    void Init(GameContext *c)
    {
        // shader Program
        this->ID = glCallR(glCreateProgram());
        if (hasVertex)
        {
            std::string vertexCode = ReadFile(this->vertexPath.c_str());

            const char* vertexCodeStr = vertexCode.c_str();

            unsigned int vertex = glCallR(glCreateShader(GL_VERTEX_SHADER));
            glCall(glShaderSource(vertex, 1, &vertexCodeStr, NULL));
            glCall(glCompileShader(vertex));
            checkCompileErrors(vertex, "VERTEX", vertexPath);
            glCall(glAttachShader(this->ID, vertex));
            glCall(glDeleteShader(vertex));
        }

        if (hasGeometry)
        {
            std::string geoCode = ReadFile(this->geometryPath.c_str());

            const char* geoCodeStr = geoCode.c_str();

            unsigned int geometry = glCallR(glCreateShader(GL_GEOMETRY_SHADER));
            glCall(glShaderSource(geometry, 1, &geoCodeStr, NULL));
            glCall(glCompileShader(geometry));
            checkCompileErrors(geometry, "GEOMETRY", geometryPath);
            glCall(glAttachShader(this->ID, geometry));
            glCall(glDeleteShader(geometry));
        }

        if (hasFragment)
        {
            std::string fragmentCode = ReadFile(this->fragmentPath.c_str());

            const char* fragmentCodeStr = fragmentCode.c_str();

            // fragment Shader
            unsigned int fragment = glCallR(glCreateShader(GL_FRAGMENT_SHADER));
            glCall(glShaderSource(fragment, 1, &fragmentCodeStr, NULL));
            glCall(glCompileShader(fragment));
            checkCompileErrors(fragment, "FRAGMENT", fragmentPath);
            glCall(glAttachShader(this->ID, fragment));
            glCall(glDeleteShader(fragment));
        }
        glCall(glLinkProgram(this->ID));
        checkLinkErrors(this->ID);
        // delete the shaders as they're linked into our program now and no longer necessary

        c->glCleanupQueue.emplace([=]() {
            glCall(glDeleteProgram(this->ID));
        });
    }

    // delete shader
    ~Shader() {
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

    void createShader(unsigned int shader)
    {
        GL_PROGRAM;
        glCall(glAttachShader(ID, shader));
    }

    void checkLinkErrors(GLuint program)
    {
#ifndef UNIT_TESTING
        GLint success;
        GLchar infoLog[1024];
        glCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
        if (!success)
        {
            glCall(glGetProgramInfoLog(program, 1024, NULL, infoLog));
            std::cout << "ERROR::PROGRAM_LINKING_ERROR \n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
#endif
    }

    void checkCompileErrors(GLuint shader, std::string type, std::string file)
    {
#ifndef UNIT_TESTING
        GLint success;
        GLchar infoLog[1024];
        glCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
        if (!success)
        {
            glCall(glGetShaderInfoLog(shader, 1024, NULL, infoLog));
            std::cout << "FILE: " << file << std::endl;
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
#endif
    }
};