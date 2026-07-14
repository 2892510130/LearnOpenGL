## Depth Testing

- We use a **Depth Buffer** to compare z value of a object. If it successes, then we keep it, if not we ignore it.
  - The comparision is after the fragment shader in the window space, we can access it with `gl_FragCoord`.
  - Some GPU support early depth testing, but it makes it unable to write a depth value in fragment shader.
  - `glEnable(GL_DEPTH_TEST);` to enable it than `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);` to update it every frame.
  - `glDepthMask(GL_FALSE);` will disable the update of the depth buffer.
- `glDepthFunc` can change the way we compare the depth value, by default it is `GL_LESS`.
- The depth buffer value is in [0, 1] so we need to project the z value into this range.
  - We can use a linear depth buffer: $F = (z - near) / (far - near)$, where $far$ and $near$ is the value of the projection matrix.
  - Though it is accurate, it is not good, because we don't want the same detail for the far object as the near one. So instead we use:
  - $$F = \frac{1/z - 1/near}{1/far - 1/near}$$
  - It is inside the projection matrix, details in http://www.songho.ca/opengl/gl_projectionmatrix.html.
- **Depth Conflict**
  - When two face are close, the resolution of the depth value (typically 24 bits) are not enough so we can see two face of two object changes fo quickly.
  - In order to handle this problem (it is not possible to erase all of it):
    - Don't put two object close (but in the far place the resolution is too small so)
    - Put the **near** plain far (but too far the near object will be cliped)
    - Use higher resolution like 32 bits (lower the performance)

## Stencil Test

- Just like depth test, it is also rely on a **Stencil Buffer**. The test is after fragment is handled, is before the depth test.
  - It is 8 bits, so it has 256 choices of value.
  - `glEnable(GL_STENCIL_TEST);` to enable the test
  - `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);` to update it
  - `glStencilMask` function to write mask, the stencil value will do AND with this mask.
  - like `glDepthFunc` we have two:
    - `glStencilFunc(GLenum func, GLint ref, GLuint mask)`, for example `glStencilFunc(GL_EQUAL, 1, 0xFF)` tells openGL when `(1 & 0xFF) == (stencilValue & 0xFF)` keep the fragment
    - `glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)`: 0 -> when it fail, 1  -> when it success and depth test fail, 2 -> when both fail. We usually set them all as `GL_KEEP`.

## Blending
- Read the texture using RGBA format, and in shader we discard when `FragColor.a < someValue` (this is for total transparent and non-transparent)
- Remember to set `GL_TEXTURE_WRAP_S` and `GL_TEXTURE_WRAP_T` to `GL_CLAMP_TO_EDGE`.
- `glEnable(GL_BLEND);` to enable blending, the blending function is:
  - $$C_{result} = C_{source} * F_{source} + C_{destination} * F_{destination}$$
  - The source is the object/face on, the destination is below
  - `glBlendFunc(GLenum sfactor, GLenum dfactor)` to set this two F value
    - `glBlendColor` set $C_{constant}$ in this func
    - `glBlendEquation` set the operation, by default it is add
- Pay attention when you do both depth test and blending (you can see in the code exe):
  - something can be ignored when it is after some transparent object
  - we can handle this problem by first sorting the transparent object, then draw the transparent, draw other thing last.

## Face Culling
Every face, every triangle has its rotation based on the vertice order, so we can use it to check whether a face should be culled or not.

## Frame Buffer
- We have introduced a lot of frame buffer before, like color buffer, depth buffer and scentil buffer...
- But we can also define our own frame buffer to get additional target.
  - Like other buffer, we can use `glGenFramebuffers` to generate a new buffer.
  - `glBindFramebuffer(GL_FRAMEBUFFER, ID);` to bind
- A **complete** frame buffer neeeds:
  - A buffer (like color buffer, depth buffer and scentil buffer)
  - A color **attachment** and they should be complete (keep memory)
  - Every buffer have the same **sample**
  - `if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)` to check it
  - Bind the buffer back to 0 will get to the default frame buffer
