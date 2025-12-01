#pragma once
#include <glad/glad.h>
#include <string>

/**
 * @brief OpenGL 着色器程序封装类
 * 
 * 该类封装了 OpenGL 着色器的编译、链接和使用功能。
 * 支持从源码字符串或文件加载着色器，并提供了常用的 Uniform 变量设置方法。
 * 
 * 特性：
 * - RAII 资源管理：析构时自动释放 OpenGL 资源
 * - 禁用拷贝：防止 OpenGL 资源被意外复制
 * - 支持移动语义：可以安全地转移着色器所有权
 * 
 * 使用示例：
 * @code
 * Shader shader;
 * if (shader.compile(vertexSource, fragmentSource)) {
 *     shader.use();
 *     shader.setFloat("time", currentTime);
 * }
 * @endcode
 */
class Shader {
public:
    /**
     * @brief 默认构造函数
     * 
     * 创建一个空的着色器对象，需要调用 compile() 或 loadFromFile() 来初始化
     */
    Shader();

    /**
     * @brief 析构函数
     * 
     * 自动释放 OpenGL 着色器程序资源
     */
    ~Shader();

    // 禁用拷贝构造和拷贝赋值，防止 OpenGL 资源被意外复制
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /**
     * @brief 移动构造函数
     * @param other 要移动的着色器对象，移动后 other 将变为无效状态
     */
    Shader(Shader&& other) noexcept;

    /**
     * @brief 移动赋值运算符
     * @param other 要移动的着色器对象，移动后 other 将变为无效状态
     * @return 当前对象的引用
     */
    Shader& operator=(Shader&& other) noexcept;

    /**
     * @brief 从源码字符串编译着色器程序
     * 
     * 编译顶点着色器和片段着色器，并将它们链接成一个着色器程序。
     * 如果当前已有着色器程序，会先释放旧的资源。
     * 
     * @param vertexSource   顶点着色器的 GLSL 源代码字符串
     * @param fragmentSource 片段着色器的 GLSL 源代码字符串
     * @return true  编译和链接成功
     * @return false 编译或链接失败，错误信息会通过日志输出
     */
    bool compile(const std::string& vertexSource, const std::string& fragmentSource);

    /**
     * @brief 从文件加载并编译着色器程序
     * 
     * 读取指定路径的着色器源文件，然后编译和链接。
     * 
     * @param vertexPath   顶点着色器文件的路径（相对或绝对路径）
     * @param fragmentPath 片段着色器文件的路径（相对或绝对路径）
     * @return true  文件读取、编译和链接都成功
     * @return false 文件读取失败或编译/链接失败
     */
    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    /**
     * @brief 激活（使用）当前着色器程序
     * 
     * 调用 glUseProgram() 将当前着色器设置为活动状态。
     * 后续的绑定和绘制调用将使用此着色器进行渲染。
     */
    void use() const;

    /**
     * @brief 获取 OpenGL 着色器程序 ID
     * @return OpenGL 程序对象的 ID，如果无效则返回 0
     */
    unsigned int getId() const { return m_programId; }

    /**
     * @brief 检查着色器程序是否有效
     * @return true  着色器程序已成功编译和链接
     * @return false 着色器程序无效（未初始化或编译失败）
     */
    bool isValid() const { return m_programId != 0; }

    /**
     * @brief 获取 Uniform 变量的位置
     * 
     * @param name Uniform 变量在着色器中的名称
     * @return Uniform 变量的位置索引，如果不存在返回 -1
     */
    int getUniformLocation(const std::string& name) const;

    // ==================== Uniform 设置函数 ====================

    /**
     * @brief 设置布尔类型的 Uniform 变量
     * @param name  Uniform 变量名称
     * @param value 要设置的布尔值（在 GLSL 中作为 int 传递，0 或 1）
     */
    void setBool(const std::string& name, bool value) const;

    /**
     * @brief 设置整数类型的 Uniform 变量
     * @param name  Uniform 变量名称
     * @param value 要设置的整数值
     */
    void setInt(const std::string& name, int value) const;

    /**
     * @brief 设置浮点类型的 Uniform 变量
     * @param name  Uniform 变量名称
     * @param value 要设置的浮点值
     */
    void setFloat(const std::string& name, float value) const;

    /**
     * @brief 设置二维向量类型的 Uniform 变量 (vec2)
     * @param name Uniform 变量名称
     * @param x    向量的 X 分量
     * @param y    向量的 Y 分量
     */
    void setVec2(const std::string& name, float x, float y) const;

    /**
     * @brief 设置三维向量类型的 Uniform 变量 (vec3)
     * @param name Uniform 变量名称
     * @param x    向量的 X 分量
     * @param y    向量的 Y 分量
     * @param z    向量的 Z 分量
     */
    void setVec3(const std::string& name, float x, float y, float z) const;

    /**
     * @brief 设置四维向量类型的 Uniform 变量 (vec4)
     * @param name Uniform 变量名称
     * @param x    向量的 X 分量
     * @param y    向量的 Y 分量
     * @param z    向量的 Z 分量
     * @param w    向量的 W 分量
     */
    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    /**
     * @brief 设置 4x4 矩阵类型的 Uniform 变量 (mat4)
     * @param name  Uniform 变量名称
     * @param value 指向 16 个 float 元素的数组指针（列优先顺序）
     */
    void setMat4(const std::string& name, const float* value) const;

private:
    unsigned int m_programId{0};  ///< OpenGL 着色器程序 ID

    /**
     * @brief 编译单个着色器
     * @param type   着色器类型：GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER
     * @param source 着色器的 GLSL 源代码
     * @return 编译成功返回着色器 ID，失败返回 0
     */
    unsigned int compileShader(GLenum type, const std::string& source);

    /**
     * @brief 检查着色器编译错误
     * @param shader 要检查的着色器 ID
     * @param type   着色器类型名称（用于错误日志）
     * @return true 编译成功，false 编译失败
     */
    bool checkCompileErrors(unsigned int shader, const std::string& type);

    /**
     * @brief 检查着色器程序链接错误
     * @param program 要检查的程序 ID
     * @return true 链接成功，false 链接失败
     */
    bool checkLinkErrors(unsigned int program);

    /**
     * @brief 清理 OpenGL 资源
     * 
     * 删除当前的着色器程序并重置 ID
     */
    void cleanup();
};
