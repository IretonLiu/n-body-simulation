#pragma once
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "renderer.h"

struct ShaderSource {
    std::string vertexSource;
    std::string fragmentSource;
};

class Shader {
   private:
    // std::string filepath;
    unsigned int rendererID;

    // TODO: consider finishing this function
    // ShaderSource ParseShader(const std::string &filepath);

    // Create vertex and fragment shader
    // unsigned int CreateVFShader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    /*
     * compiles shader from source code
     * @param type type of shader
     * @param source the source code string of the shader
     * @returns the ID of the shader as an unsigned int
     */
    unsigned int CompileShader(unsigned int type, const std::string &source);

    /*
     * read in the shader source from the shader file path
     * @param filepath the path of the source code
     * @returns the source code of the shader as a string
     */
    std::string ReadShader(const std::string &filepath);

    /*
     * get the location of the uniform variable
     * @param name the name of the uniform variable
     * @returns the location of the uniform variable
     */
    unsigned int GetUniformLocation(const std::string &name);

   public:
    Shader();
    ~Shader();

    inline unsigned int GetRendererID() { return rendererID; };

    /*
     * creates the vertex and fragment shader
     * @param vertexShaderPath the path of the file containing the vertex shader
     * @param fragmentShaderPath the path of the file containing the fragment shader
     * @returns the ID of the shader program
     */
    unsigned int CreateVFShader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    // TODO: add funtionality of other shaders eg compute shaders

    void Bind() const;
    void Unbind() const;

    // Set uniform variables
    void SetUniform4f(const std::string &name, float v0, float v1, float f2, float f3);
    void SetUniformMatrix4fv(const std::string &name, const glm::mat4 &matrix);
};