#version 450 core
//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

layout(set = 0, binding = 1) uniform sampler2D sampler_texture;

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(in_color.rgb * texture(sampler_texture, in_uv).rgb, 1);
}