- Attachment can be texture or **Renderbuffer Object**
  - For texture attachment, we can generate a texture with NULL data.
    - If we want to render with little or larger texture to window, we need to use `glViewport`
    - `glFramebufferTexture2D(target, attachment, textarget, texture, level);` put texture to frame buffer
    - We can put a color texture to it, or depth/scentil. 
    - We can aslo put a depth_scentil texture to it:
    - ```glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, 
        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
        );
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
  - For renderbuffer object
    - It is saved in native OpenGL render format, optimized for **off-screen** render.
    - It is usually write-only and it is good for depth and scentil buffer because we usually do not sample from it.
    - `glGenRenderbuffers` to generate it, and `glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);` to build the object.
- So we can generate a frame buffer, add a texture attachment as color buffer, add a render buffer object as depth and scentil buffer. Then we draw the color buffer attachment at default frame buffer.
  - So we can then in the default frame buffer, use the screen shader, do some post process.
  - For the kernel post process, we can set all the wrap parameters as clamp to edge, because we need to sample from pixel from neighbor pixel.
  
## Cube Maps
- It is basically 6 image as textures to six faces, and we can use a direction to sample from these textures
    - `glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);` to bind it. `glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);` wrap between textures.
    - ```
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
- And in GLSL shader, we use a `samplerCube`, and use `texture` same as normal textures.
- `glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));` to remove translation so we can use it as a **Skybox**.
  - Optimization: in shader, we can set the pos z as w, so we get z / w = 1.0, the depth processed will always be 1.0, and depth test will always ignore it. Draw this skybox at last will save us a lot of time.
- **Enviroment Mapping**: we can use skybox as textures, reflect it or refract it for a mirror like object.
  - refract: `refract(I, normalize(Normal), ratio);`
  - reflect: `reflect(I, normalize(Normal));`
  - Then `FragColor = vec4(texture(skybox, R).rgb, 1.0);`.

## Advanced Data
- `glBufferSubData`, fill certain part of the buffer, must after `glBufferData`.
  - `glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data); // 范围： [24, 24 + sizeof(data)]`
  - We can use this method to get vertex attribute seperatly, though we recommend the interleave way.
- We can also use `glMapBuffer`:
    - ```cpp
        void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        // 复制数据到内存
        memcpy(ptr, data, sizeof(data));
        // 记得告诉OpenGL我们不再需要这个指针了
        glUnmapBuffer(GL_ARRAY_BUFFER);
        ```
- Copy buffer data to another buffer (different type):
    - ```cpp
        void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset,
                            GLintptr writeoffset, GLsizeiptr size);
        ```
    - For the same type we can use other type:
    - ```cpp
        glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
        glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));
        ```

## Advanced GLSL
- **Built-in Variable**
  - In vertex shader
    - `gl_PointSize` to set the point size, need `glEnable(GL_PROGRAM_POINT_SIZE);`.
    - `gl_VertexID` is read-only, for `glDrawElements` it save index, for `glDrawArrays` it save total number.
  - In fragment shader
    - `gl_FragCoord` we are familiar with
    - `gl_FrontFacing` check whether this face is front face or back face
    - `gl_FragDepth` write the depth, but will disable the depth check
      - But after OpenGL 4.2+ we can enable depth check while we write `gl_FragDepth` if we can make sure we are certain to write a bigger depth or less depth.
      - `layout (depth_<condition>) out float gl_FragDepth;`
