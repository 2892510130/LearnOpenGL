#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <cmath>
#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"

#ifndef ROOT_PATH
    #define ROOT_PATH "Wrong"
#endif

void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow* window);
void test1(GLFWwindow* window);
void test2(GLFWwindow* window);
void texture_test(GLFWwindow* window);
void coordinate_test(GLFWwindow* window);
void light_test(GLFWwindow* window);
void random_test(GLFWwindow* window);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float mixValue = 0.2f;
float deltaTime = 0.0f, lastFrame = 0.0f;
bool firstMouse = true;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
const unsigned int windowHeight = 600;
const unsigned int windowWidth = 800;

int main(int argc, char** argv)
{
    std::cout << "Type to check out different tests:\n"
        << "[0]: random test\n"
        << "[1]: render two triangles with different fragment shaders\n"
        << "[2]: glsl learning\n"
        << "[3]: texture test\n"
        << "[4]: coordinate test\n"
        << "[5]: light test\n";

    int task_select_index;
    std::cin >> task_select_index;

    std::vector<std::function<void(GLFWwindow* window)>> function_collection;
    function_collection.push_back(random_test);
    function_collection.push_back(test1);
    function_collection.push_back(test2);
    function_collection.push_back(texture_test);
    function_collection.push_back(coordinate_test);
    function_collection.push_back(light_test);

    // <- initialize GLFW ->
    glfwInit(); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set as core profile

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // <- initialize glad ->
    if (!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ))
    {
        std::cout << "Failed to initialize GLAD:\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    if (task_select_index >= 0 && task_select_index < function_collection.size())
    {
        function_collection[task_select_index](window);
    }

    glfwTerminate();

    return 0;
}

void print_mat4(const glm::mat4 &matrix)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }
    std::cout << "----------\n";
}

void random_test(GLFWwindow* window)
{
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    vec = trans * vec;
    std::cout << vec.x << vec.y << vec.z << std::endl;
    std::cout << std::string(ROOT_PATH) + "\\res" << std::endl;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

    glm::mat4 lookat = glm::lookAt(cameraPos, cameraTarget, up);
    print_mat4(lookat);

    // GLM is mat[col][row] !!!
    glm::mat4 rotate = glm::mat4(0.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    rotate[0][0] = cameraRight[0];
    rotate[1][0] = cameraRight[1];
    rotate[2][0] = cameraRight[2];
    rotate[0][1] = cameraUp[0];
    rotate[1][1] = cameraUp[1];
    rotate[2][1] = cameraUp[2];
    rotate[0][2] = cameraDirection[0];
    rotate[1][2] = cameraDirection[1];
    rotate[2][2] = cameraDirection[2];
    rotate[3][3] = 1;
    translate[3][0] = -cameraPos[0];
    translate[3][1] = -cameraPos[1];
    translate[3][2] = -cameraPos[2];
    glm::mat4 lookatHand = rotate * translate;
    print_mat4(lookatHand);
}

void light_test(GLFWwindow* window)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int VAO, VBO, lightVAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::string lightVertexShaderPath = std::string(ROOT_PATH) + "\\res\\shaders\\vertexLight.shader";
    std::string objectVertexShaderPath = std::string(ROOT_PATH) + "\\res\\shaders\\vertexObject.shader";
    std::string lightFragmentShaderPath = std::string(ROOT_PATH) + "\\res\\shaders\\fragmentLight.shader";
    std::string objectFragmentShaderPath = std::string(ROOT_PATH) + "\\res\\shaders\\fragmentObject.shader";
    Shader lightShader(lightVertexShaderPath.c_str(), lightFragmentShaderPath.c_str());
    Shader objectShader(objectVertexShaderPath.c_str(), objectFragmentShaderPath.c_str());

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        lightPos = glm::vec3(1.2f + std::sin(glfwGetTime()), 1.0f + std::cos(glfwGetTime()), 2.0f);

        process_input(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view       = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 model      = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller vertices
        glBindVertexArray(lightVAO);

        lightShader.use();
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        objectShader.use();
        objectShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        objectShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        objectShader.setVec3("lightPos",  lightPos);
        objectShader.setVec3("viewPos", camera.Position);
        objectShader.setMat4("model", model);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void coordinate_test(GLFWwindow* window)
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f,  -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  -0.5f,  1.0f, 1.0f,
        -0.5f, 0.5f,  -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
        0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 verticesPositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    unsigned int VAO, VBO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::string vertex_shader_path =
        std::string(ROOT_PATH) + "\\res\\shaders\\vertex3d.shader";

    std::string fragment_shader_path =
        std::string(ROOT_PATH) + "\\res\\shaders\\fragment3d.shader";

    Shader ourShader(vertex_shader_path.c_str(),
                    fragment_shader_path.c_str());
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    std::string texture_path1 = (std::string(ROOT_PATH) + "\\res\\textures\\container.jpg");
    int width, height, numberChannels;
    unsigned char* data = stbi_load(texture_path1.c_str(), &width, &height, &numberChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 1\n";
    }

    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::string texture_path2 = (std::string(ROOT_PATH) + "\\res\\textures\\awesomeface.png");
    data = stbi_load(texture_path2.c_str(), &width, &height, &numberChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 2\n";
    }

    stbi_image_free(data);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -------------------------------------------------------------------------------
        process_input(window);
        float mixPercentage;

        // render
        // -------------------------------------------------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        ourShader.setFloat("mixPercentage", mixValue);

        float timeValue = glfwGetTime() * 30;

        // create transformations
        glm::mat4 view          = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 projection    = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        view = camera.GetViewMatrix();
        // pass transformation matrices to the shader
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("view", view);

        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, verticesPositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle + timeValue), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);
}

