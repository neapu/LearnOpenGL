#include "MyWindow.h"
#include <glad/glad.h>
#include <logger.h>

// ==================== 匿名命名空间 ====================
// 存放仅在本文件中使用的常量和数据

namespace {

/// 窗口默认宽度（像素）
constexpr int SCREEN_WIDTH = 800;

/// 窗口默认高度（像素）
constexpr int SCREEN_HEIGHT = 600;

/**
 * @brief 三角形的顶点数据
 * 
 * 每个顶点包含 3 个浮点数 (x, y, z)
 * 坐标系统使用标准化设备坐标 (NDC)，范围为 [-1, 1]
 */
float vertices[] = {
    -0.5f, -0.5f, 0.0f,   // 左下角顶点
     0.5f, -0.5f, 0.0f,   // 右下角顶点
     0.0f,  0.5f, 0.0f    // 顶部顶点
};

/**
 * @brief 顶点着色器 GLSL 源代码
 * 
 * 功能：将顶点位置从属性传递到 gl_Position
 * - layout (location = 0)：指定属性位置索引为 0
 * - in vec3 aPos：输入的顶点位置（三维向量）
 * - gl_Position：内置输出变量，表示裁剪空间位置（四维齐次坐标）
 */
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)glsl";

/**
 * @brief 片段着色器 GLSL 源代码
 * 
 * 功能：为每个片段（像素）输出固定的橙色
 * - out vec4 FragColor：输出的片段颜色 (RGBA)
 * - 颜色值：(1.0, 0.5, 0.2, 1.0) 表示橙色，完全不透明
 */
const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)glsl";

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

    // 第一步：编译着色器程序
    // 着色器程序是运行在 GPU 上的小程序，用于处理顶点和片段
    if (!m_shader.compile(vertexShaderSource, fragmentShaderSource)) {
        NEAPU_LOGE("Failed to compile shader");
        return false;
    }

    // 第二步：创建 VAO 和 VBO
    // VAO (Vertex Array Object)：存储顶点属性配置的状态
    // VBO (Vertex Buffer Object)：存储实际的顶点数据
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    // 第三步：绑定 VAO（后续的 VBO 和属性配置会记录到此 VAO）
    glBindVertexArray(m_VAO);

    // 第四步：绑定 VBO 并上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,   // 缓冲区类型
                 sizeof(vertices),   // 数据大小（字节）
                 vertices,           // 数据指针
                 GL_STATIC_DRAW);    // 使用模式：数据不会频繁改变

    // 第五步：配置顶点属性
    // 告诉 OpenGL 如何解释 VBO 中的顶点数据
    glVertexAttribPointer(
        0,                   // 属性索引（对应着色器中的 location = 0）
        3,                   // 每个顶点属性的分量数量（vec3 = 3 个分量）
        GL_FLOAT,            // 数据类型
        GL_FALSE,            // 是否标准化（映射到 [0,1] 或 [-1,1]）
        3 * sizeof(float),   // 步长（相邻顶点属性之间的字节偏移）
        (void*)0             // 属性在缓冲区中的起始偏移
    );
    
    // 启用顶点属性（索引 0）
    glEnableVertexAttribArray(0);

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
    // 第一步：设置清除颜色并清除颜色缓冲区
    // 颜色值 (0.2, 0.3, 0.3) 是一种深青色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 第二步：激活着色器程序
    m_shader.use();

    // 第三步：绑定 VAO 并绘制
    // 绑定 VAO 后，OpenGL 会使用其中存储的顶点属性配置
    glBindVertexArray(m_VAO);
    
    // 绘制三角形
    // GL_TRIANGLES：绘制模式（每 3 个顶点组成一个三角形）
    // 0：起始顶点索引
    // 3：顶点数量
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // 第四步：交换缓冲区（双缓冲技术）
    // 前缓冲区显示在屏幕上，后缓冲区用于绘制
    // 交换后，新绘制的内容会显示在屏幕上
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
    
    // 注意：m_shader 会在析构时自动清理，无需手动处理
}
