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
- Right Vector:
  - `glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);` 
  - `glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));`
- Up axis (it is not upVector):
  - `glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);`
- Then we can use a matrix to represent a coordinate system:
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