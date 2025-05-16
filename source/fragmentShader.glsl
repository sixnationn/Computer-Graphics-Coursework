#version 330 core
in vec3 FragPos;
in vec2 UV;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform int surfaceType;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform float spotCutOff;
uniform float spotOuterCutOff;
uniform vec3 spotLightColor;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 diffuseTex = texture(diffuseMap, UV).rgb;
    if(diffuseTex == vec3(0.0)) {
        if(surfaceType == 0)
            diffuseTex = vec3(0.5);
        else if(surfaceType == 1)
            diffuseTex = vec3(0.7);
        else
            diffuseTex = vec3(0.6, 0.3, 0.3);
    }

   
    vec3 ambient = 0.2 * diffuseTex;

    //lighting and reflective work
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    float specStrength = max(texture(specularMap, UV).r, 0.5);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 diffuse = diff * diffuseTex * lightColor;
    vec3 specular = spec * specStrength * lightColor;

    vec3 lightToFrag = normalize(FragPos - spotLightPos);
    float theta = dot(lightToFrag, normalize(spotLightDir)); // NO negation here
    float epsilon = spotCutOff - spotOuterCutOff;
    float intensity = clamp((theta - spotOuterCutOff) / epsilon, 0.0, 1.0);

    float distance = length(spotLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float spotDiff = max(dot(normal, -normalize(spotLightDir)), 0.0);
    vec3 spotDiffuse = spotDiff * diffuseTex * spotLightColor * intensity * attenuation;

    vec3 spotReflectDir = reflect(spotLightDir, normal);
    float spotSpec = pow(max(dot(viewDir, spotReflectDir), 0.0), 32.0);
    vec3 spotSpecular = specStrength * spotSpec * spotLightColor * intensity * attenuation;

    vec3 result = ambient + diffuse + specular + spotDiffuse + spotSpecular;

    if(surfaceType == 1 && FragPos.y > 0.0)
        result *= 0.8;

    FragColor = vec4(result, 1.0);
}
