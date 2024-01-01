// -------------------------------------------------------------
// Sourced from https://learnopengl.com/Getting-started/Shaders
// -------------------------------------------------------------
#pragma once

#ifndef _INCLUDE_GLAD_
#define _INCLUDE_GLAD_
#include <glad/glad.h>
#endif

#ifndef __INCLUDE_GLM_HPP__
#define __INCLUDE_GLM_HPP__
#include <glm/glm.hpp>
#endif

#ifndef __INCLUDE_STRING__
#define __INCLUDE_STRING__
#include <string>
#endif

#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    //Shader(const char* vertexPath, const char* fragmentPath)
    //{
    //    // 1. retrieve the vertex/fragment source code from filePath
    //    std::string vertexCode;
    //    std::string fragmentCode;
    //    std::ifstream vShaderFile;
    //    std::ifstream fShaderFile;
    //    // ensure ifstream objects can throw exceptions:
    //    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    //    try
    //    {
    //        // open files
    //        vShaderFile.open(vertexPath);
    //        fShaderFile.open(fragmentPath);
    //        std::stringstream vShaderStream, fShaderStream;
    //        // read file's buffer contents into streams
    //        vShaderStream << vShaderFile.rdbuf();
    //        fShaderStream << fShaderFile.rdbuf();
    //        // close file handlers
    //        vShaderFile.close();
    //        fShaderFile.close();
    //        // convert stream into string
    //        vertexCode = vShaderStream.str();
    //        fragmentCode = fShaderStream.str();
    //    }
    //    catch (std::ifstream::failure& e)
    //    {
    //        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    //    }
    //    const char* vShaderCode = vertexCode.c_str();
    //    const char* fShaderCode = fragmentCode.c_str();
    //    // 2. compile shaders
    //    unsigned int vertex, fragment;
    //    // vertex shader
    //    vertex = glCreateShader(GL_VERTEX_SHADER);
    //    glShaderSource(vertex, 1, &vShaderCode, NULL);
    //    glCompileShader(vertex);
    //    checkCompileErrors(vertex, "VERTEX");
    //    // fragment Shader
    //    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    //    glShaderSource(fragment, 1, &fShaderCode, NULL);
    //    glCompileShader(fragment);
    //    checkCompileErrors(fragment, "FRAGMENT");
    //    // shader Program
    //    ID = glCreateProgram();
    //    glAttachShader(ID, vertex);
    //    glAttachShader(ID, fragment);
    //    glLinkProgram(ID);
    //    checkCompileErrors(ID, "PROGRAM");
    //    // delete the shaders as they're linked into our program now and no longer necessary
    //    glDeleteShader(vertex);
    //    glDeleteShader(fragment);
    //}

    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::string tessControlCode;
        std::string tessEvalCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        std::ifstream tcShaderFile;
        std::ifstream teShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath != nullptr)
            {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
            if (tessControlPath != nullptr) {
                tcShaderFile.open(tessControlPath);
                std::stringstream tcShaderStream;
                tcShaderStream << tcShaderFile.rdbuf();
                tcShaderFile.close();
                tessControlCode = tcShaderStream.str();
            }
            if (tessEvalPath != nullptr) {
                teShaderFile.open(tessEvalPath);
                std::stringstream teShaderStream;
                teShaderStream << teShaderFile.rdbuf();
                teShaderFile.close();
                tessEvalCode = teShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "
                << e.what() << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != nullptr)
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // if tessellation shader is given, compile tessellation shader
        unsigned int tessControl;
        if (tessControlPath != nullptr)
        {
            const char* tcShaderCode = tessControlCode.c_str();
            tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
            glShaderSource(tessControl, 1, &tcShaderCode, NULL);
            glCompileShader(tessControl);
            checkCompileErrors(tessControl, "TESS_CONTROL");
        }
        unsigned int tessEval;
        if (tessEvalPath != nullptr)
        {
            const char* teShaderCode = tessEvalCode.c_str();
            tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
            glShaderSource(tessEval, 1, &teShaderCode, NULL);
            glCompileShader(tessEval);
            checkCompileErrors(tessEval, "TESS_EVALUATION");
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath != nullptr)
            glAttachShader(ID, geometry);
        if (tessControlPath != nullptr)
            glAttachShader(ID, tessControl);
        if (tessEvalPath != nullptr)
            glAttachShader(ID, tessEval);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, glm::vec3& vec) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

