
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void main()
{  
    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec4 specTex = texture(texture_specular1, TexCoords);

    float light = max(dot(-lightDirection, Normals), 0.0);

    vec3 viewDir = normalize(cameraPosition - FragPos.rgb);
    vec3 refl = reflect(lightDirection, Normals);

    float ambientOcclusion = texture(texture_ao1, TexCoords).r;
    
    float roughness = texture(texture_roughness1, TexCoords).r;
    float spec = pow(max(dot(viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb;

    vec3 viewDirection = normalize(worldPosition - cameraPosition);
    float fog = pow(clamp((distanceToCam - 250) / 1000, 0, 1), 2);
    vec3 skyTopColor = vec3(68.0 / 255.0, 118.0 / 255.0, 189.0 / 255.0);
    vec3 skyBottomColor = vec3(188.0 / 255.0, 214.0 / 255.0, 231.0 / 255.0);
    vec3 fogColor = lerp3(skyBottomColor, skyTopColor, max(viewDirection.y, 0.0));
    
    FragColor = diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0);
}