## Use `glGetError` 
- It only return the latest error, and clear other errors
    - But in distributed system they keep more then one, so in practice, use a `while` for it

## Debug Output
- OpenGL > 4.3 needed, below we need to load it with plugins
- `glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);` to enable it in debug (not in release as it is slow!)
- ```cpp
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE); 
    }
    ```
- We can also insert message to debuger
- ```cpp
    glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,                       
                     GL_DEBUG_SEVERITY_MEDIUM, -1, "error message here"); 
    ```

## Just output something
- output the shader's output, like `FragColor.rgb = Normal` to debug it
- output the framebuffer stuff, like framebuffer's color attachments

## OpenGL GLSL reference compiler
- can be download to check GLSL standard
    - .vert: vertex shader.
    - .frag: fragment shader.
    - .geom: geometry shader.
    - .tesc: tessellation control shader.
    - .tese: tessellation evaluation shader.
    - .comp: compute shader.
- If it outputs nothing, then there is no error

## Use 3rd party tools!
- RenderDoc, awesome!
- CodeXL
- NVIDIA Nsight