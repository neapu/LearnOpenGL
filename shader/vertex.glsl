/**
 * @file vertex.glsl
 * @brief 顶点着色器 - 支持纹理映射
 * 
 * 功能：
 * 1. 接收顶点位置和纹理坐标属性
 * 2. 将位置传递到裁剪空间
 * 3. 将纹理坐标传递给片段着色器
 */

#version 330 core

// 顶点属性输入
layout (location = 0) in vec3 aPos;      // 顶点位置 (x, y, z)
layout (location = 1) in vec2 aTexCoord; // 纹理坐标 (s, t)

// 输出到片段着色器
out vec2 TexCoord;  // 传递给片段着色器的纹理坐标

void main()
{
    // 将顶点位置转换为裁剪空间坐标
    gl_Position = vec4(aPos, 1.0);
    
    // 将纹理坐标传递给片段着色器
    // 在光栅化阶段，纹理坐标会在顶点之间进行插值
    TexCoord = aTexCoord;
}