- **Interface Block**, looks like struct but with in and out.
- **Uniform Buffer Object**
  - Same uniform structer across different shader. The struct layout are default **shared**, which means OpenGL can decide how to layout the structer data in memory, and it makes us hard to get the offset.
  - So we have **std140** layout, let us have a clear offset of the data, every data has its **Base Alignment**, so we can compute its **Aligned Offset**. $N$ represent 4 bytes. We also have `packed` layout, its layout may be different across different shaders.
    - For scalar, it is $N$.
    - For vec, it is $2N$ or $4N$, so vec3 is $4N$.
    - For array, matrices, its elements are $4N$.
  - ```cpp
        layout (std140) uniform ExampleBlock
        {
                             // 基准对齐量       // 对齐偏移量
            float value;     // 4               // 0 
            vec3 vector;     // 16              // 16  (必须是16的倍数，所以 4->16)
            mat4 matrix;     // 16              // 32  (列 0)
                             // 16              // 48  (列 1)
                             // 16              // 64  (列 2)
                             // 16              // 80  (列 3)
            float values[3]; // 16              // 96  (values[0])
                             // 16              // 112 (values[1])
                             // 16              // 128 (values[2])
            bool boolean;    // 4               // 144
            int integer;     // 4               // 148
        }; 
    ```
  - How can we bind the unifrom buffer object to the uniform block? Using **Binding Point**.
    - Look at https://learnopengl-cn.github.io/img/04/08/advanced_glsl_binding_points.png. The block is in shader, and buffer object is in our code.
    - ```cpp
        // Binding block to binding point
        unsigned int lights_index = glGetUniformBlockIndex(shaderA.ID, "Lights");   
        glUniformBlockBinding(shaderA.ID, lights_index, 2);
        ```
    - In OpenGL 4.2+, we can directly set it in shader like: `layout(std140, binding = 2) uniform Lights { ... };`.
    - ```cpp
        // Binding the buffer object to binding point
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock); 
        // or, the offset must be multiplier of GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
        glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboExampleBlock, 0, 152);

        // set the data
        glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
        int b = true; // GLSL中的bool是4字节的，所以我们将它存为一个integer
        glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b); 
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        ```

## Geometry Shader
- It is optional, and it is between vertex shader and fragment shader. We can specify in or out for the data.
  - For in, we have `points`, `lines`, `lines_adjacency`, `triangles`, `triangles_adjacency`
  - For out, we have `points`, `line_strip`, `triangle_strip`, it will decide how many vertices we have, for example triangle_strip is 3.
  - We can use `EmitVertex` and `EndPrimitive` to generate new vertex based on vertex data from vertex shader.
  - ```cpp
        in gl_PerVertex {
            vec4 gl_Position;
            float gl_PointSize;
            float gl_ClipDistance[];
        } gl_in[];
    ```
  - Just remember that the type in in and out is important, this will decide how we handle the input vertex and how to output it.

## Instancing
- Reduce the times we call the function, render instance by one call. Using `glDrawArraysInstanced` and `glDrawElementsInstanced`. Then use `gl_InstanceID` to distinguish between them.
- `Instanced Array` are used because uniform has a limit in numbers we can use, like `uniform vec3 box[101]` maybe too large.
- `glVertexAttribDivisor(2, 1);` the second term is **Attribute Divisor**, it sets to 1 to tell openGL this is an instanced vertex attribute (update vertex attribute when render new instance). If it sets to 0, then it will update the vertex attribute every frame.

## Anti Aliasing
- In order to get rid of **Jagged Edges** problem. We have **Super Sample Anti-aliasing** but it is expensive, so we have more effcient technique like **Multisample Anti-aliasing**.
  - It samples from 4 points (inside a pixel), or more.
  - Only do depth/scentil on all sub points, and do color compute only once. 
  - If we find at least one sub point is inside the triangle, then we get the color as triangle color. And in the end we compute the finnal color as [N_outside * background_color (in the color buffer) + N_inside * triangle_color] / N.
- We can set `glfwWindowHint(GLFW_SAMPLES, 4);` to tell GLFW, and best call `glEnable(GL_MULTISAMPLE);`.
- **Off-screen MSAA**
  - Just like what we talk in frame buffer section. Read the code.