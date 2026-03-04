#version 450 core
//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

layout (location = 0) in vec3 _in_color;
layout (location = 1) in vec2 _in_uv;

layout (set = 0, binding = 0) uniform Camera {
	mat4 view;
	mat4 projection;
} _camera;

layout (set = 0, binding = 1) uniform sampler2D _texture_sampler;

layout (push_constant) uniform Pushed {
	mat4 model;
	mat4 normal;
} _pushed;

layout (location = 0) out vec4 _out_color;

void main() {
	_out_color = vec4(_in_color * texture(_texture_sampler, _in_uv).rgb, 1.0);
}