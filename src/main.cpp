#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "shader.hpp"
/*
 *  OpenGL is a huge State Machine, its states are called *OpenGL Context*.
 *  We will generate, bind, set and unbind a *Object*.
 *
 *  GLFW -> create window, handle input cross platform
 *  GLAD -> manage OpenGL function pointers
 * */


/*
 *                             Test 1
 *  Hello, Triangle
 *  *VAO, Vertex Array Object*
 *  *VBO, Vertex Buffer Object*
 *  *EBO, Element Buffer Object* or *IBO, Index Buffer Object*
 * 
 * ----------------------------------------------------------
 *  Graphics Pipeline will run through a set of functions to take the 3D
 *  coordination into 2D pixels. These functions can be parallized so GPU 
 *  can handle them easily. These tiny functions are called *Shader*. They
 *  are written in OpenGL Shading Language, GLSL.
 *
 *  These different stages are:
 *  Vertex Data -> Vertex Shader -> Geometry Shader -> Shape Assembly ->
 *  Rasterization -> Fragment Shader -> Tests and Blending
 *  ----------------------------------------------------------
 *
 *  Vertex Data are collections of Vertex, each one is represented with 
 *  *Vertex Attribute*. The position are in *Normalized Device Coordinates, NDC*
 *  form. (x, y, z) are in region [-1, 1]. glViewport will do the viewport transform
 *  NDC -> Screen-space Coordinates.
 *  Shader will create memory to save vertex data, which is VBO. We can transfer a lot
 *  of data from CPU to GPU, then it will be fast later. 
 *  VBO has its own unique ID, and its type is GL_ARRAY_BUFFER.
 *  <--> we can bind a bunch of buffer at the same time, as long as they have differnet type.
 *      glGenBuffers(GLsizei n, GLuint* ids), n is the number of buffers.
 *      glBindBuffer(GLenum target, GLuint id), target is the type
 *      glBufferData(GLenum target, GLsizei size, const void* data, GLenum usage), 
 *        usage are how we want GPU to handle the given data:
 *        1. GL_STATIC_DRAW: data will not change or rarely change
 *        2. GL_DYNAMIC_DRAW: data will change a lot
 *        3. GL_STREAM_DRAW: data will change every time
 * 
 *  we have to mannully specify our input data 'layout' in glVertexAttribPointer
 *  [0]: the location in vertex shader
 *  [1]: size of vertex, here we use (x, y, z) so it is 3
 *  [2]: data type
 *  [3]: whether normalize data to [0, 1] or [-1, 1]
 *  [4]: stride of vertex
 *  [5]: is in void* type, the offset of location
 *  And we use different VAO to bind different vertex data and attrib.
 *  VAO contains pointers to VBO data.
 * 
 * ------------------------------------------------------------------------------
 *                              Test 2
 *  glsl data type: int, float, double, uint, bool and vector and matrix
 *  vecn (float), bvecn, ivecn, uvecn, dvecn where n is the number
 *  we can swizzling 重组 them like:
 *      vec2 a; vec4 b = a.xyxx;
 *  Uniform: global and same for all shader
 * 
 * ---------------------------------------------------------------------------------
 *                              Texture Test
 *  Texture coordinates are in (x, y) -> (0, 1), where (0, 0) is bottom left
 *  if it is out of (0, 1) then it will repeate with (wrap):
 *  GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
 * 
 *  We have Texture Pixel which are constrained by resolution, but texture coordinate
 *  are not constrained by resolution, so we have to find a way to get Texel -> coord,
 *  this is called Texture Filtering.
 *    Two common ways: GL_NEAREST and GL_LINEAR, see https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/.
 * 
 *  Mipmap 多级渐远纹理 ： it is like LOD but for texture.
 *  Between different level of textures we can also filter it.
 *  We can use Texture Unit to activate more than one textures at the same time (in one shader program).
 * */


void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void test1(GLFWwindow* window);
void test2(GLFWwindow* window);
void texture_test(GLFWwindow* window);

float mixValue = 0.2f;

int main(int argc, char** argv)
{
    std::cout << "Type to check out different tests:\n"
        << "[0]: render two triangles with different fragment shaders\n"
        << "[1]: glsl learning\n"
        << "[2]: texture test\n";

    int task_select_index;
    std::cin >> task_select_index;

    std::vector<std::function<void(GLFWwindow* window)>> function_collection;
    function_collection.push_back(test1);
    function_collection.push_back(test2);
    function_collection.push_back(texture_test);

    // <- initialize GLFW ->
    glfwInit(); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Set as core profile

    const unsigned int windowHeight = 600, windowWidth = 800;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

    // <- initialize glad ->
    if (!gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress ))
    {
        std::cout << "Failed to initialize GLAD:\n";
        return -1;
    }

    if (task_select_index >= 0 && task_select_index < function_collection.size())
    {
        function_collection[task_select_index](window);
    }

    glfwTerminate();

    return 0;
}

void texture_test(GLFWwindow* window)
{
    const char* vertex_shader_path = "C:\\Users\\pc\\Desktop\\Personal\\Code\\LearnOpenGL\\res\\shaders\\vertexTexture.shader";
    const char* fragment_shader_path = "C:\\Users\\pc\\Desktop\\Personal\\Code\\LearnOpenGL\\res\\shaders\\fragmentTexture.shader";
    Shader ourShader(vertex_shader_path, fragment_shader_path);

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

    const char* texture_path1 = "C:\\Users\\pc\\Desktop\\Personal\\Code\\LearnOpenGL\\res\\textures\\container.jpg";
    int width, height, numberChannels;
    unsigned char* data = stbi_load(texture_path1, &width, &height, &numberChannels, 0);

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

    const char* texture_path2 = "C:\\Users\\pc\\Desktop\\Personal\\Code\\LearnOpenGL\\res\\textures\\awesomeface.png";
    data = stbi_load(texture_path2, &width, &height, &numberChannels, 0);

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

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        process_input(window);
        float mixPercentage;

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // render container
        ourShader.use();
        ourShader.setFloat("mixPercentage", mixValue);
        glBindVertexArray(VAO);
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

    const char* vertex_shader_path = "C:\\Users\\pc\\Desktop\\Personal\\Code\\LearnOpenGL\\res\\shaders\\vertex.shader";
    const char* fragment_shader_path = "C:\\Users\\pc\\Desktop\\Personal\\Code\\LearnOpenGL\\res\\shaders\\fragment.shader";
    
    Shader shader_source(vertex_shader_path, fragment_shader_path);

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
}
