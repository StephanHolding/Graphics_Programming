#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vertexUV;

layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

out vec2 uv;
out mat3 TBN;
out vec3 worldPosition;

uniform mat4 world, view, projection;

void main()
{
	gl_Position = projection * view * world * vec4(aPos, 1.0);
	uv = vertexUV;

	vec3 t = normalize(mat3(world) * vTangent);
	vec3 b = normalize(mat3(world) * vBitangent);
	vec3 n = normalize(mat3(world) * vNormal);
	TBN = mat3(t, b, n);

	worldPosition = mat3(world) * aPos;
}