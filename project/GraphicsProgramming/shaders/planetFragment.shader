#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D day, night, clouds;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}
vec3 lerp(vec3 a, vec3 b, float t)
{
    return a + (b - a) * t;
}
vec4 lerp(vec4 a, vec4 b, float t)
{
    return a + (b - a) * t;
}

void main()
{

    vec4 dayColor = texture(day, TexCoords);
    vec4 nightColor = texture(night, TexCoords);
    vec4 cloudsColor = texture(clouds, TexCoords);

    float light = max(dot(-lightDirection, Normals), 0.0);

    vec3 viewDir = normalize(cameraPosition - FragPos.rgb);
    vec3 refl = reflect(lightDirection, Normals);
    
    float spec = pow(max(dot(viewDir, refl), 0.0), 6.0);
    vec3 specular = spec * vec3(0.2, 0.3, 0.6);
    
    vec4 fragOutput = lerp(nightColor, dayColor, light) + vec4(specular, 0);

    FragColor = fragOutput;
}