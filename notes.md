## Important Things
- GLM matrix are mat[col][row], first is column not row
- First bind the VAO then bind the VBO
- glfwPollEvents will checkout the callback array, which is updated by operating system

## Overall Notes
- OpenGL is a huge State Machine, its states are called **OpenGL Context**, We will generate, bind, set and unbind a **Object**.
- **GLFW**: create window, handle input cross platform
- **GLAD**: manage OpenGL function pointers

## VAO, VBO and EBO
- VAO, Vertex Array Object; VBO, Vertex Buffer Object; 
- EBO, Element Buffer Object; Or, EBO, Element Buffer Object.
- Graphics Pipeline will run through a set of functions to take the 3D
coordination into 2D pixels. These functions can be parallized so GPU
can handle them easily. These tiny functions are called **Shader**. They
are written in OpenGL Shading Language, GLSL.
- These different stages are:
Vertex Data -> Vertex Shader -> Geometry Shader -> Shape Assembly ->
Rasterization -> Fragment Shader -> Tests and Blending
- Vertex Data are collections of Vertex, each one is represented with
**Vertex Attribute**. The position are in **Normalized Device Coordinates, NDC**
form. (x, y, z) are in region [-1, 1]. glViewport will do the viewport transform
NDC -> Screen-space Coordinates.
- Shader will create memory to save vertex data, which is VBO. We can transfer a lot
of data from CPU to GPU, then it will be fast later.
VBO has its own unique ID, and its type is GL_ARRAY_BUFFER.
- glGenBuffers(GLsizei n, GLuint* ids), n is the number of buffers.
glBindBuffer(GLenum target, GLuint id), target is the type
glBufferData(GLenum target, GLsizei size, const void* data, GLenum usage),
usage are how we want GPU to handle the given data:
    1. GL_STATIC_DRAW: data will not change or rarely change
    2. GL_DYNAMIC_DRAW: data will change a lot
    3. GL_STREAM_DRAW: data will change every time

## GLSL language
- glsl data type: int, float, double, uint, bool and vector and matrix
- vecn (float), bvecn, ivecn, uvecn, dvecn where n is the number. we can swizzling (重组) them like:
    -  vec2 a; vec4 b = a.xyxx;
-  Uniform: global and same for all shader

## Texture
- Texture coordinates are in (x, y) -> (0, 1), where (0, 0) is bottom left. 
- if it is out of (0, 1) then it will repeate with (wrap):
    - GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
- We have Texture Pixel which are constrained by resolution, but texture coordinate
are not constrained by resolution, so we have to find a way to get Texel -> coord,
this is called Texture Filtering.
    - Two common ways: GL_NEAREST and GL_LINEAR, see https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/.
- Mipmap (多级渐远纹理) ： it is like LOD but for texture. Between different level of textures we can also filter it.
- We can use Texture Unit to activate more than one textures at the same time (in one shader program).

## Coordinate System
1. Local Space (Object Space) | For example, you create a model in blender
2. World Space                | Using [Model Matrix] to transfer from Local Space
3. View Space (Eye Space)     | Using [View Matrix] to transfer from View Space
4. Clip Space                 | [Orthographic Projection Matrix] or [Perspective Projection Matrix]
    1. See https://learnopengl-cn.github.io/img/01/08/coordinate_systems.png.
[Orthographic Projection Matrix] -> glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f)
    2. https://learnopengl-cn.github.io/img/01/08/orthographic_frustum.png.
[Perspective Projection Matrix] -> glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f),
first one is FOV.
5. Screen Space
6. We need to use Z-buffer to tell OpenGL to do the depth check
   
## Camera System
- Right Vector or Right axis (first we define a up Vector):
  - `glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);` 
  - `glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));`
- Up axis (it is not upVector):
  - `glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);`
- Then we can use a matrix to represent a coordinate system: (suppose we look at, a matrix for a coordinate system), it is rightVector, upAxis(I think) and direction
  - $LookAt = \begin{bmatrix}
               R_x & R_y & R_z & 0 \\ 
               U_x & U_y & U_z & 0 \\ 
               D_x & D_y & D_z & 0 \\
               0   & 0   & 0   & 0
\end{bmatrix} \begin{bmatrix}
               1 & 0 & 0 & -P_x \\
               0 & 1 & 0 & -P_y \\
               0 & 0 & 1 & -P_z \\
               0 & 0 & 0 & 1 \\
\end{bmatrix}$ is the math, R for rightVector, U for upVector and D for directionVector. P for position.
  - `view = glm::lookAt(pos, target, upVector)` is the code

## Light System
- **Phong Lighting Model**: https://learnopengl-cn.github.io/img/02/02/basic_lighting_phong.png.
  - **Ambient Lighting**: enviroment light, just times a little number.
  - **Diffuse Lighting**: direction effect of the light source. More direct to the light, more light. We need the normal vector of the object, and the light vector, we can calculate dot product.
    - We need to get the light vector, which is `lightPos - fragPos`, both in the world coordinate
    - Do we need to put normal vector in the world coordinate? Yes, but we need to use **Normal Matrix**, a special matrix, because if we use normal matrix then the vector will not be perpendicular to the plain. `Normal = mat3(transpose(inverse(model))) * aNormal;` but remember we should not do it in shader, we should it do it in CPU prehead!
  - **Specular Lighting**: light spot. If we view it right at the reflection line, it will be most light.
- If we do the light in the vertex shader rather than the fragment shader, we call it **Gouraud Shading**, which is blur. Because in this way the light is calculate on vertex then filter between. In Phong model, the normal and pos are filtered but the color is not.
- More people do it in view space rather than world space. See bottom.

## The Pipeline of OpenGL
1. The Shader (Vertex and Fragment)
```
Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. read shader files
    std::string vertexCode = readShaderFile(vertexPath);
    std::string fragmentCode = readShaderFile(fragmentPath);
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Then when we use this program
glUseProgram();
```
2. VAO (the pointer), VBO and EBO
```
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

// Then when we use this VAO
glBindVertexArray(VAO);

// After the while loop
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteBuffers(1, &EBO);
```
3. Texture
```
unsigned int texture1;
glGenTextures(1, &texture1);
glBindTexture(GL_TEXTURE_2D, texture1);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE GL_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

const char* texture_path1 = (std::string(ROOT_PATH) + "\\res\\textures\\container.jpg").c_str();
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

// In the while loop, because GL_TEXTURE are limited
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
```

## Additional Code
The light system in view space:
```
// Vertex shader:
// ================
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;

uniform vec3 lightPos; // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    LightPos = vec3(view * vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
}


// Fragment shader:
// ================
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;   // extra in variable, since we need the light position in view space we calculate this in the vertex shader

uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
```