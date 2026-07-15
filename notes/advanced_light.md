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
  
## Shadow Mapping
- We can use the same technique in depth map, render from the light pos, then check the depth to see whether it is in the shadow.