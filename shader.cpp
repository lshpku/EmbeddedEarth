#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <GLFW/glfw3.h>
#include <OpenGL/gl3.h>
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

static void throwInfoLog(GLuint program, int infoLogLength)
{
    if (!infoLogLength) {
        return;
    }
    char *infoLogBuffer = new char[infoLogLength];
    glGetShaderInfoLog(program, infoLogLength, NULL, infoLogBuffer);
    fprintf(stderr, "%s\n", infoLogBuffer);
    exit(-1);
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
    throwInfoLog(shaderID, infoLogLength);
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
    GLint result = GL_FALSE;
    int infoLogLength;
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    throwInfoLog(programID, infoLogLength);

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}
