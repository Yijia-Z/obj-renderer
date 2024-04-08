// template based on material from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, glm::mat4 &projection);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void loadOBJ(const std::string &filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &colors, std::vector<unsigned int> &indices);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

struct Material
{
    glm::vec3 color;
    float kd, ks, ka, ns;
    string name;
    Material()
    {
        color = glm::vec3(0.0f);
        kd = 0.0f;
        ks = 0.0f;
        ka = 0.0f;
        ns = 0.0f;
    }
    Material(glm::vec3 a, float b, float c, float d, float e)
    {
        color = a;
        kd = b;
        ks = c;
        ka = d;
        ns = e;
    }
};

vector<float> loadObj(string filename, vector<Material> &materials);

struct Light
{
    glm::vec3 position, color;
    float intensity;

    Light(glm::vec3 a, glm::vec3 b, float c)
    {
        position = a;
        color = b;
        intensity = c;
    }
};
float userScaleFactor = 1.0f;
float rotX = 0.0f;
float rotY = 0.0f;
float rotZ = 0.0f;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.5f;

int main()
{
    string line, text;
    ifstream in("source.vs");
    while (getline(in, line))
    {
        text += line + "\n";
    }
    GLchar const *vertexShaderSource = text.c_str();

    string aline, atext;
    ifstream ain("source.fs");
    while (getline(ain, aline))
    {
        atext += aline + "\n";
    }
    GLchar const *fragmentShaderSource = atext.c_str();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

    // // glew: load all OpenGL function pointers
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }


    // Uncomment this part for the new implementation
    Light light = Light(glm::vec3(3.0f, -1.0f, 3.0f), glm::vec3(1.0f), 1.0f);
    vector<Material> materials = vector<Material>();
    vector<float> obj = loadObj("data/pawn.obj", materials);
    unsigned int numVertices = (obj.size() / 6);

    // Uncomment this part for the original CPU vs GPU-side implementation
    /*glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Toggle CPU-side vs GPU-side model transform
    bool CPUside = false;

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
    loadOBJ("./data/pawn.obj", vertices, colors, indices);*/

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Uncomment this part for the new implementation
    glBufferData(GL_ARRAY_BUFFER, obj.size() * sizeof(float), &obj.front(), GL_STATIC_DRAW);

    // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);


    // Uncomment this part for the original CPU vs GPU-side implementation
    /*if(CPUside){
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + colors.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3) + colors.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
    }
    std::vector<glm::vec4> transformedVertices(vertices.size());


    if (CPUside){
        glBufferSubData(GL_ARRAY_BUFFER, 0, transformedVertices.size() * sizeof(glm::vec4), &transformedVertices[0]);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(glm::vec3), &vertices[0]);
    }
    
    if (CPUside){
        glBufferSubData(GL_ARRAY_BUFFER, transformedVertices.size() * sizeof(glm::vec4), colors.size() * sizeof(glm::vec3), &colors[0]);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), colors.size() * sizeof(glm::vec3), &colors[0]);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    if (CPUside){
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    } else {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    }
    
    glEnableVertexAttribArray(0);

    if (CPUside){
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(vertices.size() * sizeof(glm::vec4)));
    } else {
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(vertices.size() * sizeof(glm::vec3)));
    }*/

    // attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (char *)(3 * sizeof(float)));

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    // wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);

    // projection
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    double startTime = glfwGetTime();
    int numFrames = 0;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        // Uncomment for the new implementation
        processInput(window, projection);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // draw our first triangle
        glUseProgram(shaderProgram);

        // view
        glm::mat4 view = glm::mat4(1.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
        view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        // model
        glm::mat4 model = glm::mat4(1.0f);
        // translate
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        // rotate
        model = glm::rotate(model, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
        // scale
        glm::vec3 scaleFactor = glm::vec3(userScaleFactor);
        model = glm::scale(model, scaleFactor);

        // send final matrix to vertex shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

        // send material and light to fragment shader
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPosition"), 1, &light.position[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &light.color[0]);
        glUniform1f(glGetUniformLocation(shaderProgram, "lightIntensity"), light.intensity);

        // Bind the VAO
        glBindVertexArray(VAO);

        // Draw each material group separately
        int offset = 0;
        for (const auto &mat : materials)
        {
            glUniform3fv(glGetUniformLocation(shaderProgram, "objColor"), 1, &mat.color[0]);
            glUniform1f(glGetUniformLocation(shaderProgram, "ka"), mat.ka);
            glUniform1f(glGetUniformLocation(shaderProgram, "ks"), mat.ks);
            glUniform1f(glGetUniformLocation(shaderProgram, "kd"), mat.kd);
            glUniform1f(glGetUniformLocation(shaderProgram, "ns"), mat.ns);

            // Draw the triangles for the current material
            glDrawArrays(GL_TRIANGLES, offset, numVertices);
            offset += numVertices;
        }

        // Unbind the VAO
        glBindVertexArray(0);

        
        // Uncomment this for CPU vs. GPU-side implementation
        /*if (CPUside == false){
            unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "mvp");
            glUseProgram(shaderProgram);
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
            
            glBindVertexArray(VAO);
        } else {
            for (size_t i = 0; i < vertices.size(); ++i) {
                glm::vec4 transformedVertex = mvp * glm::vec4(vertices[i], 1.0f);
                transformedVertices[i] = transformedVertex;//glm::vec3(transformedVertex);
            }
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, transformedVertices.size() * sizeof(glm::vec4), &transformedVertices[0]);
            
          
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
        }

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
        numFrames++;
        */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    double deltaTime = glfwGetTime() - startTime;

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    double fps = ((double) numFrames) / deltaTime;
    std::cout << "performance: " << fps << " frames per second";
    return 0;
}

void processInput(GLFWwindow *window, glm::mat4 &projection)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        userScaleFactor *= 1.01f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        userScaleFactor /= 1.01f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        rotX -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        rotX += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        rotY -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        rotY += 1.0f;
    if (glfwGetKey(window,GLFW_KEY_0) == GLFW_PRESS)
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        projection = glm::perspective(90.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        projection = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
    if(glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        projection = glm::perspective(45.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 10000.0f);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        userScaleFactor = 1.0f;
        rotX = 0.0f;
        rotY = 0.0f;
        rotZ = 0.0f;
        cameraPos = glm::vec3(0.0f, 0.0f, 8.0f);
        cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    }
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Comment this one in for new implementation
vector<float> loadObj(string filename, vector<Material> &materials)
{
    string line, text;
    ifstream in(filename);
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec3> facesVertices;
    vector<glm::vec3> facesNormals;
    vector<float> vertexes;
    string mtlFilename;
    string currentMaterial;

    // read file, vertices and faces
    while (getline(in, line))
    {
        // read in vertices
        if (line.rfind("v ", 0) == 0)
        {
            vector<string> tempVertex;
            istringstream iss(line);
            string temp;
            while (getline(iss, temp, ' '))
            {
                tempVertex.push_back(temp);
            }
            float x = stof(tempVertex[1]);
            float y = stof(tempVertex[2]);
            float z = stof(tempVertex[3]);
            vertices.push_back(glm::vec3(x, y, z));
        }
        // read in normals
        if (line.rfind("vn ", 0) == 0)
        {
            vector<string> tempNormal;
            istringstream iss(line);
            string temp;
            while (getline(iss, temp, ' '))
            {
                tempNormal.push_back(temp);
            }
            float x = stof(tempNormal[1]);
            float y = stof(tempNormal[2]);
            float z = stof(tempNormal[3]);
            normals.push_back(glm::vec3(x, y, z));
        }
        // read in faces
        if (line.rfind("f ", 0) == 0)
        {
            vector<string> tempFace;
            istringstream iss(line);
            string temp;
            while (getline(iss, temp, ' '))
            {
                tempFace.push_back(temp);
            }
            // triangulate the face
            for (long unsigned int i = 1; i < tempFace.size() - 1; i++)
            {
                // first vertex
                istringstream iss1(tempFace[1]);
                string temp1;
                getline(iss1, temp1, '/');
                int v1 = stoi(temp1) - 1;
                getline(iss1, temp1, '/');
                getline(iss1, temp1, '/');
                int n1 = stoi(temp1) - 1;
                // second vertex
                istringstream iss2(tempFace[i]);
                string temp2;
                getline(iss2, temp2, '/');
                int v2 = stoi(temp2) - 1;
                getline(iss2, temp2, '/');
                getline(iss2, temp2, '/');
                int n2 = stoi(temp2) - 1;
                // third vertex
                istringstream iss3(tempFace[i + 1]);
                string temp3;
                getline(iss3, temp3, '/');
                int v3 = stoi(temp3) - 1;
                getline(iss3, temp3, '/');
                getline(iss3, temp3, '/');
                int n3 = stoi(temp3) - 1;
                // add the triangle to the vertexes vector
                vertexes.push_back(vertices[v1].x);
                vertexes.push_back(vertices[v1].y);
                vertexes.push_back(vertices[v1].z);
                vertexes.push_back(normals[n1].x);
                vertexes.push_back(normals[n1].y);
                vertexes.push_back(normals[n1].z);
                vertexes.push_back(vertices[v2].x);
                vertexes.push_back(vertices[v2].y);
                vertexes.push_back(vertices[v2].z);
                vertexes.push_back(normals[n2].x);
                vertexes.push_back(normals[n2].y);
                vertexes.push_back(normals[n2].z);
                vertexes.push_back(vertices[v3].x);
                vertexes.push_back(vertices[v3].y);
                vertexes.push_back(vertices[v3].z);
                vertexes.push_back(normals[n3].x);
                vertexes.push_back(normals[n3].y);
                vertexes.push_back(normals[n3].z);
            }
        }
        // Below is for flat shading
        /*        if (line.rfind("f ", 0) == 0)
        {
            vector<string> tempFace;
            istringstream iss(line);
            string temp;
            while (getline(iss, temp, ' '))
            {
                tempFace.push_back(temp);
            }

            // triangulate the face
            for (long unsigned int i = 1; i < tempFace.size() - 1; i++)
            {
                // first vertex
                istringstream iss1(tempFace[1]);
                string temp1;
                getline(iss1, temp1, '/');
                int v1 = stoi(temp1) - 1;

                // second vertex
                istringstream iss2(tempFace[i]);
                string temp2;
                getline(iss2, temp2, '/');
                int v2 = stoi(temp2) - 1;

                // third vertex
                istringstream iss3(tempFace[i + 1]);
                string temp3;
                getline(iss3, temp3, '/');
                int v3 = stoi(temp3) - 1;

                // calculate the normal
                glm::vec3 p1 = vertices[v1];
                glm::vec3 p2 = vertices[v2];
                glm::vec3 p3 = vertices[v3];
                glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));

                // add the triangle to the vertexes vector
                vertexes.push_back(vertices[v1].x);
                vertexes.push_back(vertices[v1].y);
                vertexes.push_back(vertices[v1].z);
                vertexes.push_back(normal.x);
                vertexes.push_back(normal.y);
                vertexes.push_back(normal.z);

                vertexes.push_back(vertices[v2].x);
                vertexes.push_back(vertices[v2].y);
                vertexes.push_back(vertices[v2].z);
                vertexes.push_back(normal.x);
                vertexes.push_back(normal.y);
                vertexes.push_back(normal.z);

                vertexes.push_back(vertices[v3].x);
                vertexes.push_back(vertices[v3].y);
                vertexes.push_back(vertices[v3].z);
                vertexes.push_back(normal.x);
                vertexes.push_back(normal.y);
                vertexes.push_back(normal.z);
            }
        }*/
        // read in MTL file
        if (line.rfind("mtllib ", 0) == 0)
        {
            mtlFilename = line.substr(7);
        }
        // read in material name
        if (line.rfind("usemtl ", 0) == 0)
        {
            currentMaterial = line.substr(7);
        }
    }

    ifstream mtlFile("data/" + mtlFilename);
    if (mtlFile.is_open())
    {
        Material currentMat = Material();
        while (getline(mtlFile, line))
        {
            if (line.rfind("newmtl ", 0) == 0)
            {
                if (!currentMat.color.r == 0 && !currentMat.color.g == 0 && !currentMat.color.b == 0)
                {
                    materials.push_back(currentMat);
                }
                currentMat = Material(glm::vec3(0.0f), 0.0f, 0.0f, 0.0f, 0.0f);
            }
            else if (line.rfind("Ka ", 0) == 0)
            {
                istringstream iss(line.substr(3));
                iss >> currentMat.ka;
            }
            else if (line.rfind("Kd ", 0) == 0)
            {
                istringstream iss(line.substr(3));
                iss >> currentMat.color.r >> currentMat.color.g >> currentMat.color.b;
                currentMat.kd = 1.0f;
            }
            else if (line.rfind("Ks ", 0) == 0)
            {
                istringstream iss(line.substr(3));
                iss >> currentMat.ks;
            }
            else if (line.rfind("Ns ", 0) == 0)
            {
                istringstream iss(line.substr(3));
                iss >> currentMat.ns;
            }
        }
        if (!currentMat.color.r == 0 && !currentMat.color.g == 0 && !currentMat.color.b == 0)
        {
            materials.push_back(currentMat);
        }
        in.close();
        mtlFile.close();
    }

    return vertexes;
}

// Old implementation, comment this one in for the CPU vs. GPU-side comparison

/*void loadOBJ(const std::string &filename, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &colors, std::vector<unsigned int> &indices){
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
                       // materials.push_back(temp_material);
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
    
}*/