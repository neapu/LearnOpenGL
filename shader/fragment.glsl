/**
 * @file fragment.glsl
 * @brief 片段着色器
 * 
 * 功能：
 * 1. 接收从顶点着色器插值后的颜色
 * 2. 输出最终的片段颜色
 * 
 * 注意：颜色在顶点之间会自动进行线性插值，
 * 这就是为什么矩形会显示渐变色效果。
 */

#version 330 core

// 从顶点着色器接收的插值颜色
in vec3 ourColor;

// 输出的片段颜色 (RGBA)
out vec4 FragColor;

void main()
{
    // 使用插值后的颜色，alpha 设为 1.0（完全不透明）
    FragColor = vec4(ourColor, 1.0);
}