void texture_test(GLFWwindow* window)
{
    std::string vertex_shader_path = (std::string(ROOT_PATH) + "\\res\\shaders\\vertexTexture.shader");
    std::string fragment_shader_path = (std::string(ROOT_PATH) + "\\res\\shaders\\fragmentTexture.shader");
    Shader ourShader(vertex_shader_path.c_str(), fragment_shader_path.c_str());

    // float vertices[] = {
    //     // positions          // colors           // texture coords
    //      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    //      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    //     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    // };
    // float vertices[] = {
    //     // positions          // colors           // texture coords
    //      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.5f, 0.5f, // top right
    //      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.5f, 0.0f, // bottom right
    //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    //     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.5f  // top left 
    // };
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // the position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // the location
    // the color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // the texture coordinate
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE GL_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

    std::string texture_path1 = (std::string(ROOT_PATH) + "\\res\\textures\\container.jpg");
    int width, height, numberChannels;
    unsigned char* data = stbi_load(texture_path1.c_str(), &width, &height, &numberChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 1\n";
    }

    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    std::string texture_path2 = (std::string(ROOT_PATH) + "\\res\\textures\\awesomeface.png");
    data = stbi_load(texture_path2.c_str(), &width, &height, &numberChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 2\n";
    }

    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    glm::mat4 trans = glm::mat4(1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        trans = glm::rotate(trans, glm::radians(mixValue), glm::vec3(0.0, 0.0, 1.0));
        unsigned int transLocation = glGetUniformLocation(ourShader.ID, "transform");
        glUniformMatrix4fv(transLocation, 1, GL_FALSE, glm::value_ptr(trans));

        // render container
        // ourShader.use();
        ourShader.setFloat("mixPercentage", mixValue);
        // glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void test2(GLFWwindow* window)
{
    int number_of_vertex_attributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &number_of_vertex_attributes);
    std::cout << "Maximum number of vertex attributes supported: " << number_of_vertex_attributes << '\n';

    std::string vertex_shader_path = (std::string(ROOT_PATH) + "\\res\\shaders\\vertex.shader");
    std::string fragment_shader_path = (std::string(ROOT_PATH) + "\\res\\shaders\\fragment.shader");
    Shader shader_source(vertex_shader_path.c_str(), fragment_shader_path.c_str());

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // the position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // the location
    // the color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClear(GL_COLOR_BUFFER_BIT);

        shader_source.use(); // use the program

        float timeValue = static_cast<float>(glfwGetTime());
        float colorValue = (sin(timeValue) / 2.0 + 0.5) * 0.33;
        int vertexColorLocation = glGetUniformLocation(shader_source.ID, "uniformColor");
        glUniform4f(vertexColorLocation, colorValue, colorValue, colorValue, 1.0f);
        shader_source.setFloat("xPosOffset", sin(timeValue) * 0.3);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void test1(GLFWwindow* window)
{
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";

    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    const char* fragmentShaderSource2 = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.5f, 1.0f, 0.6f, 1.0f);\n"
        "}\0";
        
    // <- vertex shader compilation ->
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
    // count is the number of the string, here only one string
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);     
    glCompileShader(vertexShader);

    // <- frament shader compilation ->
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER), fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glShaderSource(fragmentShader2, 1, &fragmentShaderSource2, NULL);
    glCompileShader(fragmentShader);
    glCompileShader(fragmentShader2);

    // <- check whether it is successful ->
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::Fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // <- Then we need Shader Program Object to link different shader ->
    unsigned int shaderProgram = glCreateProgram(), shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram);
    glLinkProgram(shaderProgram2);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }
    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(fragmentShader2);

    /*
     *  we have to mannully specify our input data 'layout' in glVertexAttribPointer
     *  [0]: the location in vertex shader
     *  [1]: size of vertex, here we use (x, y, z) so it is 3
     *  [2]: data type
     *  [3]: whether normalize data to [0, 1] or [-1, 1]
     *  [4]: stride of vertex
     *  [5]: is in void* type, the offset of location
     *
     *  And we use different VAO to bind different vertex data and attrib.
     *  VAO contains pointers to VBO data.
     *
     *  Look at learnopengl-cn.github.io/01 Getting started/04 Hello Triangle/
     * */
    float vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // the location
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // DO NOT UNBIND EBO
    glBindVertexArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Line mode, not fill mode

    // <- render loop ->
    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements: mode, vertex number, type, offset of EBO
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgram2);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(3 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if(mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.01f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}