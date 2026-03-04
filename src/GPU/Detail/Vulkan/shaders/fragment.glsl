#version 450 core
//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

layout(location = 0) in vec4 in_color;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = in_color;
}