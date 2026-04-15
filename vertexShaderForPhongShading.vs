#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

out vec3 VertexAmbient;
out vec3 VertexDiffuse;
out vec3 VertexSpecular;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    int enabled;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    int enabled;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    int enabled;
};

#define NR_POINT_LIGHTS 7

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform int ambientEnabled;
uniform int diffuseEnabled;
uniform int specularEnabled;
uniform int shadingMode;

void addDirLight(DirLight light, vec3 normal, vec3 viewDir, inout vec3 ambientAcc, inout vec3 diffuseAcc, inout vec3 specularAcc) {
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = (diff > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0;

    ambientAcc += light.ambient * float(ambientEnabled);
    diffuseAcc += light.diffuse * diff * float(diffuseEnabled);
    specularAcc += light.specular * spec * float(specularEnabled);
}

void addPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, inout vec3 ambientAcc, inout vec3 diffuseAcc, inout vec3 specularAcc) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = (diff > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * distance + light.k_q * distance * distance);

    ambientAcc += light.ambient * float(ambientEnabled) * attenuation;
    diffuseAcc += light.diffuse * diff * float(diffuseEnabled) * attenuation;
    specularAcc += light.specular * spec * float(specularEnabled) * attenuation;
}

void addSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, inout vec3 ambientAcc, inout vec3 diffuseAcc, inout vec3 specularAcc) {
    vec3 lightDir = normalize(light.position - fragPos);
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = (theta > light.cutOff) ? 1.0 : 0.0;

    if (intensity <= 0.0) {
        return;
    }

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = (diff > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * distance + light.k_q * distance * distance);

    ambientAcc += light.ambient * float(ambientEnabled) * attenuation * intensity;
    diffuseAcc += light.diffuse * diff * float(diffuseEnabled) * attenuation * intensity;
    specularAcc += light.specular * spec * float(specularEnabled) * attenuation * intensity;
}

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
    TexCoord = aTexCoord;

    VertexAmbient = vec3(0.0);
    VertexDiffuse = vec3(0.0);
    VertexSpecular = vec3(0.0);

    if (shadingMode == 1) {
        vec3 viewDir = normalize(viewPos - FragPos);

        if (dirLight.enabled == 1) {
            addDirLight(dirLight, Normal, viewDir, VertexAmbient, VertexDiffuse, VertexSpecular);
        }

        for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
            if (pointLights[i].enabled == 1) {
                addPointLight(pointLights[i], Normal, FragPos, viewDir, VertexAmbient, VertexDiffuse, VertexSpecular);
            }
        }

        if (spotLight.enabled == 1) {
            addSpotLight(spotLight, Normal, FragPos, viewDir, VertexAmbient, VertexDiffuse, VertexSpecular);
        }
    }

    gl_Position = projection * view * worldPos;
}
