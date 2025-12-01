#include "MyWindow.h"
#include <glad/glad.h>
#include <logger.h>
#include <string>

// stb_image 图片加载库
// 定义 STB_IMAGE_IMPLEMENTATION 使 stb_image 包含实现代码
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ==================== 匿名命名空间 ====================
// 存放仅在本文件中使用的常量和数据

namespace {

/// 窗口默认宽度（像素）
constexpr int SCREEN_WIDTH = 800;

/// 窗口默认高度（像素）
constexpr int SCREEN_HEIGHT = 600;

/**
 * @brief 获取资源文件路径
 * 
 * Debug 模式：使用 SOURCE_DIR 宏获取项目根目录
 * Release 模式：使用可执行文件的相对路径
 * 
 * @param subdir 子目录名（如 "shader" 或 "images"）
 * @param filename 文件名
 * @return 文件的完整路径
 */
std::string getResourcePath(const std::string& subdir, const std::string& filename) {
#ifdef DEBUG
    // Debug 模式：使用 CMake 定义的源码目录
    return std::string(SOURCE_DIR) + "/" + subdir + "/" + filename;
#else
    // Release 模式：使用可执行文件的相对路径
    return "../" + subdir + "/" + filename;
#endif
}

/**
 * @brief 获取着色器文件路径
 * @param filename 着色器文件名
 * @return 着色器文件的完整路径
 */
std::string getShaderPath(const std::string& filename) {
    return getResourcePath("shader", filename);
}

/**
 * @brief 获取图片文件路径
 * @param filename 图片文件名
 * @return 图片文件的完整路径
 */
std::string getImagePath(const std::string& filename) {
    return getResourcePath("images", filename);
}

/**
 * @brief 矩形的顶点数据（包含位置和纹理坐标）
 * 
 * 每个顶点包含 5 个浮点数：
 * - 位置 (x, y, z)：标准化设备坐标 (NDC)，范围 [-1, 1]
 * - 纹理坐标 (s, t)：纹理坐标，范围 [0, 1]
 * 
 * 矩形由 4 个顶点组成：
 *   3(0,1) -------- 0(1,1)
 *   |                    |
 *   |      纹理           |
 *   |                    |
 *   2(0,0) -------- 1(1,0)
 * 
 * 纹理坐标系统：
 * - (0,0) 在左下角
 * - (1,1) 在右上角
 */
float vertices[] = {
    // 位置 (x, y, z)        // 纹理坐标 (s, t)
     0.5f,  0.5f, 0.0f,      1.0f, 1.0f,   // 顶点 0：右上角
     0.5f, -0.5f, 0.0f,      1.0f, 0.0f,   // 顶点 1：右下角
    -0.5f, -0.5f, 0.0f,      0.0f, 0.0f,   // 顶点 2：左下角
    -0.5f,  0.5f, 0.0f,      0.0f, 1.0f    // 顶点 3：左上角
};

/**
 * @brief 矩形的索引数据
 * 
 * 指定如何使用顶点数据构建三角形。
 * 一个矩形由两个三角形组成：
 * 
 * 三角形 1：顶点 0 -> 1 -> 3（右上 -> 右下 -> 左上）
 * 三角形 2：顶点 1 -> 2 -> 3（右下 -> 左下 -> 左上）
 * 
 *   3 -------- 0         3 -------- 0
 *   | \        |         |        / |
 *   |   \  T1  |   -->   |  T2  /   |
 *   |     \    |         |    /     |
 *   2 -------- 1         2 -------- 1
 * 
 * 索引顺序决定了三角形的绕序（逆时针为正面）
 */
unsigned int indices[] = {
    0, 1, 3,   // 第一个三角形：右上 -> 右下 -> 左上
    1, 2, 3    // 第二个三角形：右下 -> 左下 -> 左上
};

} // namespace

// ==================== 构造函数和析构函数 ====================

MyWindow::MyWindow()
    : OpenGLWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL")
{
    // 基类构造函数已创建窗口和 OpenGL 上下文
}

MyWindow::~MyWindow()
{
    // 确保资源被正确释放
    cleanup();
}

// ==================== 初始化 ====================

