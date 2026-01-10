// Main shader to clearing render target's color and alpha flag textures

#[vertex]
#version 450 core

vec2 uvs[3] = vec2[](
	vec2(0, 0), vec2(2, 0), vec2(0, 2)
);

void main() {
	vec2 uv = uvs[gl_VertexIndex];
	gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
}

#[fragment]
#version 450 core

layout(set = 0, binding = 0) uniform sampler2D screen;
layout(set = 0, binding = 1) uniform usampler2D screen_alpha;

layout(location = 0) out vec4 o_color;
layout(location = 1) out uint o_alpha_flag;

void main() {
    o_color = texelFetch(screen, ivec2(gl_FragCoord.xy), 0);
    o_alpha_flag = texelFetch(screen_alpha, ivec2(gl_FragCoord.xy), 0).r;
}