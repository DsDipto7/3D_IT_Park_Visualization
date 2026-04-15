#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 VertexAmbient;
in vec3 VertexDiffuse;
in vec3 VertexSpecular;

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

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 viewPos;
uniform int ambientEnabled;
uniform int diffuseEnabled;
uniform int specularEnabled;
uniform int shadingMode;

// Texture support
uniform sampler2D texture1;
uniform int textureMode;   // 0=no texture, 1=simple texture, 2=blended texture
uniform int useTexture;    // 0=this object has no texture, 1=use texture
uniform float texRepeat;   // UV tiling multiplier (default 1.0)

vec3 getEffectiveDiffuse() {
    if (useTexture == 0 || textureMode == 0) {
        return material.diffuse;
    }
    vec3 texColor = texture(texture1, TexCoord * texRepeat).rgb;
    if (textureMode == 1) {
        return texColor;  // simple: texture replaces color
    }
    // textureMode == 2: blended
    return material.diffuse * texColor;
}

vec3 getEffectiveAmbient() {
    if (useTexture == 0 || textureMode == 0) {
        return material.ambient;
    }
    vec3 texColor = texture(texture1, TexCoord * texRepeat).rgb;
    if (textureMode == 1) {
        return texColor * 0.22;
    }
    return material.ambient * texColor;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 result = material.emissive;

    if (shadingMode == 1) {
        result += getEffectiveAmbient() * VertexAmbient;
        result += getEffectiveDiffuse() * VertexDiffuse;
        result += material.specular * VertexSpecular;
        FragColor = vec4(result, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    if (dirLight.enabled == 1) {
        result += calcDirLight(dirLight, norm, viewDir);
    }

    for (int i = 0; i < NR_POINT_LIGHTS; ++i) {
        if (pointLights[i].enabled == 1) {
            result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
        }
    }

    if (spotLight.enabled == 1) {
        result += calcSpotLight(spotLight, norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = (diff > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0;

    vec3 ambient = getEffectiveAmbient() * light.ambient * float(ambientEnabled);
    vec3 diffuse = getEffectiveDiffuse() * light.diffuse * diff * float(diffuseEnabled);
    vec3 specular = material.specular * light.specular * spec * float(specularEnabled);
    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = (diff > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * distance + light.k_q * distance * distance);

    vec3 ambient = getEffectiveAmbient() * light.ambient * float(ambientEnabled);
    vec3 diffuse = getEffectiveDiffuse() * light.diffuse * diff * float(diffuseEnabled);
    vec3 specular = material.specular * light.specular * spec * float(specularEnabled);
    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = (theta > light.cutOff) ? 1.0 : 0.0;

    if (intensity <= 0.0) {
        return vec3(0.0);
    }

    float diff = max(dot(normal, lightDir), 0.0);
    float spec = (diff > 0.0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * distance + light.k_q * distance * distance);

    vec3 ambient = getEffectiveAmbient() * light.ambient * float(ambientEnabled);
    vec3 diffuse = getEffectiveDiffuse() * light.diffuse * diff * float(diffuseEnabled);
    vec3 specular = material.specular * light.specular * spec * float(specularEnabled);
    return (ambient + diffuse + specular) * attenuation * intensity;
}
