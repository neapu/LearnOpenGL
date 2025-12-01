#include "MyWindow.h"
#include <glad/glad.h>
#include <logger.h>
#include <string>

// ==================== 匿名命名空间 ====================
// 存放仅在本文件中使用的常量和数据

namespace {

/// 窗口默认宽度（像素）
constexpr int SCREEN_WIDTH = 800;

/// 窗口默认高度（像素）
constexpr int SCREEN_HEIGHT = 600;

/**
 * @brief 获取着色器文件路径
 * 
 * Debug 模式：使用 SOURCE_DIR 宏获取项目根目录
 * Release 模式：使用可执行文件的相对路径
 * 
 * @param filename 着色器文件名
 * @return 着色器文件的完整路径
 */
std::string getShaderPath(const std::string& filename) {
#ifdef DEBUG
    // Debug 模式：使用 CMake 定义的源码目录
    return std::string(SOURCE_DIR) + "/shader/" + filename;
#else
    // Release 模式：使用可执行文件的相对路径
    return "../shader/" + filename;
#endif
}

/**
 * @brief 矩形的顶点数据（包含位置和颜色）
 * 
 * 每个顶点包含 6 个浮点数：
 * - 位置 (x, y, z)：标准化设备坐标 (NDC)，范围 [-1, 1]
 * - 颜色 (r, g, b)：颜色分量，范围 [0, 1]
 * 
 * 矩形由 4 个顶点组成，每个顶点有不同的颜色：
 *   3(蓝色) -------- 0(红色)
 *   |                    |
 *   |                    |
 *   |                    |
 *   2(黄色) -------- 1(绿色)
 * 
 * 在光栅化阶段，颜色会在顶点之间进行线性插值，
 * 产生渐变色效果。
 */
float vertices[] = {
    // 位置 (x, y, z)        // 颜色 (r, g, b)
     0.5f,  0.5f, 0.0f,      1.0f, 0.0f, 0.0f,   // 顶点 0：右上角 - 红色
     0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f,   // 顶点 1：右下角 - 绿色
    -0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 0.0f,   // 顶点 2：左下角 - 黄色
    -0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f    // 顶点 3：左上角 - 蓝色
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

    // 第一步：从文件加载并编译着色器程序
    // 着色器程序是运行在 GPU 上的小程序，用于处理顶点和片段
    std::string vertexPath = getShaderPath("vertex.glsl");
    std::string fragmentPath = getShaderPath("fragment.glsl");
    
    NEAPU_LOGI("Loading vertex shader from: {}", vertexPath);
    NEAPU_LOGI("Loading fragment shader from: {}", fragmentPath);
    
    if (!m_shader.loadFromFile(vertexPath, fragmentPath)) {
        NEAPU_LOGE("Failed to load shader from files");
        return false;
    }

    // 第二步：创建 VAO、VBO 和 EBO
    // VAO (Vertex Array Object)：存储顶点属性配置的状态
    // VBO (Vertex Buffer Object)：存储实际的顶点数据
    // EBO (Element Buffer Object)：存储顶点索引，用于顶点复用
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    // 第三步：绑定 VAO
    // 注意：必须先绑定 VAO，后续的 VBO/EBO 和属性配置会记录到此 VAO
    glBindVertexArray(m_VAO);

    // 第四步：绑定 VBO 并上传顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,   // 缓冲区类型：顶点数据
                 sizeof(vertices),   // 数据大小（字节）
                 vertices,           // 数据指针
                 GL_STATIC_DRAW);    // 使用模式：数据不会频繁改变

    // 第五步：绑定 EBO 并上传索引数据
    // EBO 用于实现顶点复用，减少内存占用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,   // 缓冲区类型：索引数据
                 sizeof(indices),            // 数据大小（字节）
                 indices,                    // 数据指针
                 GL_STATIC_DRAW);            // 使用模式

    // 第六步：配置顶点属性
    // 顶点数据布局：| 位置 (3 floats) | 颜色 (3 floats) |
    // 步长 = 6 * sizeof(float) = 24 字节
    
    // 位置属性 (location = 0)
    glVertexAttribPointer(
        0,                   // 属性索引（对应着色器中的 location = 0）
        3,                   // 每个属性的分量数量（vec3 = 3 个分量）
        GL_FLOAT,            // 数据类型
        GL_FALSE,            // 是否标准化
        6 * sizeof(float),   // 步长（相邻顶点之间的字节偏移）
        (void*)0             // 属性在顶点数据中的起始偏移
    );
    glEnableVertexAttribArray(0);
    
    // 颜色属性 (location = 1)
    glVertexAttribPointer(
        1,                   // 属性索引（对应着色器中的 location = 1）
        3,                   // 每个属性的分量数量（vec3 = 3 个分量）
        GL_FLOAT,            // 数据类型
        GL_FALSE,            // 是否标准化
        6 * sizeof(float),   // 步长
        (void*)(3 * sizeof(float))  // 偏移：跳过前面 3 个 float（位置数据）
    );
    glEnableVertexAttribArray(1);

    // 解绑 VAO（可选，防止意外修改）
    // 注意：不要在 VAO 活动时解绑 EBO，因为 EBO 是 VAO 状态的一部分
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
    // 第一步：设置清除颜色并清除颜色缓冲区
    // 颜色值 (0.2, 0.3, 0.3) 是一种深青色
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 第二步：激活着色器程序
    m_shader.use();

    // 第三步：绑定 VAO
    // 绑定 VAO 后，OpenGL 会使用其中存储的顶点属性配置和 EBO
    glBindVertexArray(m_VAO);
    
    // 第四步：使用索引绘制矩形
    // glDrawElements 使用 EBO 中的索引来绘制图元
    glDrawElements(
        GL_TRIANGLES,       // 绘制模式：三角形
        6,                  // 索引数量（两个三角形，每个 3 个顶点）
        GL_UNSIGNED_INT,    // 索引数据类型
        0                   // EBO 中的偏移量（从开头开始）
    );

    // 第五步：交换缓冲区（双缓冲技术）
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
    
    // 删除 EBO
    if (m_EBO) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    
    // 注意：m_shader 会在析构时自动清理，无需手动处理
}
