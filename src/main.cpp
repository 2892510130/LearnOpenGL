#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
/*
 *  OpenGL is a huge State Machine, its states are called *OpenGL Context*.
 *  We will generate, bind, set and unbind a *Object*.
 *
 *  GLFW -> create window, handle input cross platform
 *  GLAD -> manage OpenGL function pointers
 * */


/*
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
 * */


void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

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


int main()
{
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

    glfwTerminate();

    return 0;
}

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}
