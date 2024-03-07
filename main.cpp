// template based on material from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, glm::vec3 &modelPos, float &rotationAngle, float &scale);
std::string readFile(const std::string &filename);
void loadOBJ(const std::string &filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &colors, std::vector<unsigned int> &indices);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glew: load all OpenGL function pointers
    glewInit();

    // build and compile our shader program
    // ------------------------------------
    std::string vertexShaderSource = readFile("source.vs");
    std::string fragmentShaderSource = readFile("source.fs");

    const char *vertexShaderSourcePtr = vertexShaderSource.c_str();
    const char *fragmentShaderSourcePtr = fragmentShaderSource.c_str();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSourcePtr, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
    loadOBJ("./data/pig.obj", vertices, colors, indices);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + colors.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(vertices.size() * sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // uncomment this call to draw in wireframe polygons.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    glm::vec3 modelPos(0.0f, 0.0f, -10.0f);
    float rotationAngle = 0.0f;
    float scale = 1.0f;
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window, modelPos, rotationAngle, scale);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        model = glm::translate(glm::mat4(1.0f), modelPos);
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale, scale));
        glm::mat4 mvp = projection * view * model;
        unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, glm::vec3 &modelPos, float &rotationAngle, float &scale)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        modelPos.z += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        modelPos.z -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        modelPos.x -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        modelPos.x += cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        modelPos.y -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        modelPos.y += cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        rotationAngle += 0.1 * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        rotationAngle -= 0.1 * cameraSpeed;
    // scaling
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        scale *= (1 + 0.1 * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        scale *= (1 - 0.1 * cameraSpeed);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

std::map<std::string, Material> materials;

void loadOBJ(const std::string &filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &colors, std::vector<unsigned int> &indices)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_colors;

    std::string line;
    std::string material_file;
    glm::vec3 current_color(1.0f, 0.5f, 0.2f); // default color

    std::ifstream mtl_file;
    Material current_material;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            temp_vertices.emplace_back(x, y, z);
            temp_colors.emplace_back(current_color);
        }
        else if (prefix == "mtllib")
        {
            std::string mtl_filename;
            iss >> mtl_filename;
            mtl_file.open("./data/" + mtl_filename);
            if (!mtl_file.is_open())
            {
                std::cerr << "Failed to open material file: " << mtl_filename << std::endl;
                continue;
            }

            std::string mtl_line;
            Material temp_material;
            std::string current_mtl_name;

            while (std::getline(mtl_file, mtl_line))
            {
                std::istringstream mtl_iss(mtl_line);
                std::string mtl_prefix;
                mtl_iss >> mtl_prefix;

                if (mtl_prefix == "newmtl")
                {
                    if (!current_mtl_name.empty())
                    {
                        materials[current_mtl_name] = temp_material;
                    }
                    mtl_iss >> current_mtl_name;
                    temp_material = Material();
                }
                else if (mtl_prefix == "Ka")
                {
                    float r, g, b;
                    mtl_iss >> r >> g >> b;
                    temp_material.ambient = glm::vec3(r, g, b);
                }
                else if (mtl_prefix == "Kd")
                {
                    float r, g, b;
                    mtl_iss >> r >> g >> b;
                    temp_material.diffuse = glm::vec3(r, g, b);
                }
                else if (mtl_prefix == "Ks")
                {
                    float r, g, b;
                    mtl_iss >> r >> g >> b;
                    temp_material.specular = glm::vec3(r, g, b);
                }
                else if (mtl_prefix == "Ns")
                {
                    float shininess;
                    mtl_iss >> shininess;
                    temp_material.shininess = shininess;
                }
            }

            if (!current_mtl_name.empty())
            {
                materials[current_mtl_name] = temp_material;
            }

            mtl_file.close();
        }
        else if (prefix == "usemtl")
        {
            std::string material_name;
            iss >> material_name;
            if (materials.count(material_name))
            {
                current_material = materials[material_name];
                current_color = current_material.diffuse;
            }
            else
            {
                std::cerr << "Warning: Material '" << material_name << "' not found" << std::endl;
            }
        }
        else if (prefix == "f")
        {
            std::vector<unsigned int> face_vertices;
            std::string vertex_data;
            while (iss >> vertex_data)
            {
                size_t delimiter_pos = vertex_data.find('/');
                if (delimiter_pos == std::string::npos)
                {
                    // Vertex only
                    face_vertices.push_back(std::stoul(vertex_data) - 1);
                }
                else
                {
                    // Vertex and texture coordinate (or normal)
                    face_vertices.push_back(std::stoul(vertex_data.substr(0, delimiter_pos)) - 1);
                }
            }

            // Triangulate the face
            for (size_t i = 2; i < face_vertices.size(); ++i)
            {
                indices.push_back(face_vertices[0]);
                indices.push_back(face_vertices[i - 1]);
                indices.push_back(face_vertices[i]);

                // Assign the current color to the vertices of the face
                colors.push_back(current_color);
                colors.push_back(current_color);
                colors.push_back(current_color);
            }
        }
    }

    file.close();

    vertices = std::move(temp_vertices);
    colors = std::move(temp_colors);
}