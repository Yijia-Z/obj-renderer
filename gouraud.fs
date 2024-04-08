#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objColor;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float lightIntensity;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float ns;

void main()
{
    // Ambient lighting
    vec3 ambient = ka * lightIntensity * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = kd * diff * lightIntensity * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), ns);
    vec3 specular = ks * spec * lightIntensity * lightColor;  

    // Combine lighting components
    vec3 result = (ambient + diffuse + specular) * objColor;

    FragColor = vec4(result, 1.0);
}
