#[vertex]
#version 450 core

layout(location = 0) in vec2 i_vertex_position;
layout(location = 1) in vec4 i_vertex_color;
layout(location = 2) in vec2 i_vertex_uv;

layout(location = 0) out vec2 o_uv;
layout(location = 1) out vec4 o_color;

layout(push_constant, std430) uniform GeometryData {
	vec2 inv_viewport_size;
	mat4 transform;
} geometry_data;

void main() {
	// vec2 pos = (geometry_data.translation + i_vertex_position) * geometry_data.inv_viewport_size;
	vec2 pos = i_vertex_position;
	pos = (geometry_data.transform * vec4(pos, 0.0, 1.0)).xy;
	gl_Position = vec4(pos * geometry_data.inv_viewport_size * 2.0 - 1.0, 0.0, 1.0);
	o_color = i_vertex_color;
	o_uv = i_vertex_uv;
}

#[fragment]
#version 450 core

layout(location = 0) in vec2 i_uv;
layout(location = 1) in vec4 i_color;

layout(location = 0) out vec4 o_color;

layout(set = 0, binding = 0) uniform sampler2D tex;

void main() {
    o_color = texture(tex, i_uv) * i_color;
}