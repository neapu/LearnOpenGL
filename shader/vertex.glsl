/**
 * @file vertex.glsl
 * @brief 顶点着色器
 * 
 * 功能：
 * 1. 接收顶点位置和颜色属性
 * 2. 将位置传递到裁剪空间
 * 3. 将颜色传递给片段着色器
 */

#version 330 core

// 顶点属性输入
layout (location = 0) in vec3 aPos;    // 顶点位置 (x, y, z)
layout (location = 1) in vec3 aColor;  // 顶点颜色 (r, g, b)

// 输出到片段着色器
out vec3 ourColor;  // 传递给片段着色器的颜色

void main()
{
    // 将顶点位置转换为裁剪空间坐标
    // vec4 的第四个分量 w = 1.0 表示这是一个位置（而非方向）
    gl_Position = vec4(aPos, 1.0);
    
    // 将顶点颜色传递给片段着色器
    // 在光栅化阶段，颜色会在顶点之间进行插值
    ourColor = aColor;
}
