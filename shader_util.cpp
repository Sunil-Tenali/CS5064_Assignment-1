#include <GL/glew.h>
#include "shader_util.hpp"

namespace glFramework
{
    GLuint LoadShaderGL(GLenum eShaderType, const std::string &strFilename)
    {
        std::ifstream shaderFile(strFilename.c_str());
        if (!shaderFile.is_open())
            throw std::runtime_error("Cannot find file: " + strFilename);
        
        std::stringstream shaderData;
        shaderData << shaderFile.rdbuf();
        shaderFile.close();
        
        return CreateShaderGL(eShaderType, shaderData.str());
    }

    GLuint CreateShaderGL(GLenum eShaderType, const std::string &strShaderFile)
    {
        GLuint shader = glCreateShader(eShaderType);
        const char *strFileData = strShaderFile.c_str();
        glShaderSource(shader, 1, &strFileData, NULL);
        
        glCompileShader(shader);
        
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
            
            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
            std::cerr << "Compile failure in shader:" << std::endl << strInfoLog << std::endl;
            delete[] strInfoLog;
        }
        return shader;
    }

    GLuint CreateProgramGL(const std::vector<GLuint> &shaderList)
    {
        GLuint program = glCreateProgram();
        
        for (size_t i = 0; i < shaderList.size(); i++)
            glAttachShader(program, shaderList[i]);
        
        glLinkProgram(program);
        
        GLint status;
        glGetProgramiv(program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
            
            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
            std::cerr << "GLSL Linker failure: " << strInfoLog << std::endl;
            delete[] strInfoLog;
        }

        for (size_t i = 0; i < shaderList.size(); i++)
            glDetachShader(program, shaderList[i]);

        return program;
    }
};








