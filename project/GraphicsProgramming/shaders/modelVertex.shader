#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normals;
out vec4 FragPos;
out mat3 TBN;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    TexCoords = aTexCoords;
    Normals = normalize(mat3(inverse(transpose(world))) * aNormal);

    vec3 t = normalize(mat3(world) * aTangent);
    vec3 b = normalize(mat3(world) * aBitangent);
    vec3 n = normalize(mat3(world) * aNormal);
    TBN = mat3(t, b, n);

    FragPos = world * vec4(aPos, 1.0);
    gl_Position = projection * view * FragPos;
}