bool MyWindow::initialize()
{
    // 确保当前线程的 OpenGL 上下文是此窗口的上下文
    SDL_GL_MakeCurrent(m_window, m_glContext);

    // ==================== 第一步：加载着色器 ====================
    std::string vertexPath = getShaderPath("vertex.glsl");
    std::string fragmentPath = getShaderPath("fragment.glsl");
    
    NEAPU_LOGI("Loading vertex shader from: {}", vertexPath);
    NEAPU_LOGI("Loading fragment shader from: {}", fragmentPath);
    
    if (!m_shader.loadFromFile(vertexPath, fragmentPath)) {
        NEAPU_LOGE("Failed to load shader from files");
        return false;
    }

    // ==================== 第二步：加载纹理 ====================
    std::string imagePath = getImagePath("image1.png");
    NEAPU_LOGI("Loading texture from: {}", imagePath);
    
    // 创建纹理对象
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    
    // 设置纹理环绕方式
    // GL_REPEAT：纹理坐标超出 [0,1] 时重复纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // 设置纹理过滤方式
    // GL_LINEAR：线性插值，产生平滑效果
    // GL_LINEAR_MIPMAP_LINEAR：三线性过滤，在 mipmap 之间也进行插值
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // 使用 stb_image 加载图片
    // stbi_set_flip_vertically_on_load(true) 翻转图片，因为 OpenGL 纹理坐标 y=0 在底部
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &nrChannels, 0);
    
    if (data) {
        // 根据通道数确定图片格式
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        
        NEAPU_LOGI("Texture loaded: {}x{}, {} channels", width, height, nrChannels);
        
        // 上传纹理数据到 GPU
        glTexImage2D(
            GL_TEXTURE_2D,      // 纹理类型
            0,                  // mipmap 级别（0 为基础级别）
            format,             // 纹理内部格式
            width,              // 宽度
            height,             // 高度
            0,                  // 边框（必须为 0）
            format,             // 源图片格式
            GL_UNSIGNED_BYTE,   // 数据类型
            data                // 图片数据
        );
        
        // 自动生成 mipmap
        // Mipmap 是预计算的缩小版本纹理，用于远距离渲染
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        NEAPU_LOGE("Failed to load texture: {}", imagePath);
        return false;
    }
    
    // 释放图片数据（已上传到 GPU）
    stbi_image_free(data);

    // ==================== 第三步：创建 VAO、VBO 和 EBO ====================
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // 绑定 VAO
    glBindVertexArray(m_VAO);

    // 绑定 VBO 并上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定 EBO 并上传索引数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // ==================== 第四步：配置顶点属性 ====================
    // 顶点数据布局：| 位置 (3 floats) | 纹理坐标 (2 floats) |
    // 步长 = 5 * sizeof(float) = 20 字节
    
    // 位置属性 (location = 0)
    glVertexAttribPointer(
        0,                   // 属性索引
        3,                   // 分量数量（vec3）
        GL_FLOAT,            // 数据类型
        GL_FALSE,            // 是否标准化
        5 * sizeof(float),   // 步长
        (void*)0             // 偏移
    );
    glEnableVertexAttribArray(0);
    
    // 纹理坐标属性 (location = 1)
    glVertexAttribPointer(
        1,                   // 属性索引
        2,                   // 分量数量（vec2）
        GL_FLOAT,            // 数据类型
        GL_FALSE,            // 是否标准化
        5 * sizeof(float),   // 步长
        (void*)(3 * sizeof(float))  // 偏移：跳过位置数据
    );
    glEnableVertexAttribArray(1);

    // 解绑 VAO
    glBindVertexArray(0);

    return true;
}

// ==================== 事件处理 ====================

void MyWindow::processEvents(SDL_Event &event)
{
    // 处理窗口大小调整事件
    // 当窗口大小改变时，需要更新 OpenGL 视口以匹配新的窗口尺寸
    if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        // glViewport 定义了渲染输出到窗口的哪个区域
        // 参数：(x, y, width, height) - 左下角坐标和宽高
        glViewport(0, 0, event.window.data1, event.window.data2);
    }
    
    // 调用父类的事件处理（处理其他通用事件）
    OpenGLWindow::processEvents(event);
}

// ==================== 渲染 ====================

void MyWindow::render()
{
    // 第一步：清除颜色缓冲区
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 第二步：绑定纹理
    // 将纹理绑定到纹理单元 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    // 第三步：激活着色器并设置纹理采样器
    m_shader.use();
    // 设置 uniform 采样器使用纹理单元 0
    m_shader.setInt("texture1", 0);

    // 第四步：绑定 VAO 并绘制
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // 第五步：交换缓冲区
    SDL_GL_SwapWindow(m_window);
}

// ==================== 资源清理 ====================

void MyWindow::cleanup()
{
    // 删除 VAO
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    
    // 删除 VBO
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    
    // 删除 EBO
    if (m_EBO) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    
    // 删除纹理
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
    
    // 注意：m_shader 会在析构时自动清理，无需手动处理
}
