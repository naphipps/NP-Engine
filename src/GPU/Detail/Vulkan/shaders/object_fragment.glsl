#version 450 core
//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

layout (binding = 0) uniform PipelineMetaValues {
	mat4 view;
	mat4 projection;
} pipeline_meta;

layout (binding = 1) uniform sampler2D texture_sampler;

layout (location = 0) in vec3 in_color;
layout (location = 1) in vec2 in_texture_coordinate;

layout (location = 0) out vec4 out_color;

layout (push_constant) uniform RenderableMetaValues {
	mat4 model;
	mat4 normal;
} renderable_meta;

void main() {
	out_color = vec4(in_color * texture(texture_sampler, in_texture_coordinate).rgb, 1.0);
}