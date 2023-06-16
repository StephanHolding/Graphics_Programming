#version 330 core
layout(location = 0) in vec3 aPos;
//layout(location = 1) in vec2 aTexCoords;

out vec2 fragCoords;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	fragCoords = (gl_Position.xy / gl_Position.w) * 0.5 + 0.5;
	//texCoords = aTexCoords;
}