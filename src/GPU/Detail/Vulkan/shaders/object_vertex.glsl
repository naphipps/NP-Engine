#version 450 core
//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

const int RenderableMetaValuesPaddingSize = 21; // sizeof(::np::gpu::RenderableMetaValues.padding)

layout (binding = 0) uniform PipelineMetaValues
{
	mat4 view;
	mat4 projection;
} _pipeline;

layout (location = 0) in vec3 _in_position;
layout (location = 1) in vec3 _in_color;
layout (location = 2) in vec2 _in_uv;

layout (location = 0) out vec3 _out_color;
layout (location = 1) out vec2 _out_uv;

layout (push_constant) uniform RenderableMetaValues
{
	float position[3];
	float orientation[4];
	float scale[3];
	int type;
	int padding[RenderableMetaValuesPaddingSize];
} _renderable;

struct Transform
{
	vec3 position;
	vec4 orientation;
	vec3 scale;
};

struct RenderableObject
{
	Transform transform;
	int type;
	int padding[RenderableMetaValuesPaddingSize];
};

RenderableObject ExtractRenderableObject()
{
	RenderableObject renderable;
	renderable.transform.position = vec3(_renderable.position[0], _renderable.position[1], _renderable.position[2]);
	renderable.transform.orientation = vec4(_renderable.orientation[0], _renderable.orientation[1], _renderable.orientation[2], _renderable.orientation[3]);
	renderable.transform.scale = vec3(_renderable.scale[0], _renderable.scale[1], _renderable.scale[2]);
	renderable.type = _renderable.type;

	for (int i=0; i<RenderableMetaValuesPaddingSize; i++)
		renderable.padding[i] = _renderable.padding[i];

	return renderable;
}

mat4 CreateTranslateMatrix(in vec3 translate)
{
	mat4 mat = mat4(1.0);
	mat[3] = vec4(translate, 1.0);
	return mat;
}

mat4 CreateOrientationMatrix(in vec4 quat)
{
	float xx = quat.x * quat.x;
	float yy = quat.y * quat.y;
	float zz = quat.z * quat.z;
	float xz = quat.x * quat.z;
	float xy = quat.x * quat.y;
	float yz = quat.y * quat.z;
	float wx = quat.w * quat.x;
	float wy = quat.w * quat.y;
	float wz = quat.w * quat.z;

	mat4 mat = mat4(1.0);
	mat[0][0] = 1.0 - 2.0 * (yy + zz);
	mat[0][1] = 2.0 * (xy + wz);
	mat[0][2] = 2.0 * (xz - wy);
	mat[1][0] = 2.0 * (xy - wz);
	mat[1][1] = 1.0 - 2.0 * (xx +  zz);
	mat[1][2] = 2.0 * (yz + wx);
	mat[2][0] = 2.0 * (xz + wy);
	mat[2][1] = 2.0 * (yz - wx);
	mat[2][2] = 1.0 - 2.0 * (xx +  yy);
	return mat;
}

mat4 CreateScaleMatrix(in vec3 scale)
{
	mat4 mat = mat4(1.0);
	mat[0][0] *= scale.x;
	mat[1][1] *= scale.y;
	mat[2][2] *= scale.z;
	return mat;
}

mat4 ToMat4(in Transform transform)
{
	mat4 translation = CreateTranslateMatrix(transform.position);
	mat4 orientation = CreateOrientationMatrix(transform.orientation);
	mat4 scalation = CreateScaleMatrix(transform.scale);
	return translation * orientation * scalation;
}

void main()
{
	RenderableObject renderable = ExtractRenderableObject();
	mat4 model = ToMat4(renderable.transform);
	gl_Position = _pipeline.projection * _pipeline.view * model * vec4(_in_position, 1.0);
	_out_color = _in_color;
	_out_uv = _in_uv;
}