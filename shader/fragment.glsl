/**
 * @file fragment.glsl
 * @brief 片段着色器 - 支持纹理采样
 * 
 * 功能：
 * 1. 接收从顶点着色器插值后的纹理坐标
 * 2. 从纹理采样器中采样颜色
 * 3. 输出最终的片段颜色
 */

#version 330 core

// 从顶点着色器接收的插值纹理坐标
in vec2 TexCoord;

// 输出的片段颜色 (RGBA)
out vec4 FragColor;

// 纹理采样器
// sampler2D 是一个特殊的 uniform 类型，用于从 2D 纹理中采样
uniform sampler2D texture1;

void main()
{
    // 使用纹理坐标从纹理中采样颜色
    // texture() 函数根据纹理坐标返回对应的颜色值
    FragColor = texture(texture1, TexCoord);
}
