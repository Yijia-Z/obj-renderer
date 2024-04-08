#version 330 core

// vec4 aPos for CPU, vec3 aPos for GPU
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec3 ViewPos;
out vec3 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Uncomment for GPU-side
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    LightPos = vec3(view * vec4(3.0f, -1.0f, 10.0f, 1.0f)); // Light position in view space
    ViewPos = vec3(inverse(view)[3]); // Camera position in world space
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Uncomment for CPU-side
    // gl_Position = aPos;

    //Uncomment for zbuffer
    // vec3 zBuff = vec3(1) * ((gl_Position.z - 5.0) / gl_Position.w);
    // vertexColor = zBuff;
    // Uncomment for non-zbuffer
    // vertexColor = vec3(0);
}
