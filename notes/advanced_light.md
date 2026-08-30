## Advanced Light
- **Blinn-Phong** model
  - **Halfway Vector** is the vector between view and light, in this model we compute specular light using the normal vector and halfway vector, rather than view vector and light vector.
  - ```cpp
        vec3 lightDir   = normalize(lightPos - FragPos);
        vec3 viewDir    = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        vec3 specular = lightColor * spec;
    ```
  - We set the shininess 2X or 4X than Phong model.

## Gamma Correction
- The light human perceive is not linear in physic, we are not sencetive to birght color difference, but sencetive to dark difference.
  - So we encode the linear light into nonlinear one, so we have more bits for dark lights. Given $\gamma$, the color encoded is $V_{encode} = V_{in}^{1/\gamma}$, and the monitor will do the decode for us, $V_{decode} = V_{encode}^{\gamma}$. $\gamma$ is usually 2.2, but maybe different between monitors.
  - The encoded process is called gamma correction, we do it at the last color process step (not in the between).
  - `glEnable(GL_FRAMEBUFFER_SRGB);` will let OpenGL do it for us.
- Manully correction
  - `fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));` at the last step.
  - But some texture are stored in sRGB format, if we do the correction it will be too light. So we need to tell OpenGL to load the texture as sRGB format internally. Which is `glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);` or set the first format as `GL_SRGB_ALPHA` if we need alpha. This will do the encode for us automaticly.
  - We have to do the attenuation differently if we load it with sRGB format, it is now in linear mode after we read the texture so we do not soft 1/d^2 to 1/d, using 1/d^2 is much better. Before we use 1/d it looks better, not that dark. I can talk some math here:
    - We use RBG not sRGB, than the decay using $1/d$, then the monitor do the correction we get $1/d^{2.2} \approx 1/d^2$ which is the physic light decay.
    - If we use sRGB, $(1/d^2)^{1/2.2}$ is the processed color, the monitor to the correction will return exactly $1/d^2$.
- More informations:
  - https://blog.johnnovak.net/2016/09/21/what-every-coder-should-know-about-gamma/
  - https://www.wolfire.com/blog/2010/02/Gamma-correct-lighting/
  
## Shadow Mapping (directional shadow mapping)
- We can use the same technique in depth map, render from the light pos to get the depth, then check the depth to see whether it is in the shadow.
    - use `texture(depthMap, fragToLight)` get the closest depth and compare it with `length(fragToLight)` which is the current depth
- **Shadow Acne**: due to the resolution of the depth map, sometime we get this unreal shadow
    - See https://learnopengl-cn.github.io/img/05/03/01/shadow_mapping_acne_diagram.png
    - So we need a small offset to fix this, this offset bias can be written as `float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);`, and this two number will need you to set based on the effect
- **Peter Panning**: when using the offset, we modify the depth so maybe we will see some frag flying above the plane. Just see one corner of the cube.
    - We can use **front face culling** to fix it. See https://learnopengl-cn.github.io/img/05/03/01/shadow_mapping_culling.png.
    - This only works for things that have closed inner space like a cube, not works for plane.
- **Over Sample**: The oversampling issue causes areas outside the light's frustum to be incorrectly treated as shadowed
    - Set the wrap method as `GL_CLAP_BORDER` with a border color of 1.0
    - Force shadow = 0.0 when projCoords.z > 1.0 in the shadow calculation
- **PCF** (percentage-closer filtering): generate more soft shadow.
    - Sample the depth map (from nearby positions) multiple times, and get the mean of them.

## Point Shadows (omnidirectional shadow maps)
- Instead of a 2D texture we use a cube texture with 6 face to render the depth map of a point light.
- We can use a geometry shader:
    - ```cpp
        #version 330 core
        layout (triangles) in;
        layout (triangle_strip, max_vertices=18) out;

        uniform mat4 shadowMatrices[6];

        out vec4 FragPos; // FragPos from GS (output per emitvertex)

        void main()
        {
            for(int face = 0; face < 6; ++face)
            {
                gl_Layer = face; // built-in variable that specifies to which face we render.
                for(int i = 0; i < 3; ++i) // for each triangle vertex
                {
                    FragPos = gl_in[i].gl_Position;
                    gl_Position = shadowMatrices[face] * FragPos;
                    EmitVertex();
                }    
                EndPrimitive();
            }
        }
        ```
    - This will emit 6 points for a given point.
