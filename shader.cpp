#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "common.hpp"
using namespace std;

static string readFile(const char *path)
{
    string code;
    ifstream stream(path, std::ios::in);
    if (stream.is_open()) {
        stringstream sstr;
        sstr << stream.rdbuf();
        code = sstr.str();
        stream.close();
    } else {
        fprintf(stderr, "file not found: %s\n", path);
        exit(-1);
    }
    return code;
}

static void compileShader(const string &code, GLuint shaderID)
{
    GLint result = GL_FALSE;
    int infoLogLength;

    const char *codePointer = code.c_str();
    glShaderSource(shaderID, 1, &codePointer, NULL);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

    // the debug code is from http://docs.gl/gl4/glCompileShader
    if (result != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar message[1024] = {};
        glGetShaderInfoLog(shaderID, 1024, &log_length, message);
        // Write the error to a log
        fprintf(stderr, "%s\n", message);
        exit(-1);
    }
}

GLuint LoadShaders(const char *vertexFilePath, const char *fragmentFilePath)
{
    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile the shaders
    printf("loading shader %s\n", vertexFilePath);
    string vertexShaderCode = readFile(vertexFilePath);
    compileShader(vertexShaderCode.c_str(), vertexShaderID);
    printf("loading shader %s\n", fragmentFilePath);
    string fragmentShaderCode = readFile(fragmentFilePath);
    compileShader(fragmentShaderCode, fragmentShaderID);

    // Link the program
    GLuint programID = glCreateProgram();
    printf("linking shaders\n");
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    // Check the program
    // the debug code is from http://docs.gl/gl4/glCreateProgram
    GLint program_linked = GL_FALSE;
    glGetProgramiv(programID, GL_LINK_STATUS, &program_linked);
    
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024] = {};
        glGetProgramInfoLog(programID, 1024, &log_length, message);
        // Write the error to a log
        fprintf(stderr, "%s\n", message);
        exit(-1);
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}
