#version 450 core
//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/4/22
//
//##===----------------------------------------------------------------------===##//

layout (binding = 0) uniform PipelineMetaValues {
	mat4 view;
	mat4 projection;
} pipeline_meta;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;
layout (location = 2) in vec2 in_texture_coordinate;

layout (location = 0) out vec3 out_color;
layout (location = 1) out vec2 out_texture_coordinate;

layout (push_constant) uniform RenderableMetaValues {
	vec4 color;
	vec3 position;
	float radius;
	//probably need some padding here?
} renderable_meta;

void main() {
	gl_Position = pipeline_meta.projection * pipeline_meta.view /** renderable_meta.model*/ * vec4(in_position, 1.0);
	out_color = in_color;
	out_texture_coordinate = in_texture_coordinate;
}