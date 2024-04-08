#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

uniform vec3 objColor;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float ns;
uniform vec3 lightColor;
uniform float lightIntensity;

void main()
{
    // Ambient
    vec3 ambient = ka * lightIntensity * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff * lightIntensity * lightColor;

    // Specular
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 viewDir = normalize(-FragPos); // In flat shading, the view direction is constant for all fragments
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), ns);
    vec3 specular = ks * spec * lightIntensity * lightColor;

    vec3 result = (ambient + diffuse + specular) * objColor;
    FragColor = vec4(result, 1.0);
}