- **PCF**:
    - We can sample with samples = 4.0, like below, but it is too expensive
    - ```cpp
        for(float x = -offset; x < offset; x += offset / (samples * 0.5))
        {
            for(float y = -offset; y < offset; y += offset / (samples * 0.5))
            {
                for(float z = -offset; z < offset; z += offset / (samples * 0.5))
                {
                    float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
                    closestDepth *= far_plane;   // undo mapping [0;1]
                    if(currentDepth - bias > closestDepth)
                        shadow += 1.0;
                }
            }
        }
        shadow /= samples * samples * samples;
        ```
    - Instead we sample from 20 points like this:
    - ```cpp
        vec3 gridSamplingDisk[20] = vec3[]
        (
            vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
            vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
            vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
            vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
            vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
        );
        ```

## Normal Map
- Using per-fragment normals instead of per-surface normals. Also known as **Bump Mapping**.
    - Remember that OpenGL is left bottom the original point, so OpenGL will flip it, this will mess up the normal texture when loading
    - In the code, we read a pre-fliped picture but in general the picture is not fliped in advance.
- **Tagnent Space**
    - But we will not use the x, y, z in normal texture, because it relatives to the surface. 
    - So we need a coordinate system which is relative to the indivisual **triangles**. We called it **TBN** matrix, where T is Tagnent, B is Bitagnent, and N for Normal.
    - For visual understand look at https://learnopengl.com/img/advanced-lighting/normal_mapping_surface_edges.png.
    - What we have: three points of triangles $P_1, P_2, P_3$ and there texture coordinate $(u_1, v_1), (u_2, v_2), (u_3, v_3)$. T is the direction of $u$ changes, and B for $v$ changes so we have:
        - $$
            E_{12} = P_2 - P_1 = \Delta u_2 T + \Delta v_2 B, \quad \Delta u_2 = u_2 - u_1 \\
            E_{13} = P_3 - P_1 = \Delta u_3 T + \Delta v_3 B, \quad \Delta v_3 = v_3 - v_1
        $$
        - $$
            \begin{bmatrix} T_x & T_y & T_z \\ B_x & B_y & B_z \end{bmatrix} 
            = \frac{1}{\Delta U_1 \Delta V_2 - \Delta U_2 \Delta V_1} 
            \begin{bmatrix} \Delta V_2 & -\Delta V_1 \\ -\Delta U_2 & \Delta U_1 \end{bmatrix} 
            \begin{bmatrix} E_{1x} & E_{1y} & E_{1z} \\ E_{2x} & E_{2y} & E_{2z} \end{bmatrix}
        $$
        - Then the TBN matrix is [T, B, N], each one of the vec is column vec
    - There are two ways to use this TBN matrix
        - Use it in fragment shader, transform the normals to world space
        - Use it in vertex shader, using the inverse TBN (it is orthogonal so we just use **transpose**) to transform the lightDir, viewPos and fragPos to the tagnent space
        - Why the second way maybe better? Because fragment shader has more calculations to do, as vertex shader only works on vertex points
- Normal map is good for preformance boost, we can reduce a lot of vertices and still get good resolution.
- After model transform, T and N may not perpendicular, so we can use a trick called **Gram-Schmidt process** to re-orthogonalize it
    - `T = normalize(T - dot(T, N) * N);` then calculate B with `vec3 B = cross(N, T);`

## Parallax Mapping (视差贴图)
- One of the displacement mapping techniques that displace or offset vertices based on geometrical information stored inside a texture.
- So we also have a height map (or a depth map), we have fragPos $A$ that is on the viewDir $\bar V$, then we scale it with $\bar P = H(A) \bar V$ (or negative for depth map) to get $H(P)$, see at https://learnopengl.com/img/advanced-lighting/parallax_mapping_depth.png.
    - ```cpp
        vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
        { 
            float height =  texture(depthMap, texCoords).r;    
            vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
            return texCoords - p;    
        }
        ```
    - We can also cancle this division of z to make it more stable, the division will make it more real from a large angle, but may also bring some torque
- **Steep Parallax Mapping**: we sample multiple times
    - We divide $n$ samples of height (depth) uniformly from 0.0 to 1.0, then we walk through the viewDir from top to bottom, compare the layer height and the cross point height, until the cross point height higher than the layer height.
    - Then we lerp it with last cross point height to get the final depth (this is called **Parallax Occlusion Mapping**)

## HDR (high dynamic range)
- Allow fragment color temporarily exceed 1.0. We allow for a much larger range of color values to render to, collecting a large range of dark and bright details of a scene, and at the end we transform all the HDR values back to the low dynamic range (LDR) of [0.0, 1.0].
    - **tone mapping**: HDR -> LDR
    - When the internal format of a framebuffer's color buffer is specified as `GL_RGB16F`, `GL_RGBA16F`, `GL_RGB32F`, or `GL_RGBA32F` the framebuffer is known as a floating point framebuffer that can store floating point values outside the default range of 0.0 and 1.0. 








