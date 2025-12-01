#pragma once
#include "OpenGLWindow.h"
#include "Shader.h"

/**
 * @brief 主窗口类 - 实现具体的 OpenGL 渲染逻辑
 * 
 * 该类继承自 OpenGLWindow，实现了一个使用 EBO 绘制矩形的示例。
 * 
 * 主要职责包括：
 * - 初始化着色器程序
 * - 设置顶点数据、索引数据和属性
 * - 处理窗口事件（如窗口大小调整）
 * - 执行每帧的渲染操作
 * 
 * 渲染技术说明：
 * - 使用 VAO 存储顶点属性配置
 * - 使用 VBO 存储顶点数据
 * - 使用 EBO 存储索引数据，实现顶点复用
 * 
 * 使用 final 关键字禁止进一步继承，表示这是窗口类层次的最终实现。
 */
class MyWindow final : public OpenGLWindow {
public:
    /**
     * @brief 构造函数
     * 
     * 创建一个 800x600 的 OpenGL 窗口，标题为 "LearnOpenGL"
     */
    MyWindow();

    /**
     * @brief 析构函数
     * 
     * 自动调用 cleanup() 释放所有 OpenGL 资源
     */
    virtual ~MyWindow();

    /**
     * @brief 初始化渲染资源
     * 
     * 执行以下初始化操作：
     * 1. 编译和链接着色器程序
     * 2. 创建 VAO、VBO 和 EBO
     * 3. 上传顶点数据和索引数据
     * 4. 配置顶点属性
     * 
     * @return true  初始化成功
     * @return false 初始化失败（如着色器编译错误）
     */
    virtual bool initialize() override;

    /**
     * @brief 处理窗口事件
     * 
     * 处理以下事件：
     * - 窗口大小调整：更新 OpenGL 视口大小
     * - 其他事件：交给父类处理
     * 
     * @param event SDL 事件对象的引用
     */
    virtual void processEvents(SDL_Event& event) override;

    /**
     * @brief 渲染一帧
     * 
     * 执行以下渲染操作：
     * 1. 清除颜色缓冲区（使用深青色背景）
     * 2. 激活着色器程序
     * 3. 绑定 VAO 并使用索引绘制矩形
     * 4. 交换前后缓冲区（双缓冲）
     */
    virtual void render() override;

    /**
     * @brief 清理 OpenGL 资源
     * 
     * 释放以下资源：
     * - VAO（顶点数组对象）
     * - VBO（顶点缓冲对象）
     * - EBO（索引缓冲对象）
     * - 着色器程序由 m_shader 自动管理
     */
    virtual void cleanup() override;

private:
    Shader m_shader;          ///< 着色器程序对象，管理顶点着色器和片段着色器
    unsigned int m_VAO{0};    ///< 顶点数组对象 ID，存储顶点属性配置
    unsigned int m_VBO{0};    ///< 顶点缓冲对象 ID，存储顶点数据
    unsigned int m_EBO{0};    ///< 索引缓冲对象 ID，存储顶点索引（用于顶点复用）
};