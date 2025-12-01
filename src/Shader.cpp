#include "Shader.h"
#include <logger.h>
#include <fstream>
#include <sstream>

// ==================== 构造函数和析构函数 ====================

Shader::Shader() = default;

Shader::~Shader()
{
    // 析构时自动释放 OpenGL 资源
    cleanup();
}

// ==================== 移动语义 ====================

Shader::Shader(Shader&& other) noexcept
    : m_programId(other.m_programId)
{
    // 将源对象的程序 ID 置为 0，防止其析构时删除资源
    other.m_programId = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other) {
        // 先清理当前持有的资源
        cleanup();
        // 接管源对象的资源
        m_programId = other.m_programId;
        other.m_programId = 0;
    }
    return *this;
}

// ==================== 编译和加载 ====================

bool Shader::compile(const std::string& vertexSource, const std::string& fragmentSource)
{
    // 如果已有着色器程序，先释放旧资源
    cleanup();

    // 第一步：编译顶点着色器
    // 顶点着色器负责处理每个顶点的位置变换
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) {
        return false;
    }

    // 第二步：编译片段着色器
    // 片段着色器负责计算每个像素的最终颜色
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        // 片段着色器编译失败，需要清理已编译的顶点着色器
        glDeleteShader(vertexShader);
        return false;
    }

    // 第三步：创建着色器程序并链接
    // 着色器程序是多个着色器链接在一起的最终可执行对象
    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertexShader);    // 附加顶点着色器
    glAttachShader(m_programId, fragmentShader);  // 附加片段着色器
    glLinkProgram(m_programId);                   // 链接程序

    // 检查链接是否成功
    bool success = checkLinkErrors(m_programId);

    // 第四步：删除着色器对象
    // 着色器已经链接到程序中，不再需要保留着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success) {
        // 链接失败，清理程序对象
        glDeleteProgram(m_programId);
        m_programId = 0;
        return false;
    }

    return true;
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexSource;
    std::string fragmentSource;

    // 读取顶点着色器文件
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        NEAPU_LOGE("Failed to open vertex shader file: {}", vertexPath);
        return false;
    }
    // 使用 stringstream 读取整个文件内容
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexSource = vertexStream.str();
    vertexFile.close();

    // 读取片段着色器文件
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        NEAPU_LOGE("Failed to open fragment shader file: {}", fragmentPath);
        return false;
    }
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentSource = fragmentStream.str();
    fragmentFile.close();

    // 调用 compile() 进行编译
    return compile(vertexSource, fragmentSource);
}

// ==================== 使用着色器 ====================

void Shader::use() const
{
    if (m_programId != 0) {
        // 将当前着色器程序设置为 OpenGL 渲染管线的活动程序
        glUseProgram(m_programId);
    }
}

int Shader::getUniformLocation(const std::string& name) const
{
    // 查询 Uniform 变量在着色器程序中的位置
    // 返回 -1 表示该变量不存在或未被使用
    return glGetUniformLocation(m_programId, name.c_str());
}

// ==================== 私有辅助函数 ====================

unsigned int Shader::compileShader(GLenum type, const std::string& source)
{
    // 创建着色器对象
    unsigned int shader = glCreateShader(type);
    
    // 设置着色器源代码
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    
    // 编译着色器
    glCompileShader(shader);

    // 检查编译错误
    std::string typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
    if (!checkCompileErrors(shader, typeName)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::checkCompileErrors(unsigned int shader, const std::string& type)
{
    int success;
    char infoLog[1024];

    // 查询编译状态
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        // 获取并输出错误日志
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        NEAPU_LOGE("{} shader compilation failed:\n{}", type, infoLog);
        return false;
    }
    return true;
}

bool Shader::checkLinkErrors(unsigned int program)
{
    int success;
    char infoLog[1024];

    // 查询链接状态
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        // 获取并输出错误日志
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        NEAPU_LOGE("Shader program linking failed:\n{}", infoLog);
        return false;
    }
    return true;
}

void Shader::cleanup()
{
    if (m_programId != 0) {
        // 删除 OpenGL 着色器程序对象
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

// ==================== Uniform 设置函数实现 ====================
// 注意：调用这些函数前需要先调用 use() 激活着色器

void Shader::setBool(const std::string& name, bool value) const
{
    // GLSL 没有原生的 bool uniform，使用 int 代替（0 = false, 1 = true）
    glUniform1i(glGetUniformLocation(m_programId, name.c_str()), static_cast<int>(value));
}

void Shader::setInt(const std::string& name, int value) const
{
    // 设置 int 类型 uniform，常用于纹理采样器（sampler2D）
    glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    // 设置 float 类型 uniform
    glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    // 设置 vec2 类型 uniform（二维向量）
    glUniform2f(glGetUniformLocation(m_programId, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    // 设置 vec3 类型 uniform（三维向量），常用于颜色、位置、法线等
    glUniform3f(glGetUniformLocation(m_programId, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    // 设置 vec4 类型 uniform（四维向量），常用于颜色（含透明度）、齐次坐标
    glUniform4f(glGetUniformLocation(m_programId, name.c_str()), x, y, z, w);
}

void Shader::setMat4(const std::string& name, const float* value) const
{
    // 设置 mat4 类型 uniform（4x4 矩阵），常用于模型、视图、投影矩阵
    // 参数说明：
    // - 第2个参数 1：矩阵数量
    // - 第3个参数 GL_FALSE：是否转置矩阵（OpenGL 使用列优先，通常不需要转置）
    // - 第4个参数：指向 16 个 float 的数组指针
    glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, value);
}
