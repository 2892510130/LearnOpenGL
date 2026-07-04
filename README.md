## Important Things
- glBindVertexArray must after the glUseProgram, because glUseProgram will tell 
OpenGL Pipeline from now on, all the draw are using this program  

## Critic Points to better Understanding

The pipeline: https://learnopengl-cn.github.io/img/01/04/pipeline.png.  
The vertex attribute: https://learnopengl-cn.github.io/img/01/04/vertex_attribute_pointer.png.  
The relation of VAO, VBO, EBO: https://learnopengl-cn.github.io/img/01/04/vertex_array_objects_ebo.png.  
Texture repeate: https://learnopengl-cn.github.io/img/01/06/texture_wrapping.png.  
Coordinate System: https://learnopengl-cn.github.io/img/01/08/coordinate_systems.png.

## Additional Materials
1. The projection: https://www.songho.ca/opengl/gl_projectionmatrix.html  

## Questions
1. Why gl_Position is (x, y, z, w)?
   Becuase we need represent infinite, w -> 1.0 if a point, w -> 0.0 is a direction/vector.
   The real positon is (x/w, y/w, z/w).


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

## The Pipeline
CPU 准备数据  
    ↓  
[VAO/VBO/EBO] → Vertex Shader (顶点位置变换)  
    ↓  
[Optional: Tessellation / Geometry Shader] (曲面细分/几何着色器，高级话题)  
    ↓  
[裁剪 (Clip) + 视口变换 (Viewport)]  
    ↓  
[光栅化 (Rasterization)] → 生成片元  
    ↓  
[Fragment Shader] (纹理采样、光照计算)  
    ↓  
[Scissor Test] (可选裁剪)  
    ↓  
[Stencil Test] (模板测试)  
    ↓  
[Depth Test] (深度测试)  
    ↓  
[Blending] (混合/透明度)  
    ↓  
[FrameBuffer] (写入 FBO 或 屏幕)  
    ↓  
[MSAA 解析] (如果开启了多重采样)  
    ↓  
屏幕显示