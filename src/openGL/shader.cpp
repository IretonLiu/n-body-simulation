#include "shader.h"

static std::string ShaderTypeToString(unsigned int type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "vertex";
            break;

        case GL_FRAGMENT_SHADER:
            return "fragment";
            break;

        case GL_COMPUTE_SHADER:
            return "compute";
            break;

        default:
            break;
    }
    ASSERT(false);
    return "";
}

static unsigned int ShaderStringToType(const std::string &shaderString) {
    if (shaderString == "vertex") {
        return GL_VERTEX_SHADER;
    }
    if (shaderString == "fragment") {
        return GL_FRAGMENT_SHADER;
    }
    if (shaderString == "compute") {
        return GL_COMPUTE_SHADER;
    }
    ASSERT(false);
    return 0;
}

Shader::Shader() : rendererID(0) {}

Shader::~Shader() {
    GLCall(glDeleteProgram(rendererID));
}

void Shader::Bind() const {
    GLCall(glUseProgram(rendererID));
}

void Shader::Unbind() const {
    GLCall(glUseProgram(0));
}

std::string Shader::ReadShader(const std::string &filepath) {
    std::string shaderCode;
    std::ifstream shaderStream(filepath, std::ios::in);
    if (shaderStream.is_open()) {  // Check if the file can be opened
        std::stringstream sstr;
        sstr << shaderStream.rdbuf();
        shaderCode = sstr.str();
        shaderStream.close();
    } else {
        printf("Could not open %s.?\n", filepath);
        getchar();
        return 0;
    }

    return shaderCode;
}

// consider a parseing function for putting a vertex and fragment shader in the same file

unsigned int Shader::CreateVFShader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    GLCall(unsigned int programID = glCreateProgram());

    std::string vertexShader = ReadShader(vertexShaderPath);
    std::string fragmentShader = ReadShader(fragmentShaderPath);
    unsigned int vsID = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fsID = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(programID, vsID));
    GLCall(glAttachShader(programID, fsID));
    GLCall(glLinkProgram(programID));
    GLCall(glValidateProgram(programID));

    GLCall(glDeleteShader(vsID));
    GLCall(glDeleteShader(fsID));
    rendererID = programID;
    return programID;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source) {
    GLCall(unsigned int shaderID = glCreateShader(type));
    const char *src = source.c_str();  // pointer to the source string data
    GLCall(glShaderSource(shaderID, 1, &src, nullptr));
    GLCall(glCompileShader(shaderID));

    int result = GL_FALSE;
    int infoLogLength;

    GLCall(glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result));
    GLCall(glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength));
    if (infoLogLength > 0) {
        std::vector<char> shaderErrorMessage(infoLogLength + 1);
        GLCall(glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]));
        std::cout << "Failed to compile " << ShaderTypeToString(type) << " shader, reason:" << std::endl;
        printf("%s\n", &shaderErrorMessage[0]);
        glDeleteShader(shaderID);
        return 0;
    }

    return shaderID;
}

unsigned int Shader::GetUniformLocation(const std::string &name) {
    GLCall(unsigned int location = glGetUniformLocation(rendererID, name.c_str()));
    if (location == -1)
        std::cout << "Uniform error: no uniform variable with name '" << name << "'" << std::endl;
    return location;
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    unsigned int location = GetUniformLocation(name);
    GLCall(glUniform4f(location, v0, v1, v2, v3));
}

void Shader::SetUniformMatrix4fv(const std::string &name, const glm::mat4 &matrix) {
    unsigned int location = GetUniformLocation(name);
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]));
}
