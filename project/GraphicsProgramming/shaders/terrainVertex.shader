#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vertexUV;

out vec2 uv;
out vec3 worldPosition;

uniform mat4 world, view, projection;
uniform sampler2D mainTex;

void main()
{
	//object space
	//vec3 objectSpaceOffset = aPos;

	//world space
	//vec4  worldPos = world * vec4(objectSpaceOffset, 1.0);
	//worldPos.y += texture(mainTex, vertexUV).r * 100.0;

	gl_Position = projection * view * world * vec4(aPos, 1.0);

	uv = vertexUV;

	worldPosition = mat3(world) * aPos;
}