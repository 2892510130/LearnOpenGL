## PBR (physically based rendering)
- For a PBR lighting model to be considered physically based, it has to satisfy the following 3 conditions
    - Be based on the microfacet surface model.
    - Be energy conserving.
    - Use a physically based BRDF.
- The ***microfacet*** model: any surface at a microscopic scale can be described by tiny little perfectly reflective mirrors called microfacets
    - Light scatters on rougher surface, and align on smoother surface
    - We define a **roughness** parameter approximate the surface's microfacet roughness. Then we can calculate the ratio of microfacets roughly aligned to **halfway vector** $h$, where
        - $$h = \frac{l + v}{\|l + v\|}$$
        - $l$ stands for light vector, $v$ stands for view vector
- ***Energy conservation***: outgoing light energy should never exceed the incoming light energy (excluding emissive surfaces)
    - A light hit the surface than split into two parts: reflection (specular light) and refraction (diffuse light)
        - they are mutually exclusive
    - In PBR we assume all refraction light are absorbed (subsurface scattering technique consider it, it is another topic)
        - This assumption is just for dielectrics, because for metallic this is not assumption, it is its property
- The ***reflectance equation***: one of the render equation (currently the best model we have for simulating the visuals of light)
    - $$L_o(p,\omega_o) = \int\limits_{\Omega} f_r(p,\omega_i,\omega_o) L_i(p,\omega_i) n \cdot \omega_i d\omega_i$$
    - To understant this we need to understand some **radiometry**, for reflectance model, we just need to know **radiance**, which is used to quantify the magnitude or strength of light coming from a single direction. We denoted it as $L$.
        - **Radiant flux** $\Phi$ is the transmitted energy of a light source measured in Watts, can be considered as a function of all its different wavelengths. And in practice, we represent them in RGB (the light color) instead of wavelengths.
        - **Solide angle** $\omega$ is the angle you see from the origin point of a sphere a surface takes. It is the generalization of rad in 3D space.
        - **Rradiant intensity** measures the amount of radiant flux per solid angle, or the strength of a light source over a projected area onto the unit sphere.
            - $$I = \frac{d \phi}{d \omega}$$
        - **Irradiance** measures the amount of radiant flux per area
            - $$E = \frac{d \phi}{d A}$$
        - Radiance measures the amount of radiant flux $\Phi$ per unit projected area ($A \cos \theta$) per solide angle $\omega$
            - $$L=\frac{d^2\Phi}{ dA d\omega \cos\theta}$$
            - Look at https://learnopengl.com/img/pbr/radiance.png for visualization.
        - If this angle is small enough than we can consider it as a single ray of light on a fragment (if area is small enough)
    - Now we can understand the equation, if we don't consider $f_r$ it is just irradiance of point p!
        - $$
            \begin{aligned}
                \int\limits_{\Omega} L_i(p, \omega_i) n \cdot \omega_i d\omega_i
                &= \int\limits_{\Omega} L_i(p, \omega_i) \cos \theta d\omega_i \\
                &= \int\limits_{\Omega} \frac{d^2\Phi}{ dA d\omega_i \cos\theta} \cos \theta d\omega_i
                = \frac{d \phi}{d A}
            \end{aligned}
          $$
    - We use **Riemann sum** to numerically compute this equation.
- ***BRDF*** (bidirectional reflective distribution function)
    - All most all real time BRFD use **Cook-Torrance BRDF**
        - $$f_r = k_d f_{lambert} +  k_s f_{cook-torrance}$$
        - $k_s$ being the ratio that gets reflected and $k_d$ being the ratio that gets refracted
        - The left side is diffuse part known as **Lambertian diffuse**, which $f_{lambert} = c / \pi$, $\pi$ is for normalization, $c$ being the albedo or surface color.
        - The specular part is more advanced
            - $$f_{CookTorrance} = \frac{DFG}{4(\omega_o \cdot n)(\omega_i \cdot n)}$$
            - N stands for **Normal distribution function**: approximates the amount the surface's microfacets are aligned to the halfway vector, influenced by the roughness of the surface
            - G stands for **Geometry function**: describes the self-shadowing property of the microfacets. 
            - F stands for **Fresnel equation**: The Fresnel equation describes the ratio of light that gets reflected over the light that gets refracted at different surface angles.
            - Look at https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html for different functions of these.
    - For normal distribution function we use **Trowbridge-Reitz GGX**
        - $$NDF_{GGX TR}(n, h, \alpha) = \frac{\alpha^2}{\pi((n \cdot h)^2 (\alpha^2 - 1) + 1)^2}$$
        - where $\alpha$ is the roughness parameter
    - For geometry function we use **Schlick-GGX**
        - $$G_{SchlickGGX}(n, v, k) = \frac{n \cdot v} {(n \cdot v)(1 - k) + k }$$
        - Here $k$ is a remapping of $\alpha$ based on whether we're using the geometry function for either direct lighting or IBL lighting: 
            - $$k_{direct} = \frac{(\alpha + 1)^2}{8}, \quad k_{IBL} = \frac{\alpha^2}{2}$$
        - To effectively approximate the geometry we need to take account of both the **view direction (geometry obstruction)** and the **light direction vector (geometry shadowing)**. We can take both into account using **Smith's method**: 
            - $$G(n, v, l, k) = G_{sub}(n, v, k) G_{sub}(n, l, k)$$
    - For fresnel equation we use **Fresnel-Schlick** approximation
        - Every surface or material has a level of **base reflectivity** when looking straight at its surface, but when looking at the surface from an angle all reflections become more apparent compared to the surface's base reflectivity.
        - $$F_{Schlick}(h, v, F_0) = F_0 + (1 - F_0) ( 1 - (h \cdot v))^5$$
        - $F_0$ represents the base reflectivity of the surface, which we calculate using something called the **indices of refraction** or **IOR**. But this way only works for non-metal materials, so we pre-compute $F_0$ from the normal vectore angle (minus angle, angle 0). These pre-compute values can be found in http://refractiveindex.info/ or other database.
        - And also for unifying both metal and non-metal materials we introduce a parameter called **metalness**
        - ```cpp
            // first set to the defalut dielectric materials (0.04 is good enough)
            vec3 F0 = vec3(0.04);
            // then mix to get metallic materials, with the surface color because specualr is much more the diffuse
            F0 = mix(F0, surfaceColor.rgb, metalness);

            vec3 fresnelSchlick(float cosTheta, vec3 F0)
            {
                // cosTheta is angle between normal vector and view vector, power of 5 simulate the faster speed when angle increase
                return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
            }
            ```
    - The final equation:
        - $$
            L_o(p,\omega_o) = 
            \int\limits_{\Omega} (k_d\frac{c}{\pi} + \frac{DFG}{4(\omega_o \cdot n)(\omega_i \cdot n)}) 
            L_i(p,\omega_i) n \cdot \omega_i d\omega_i
            $$

## Diffuse irradiance
- Recall the final equation, we can seperate it into two part, the left one is for diffuse light (in this section we only consider this).
    - $$L_o(p,\omega_o) = k_d\frac{c}{\pi} \int\limits_{\Omega} L_i(p,\omega_i) n \cdot \omega_i  d\omega_i$$
- We need to think about how to get radiance given a direction, and how to compute the integration efficiently.
    - For the first question, we use cube map texture, each pixel of the texture seen as light source
    - For the second question, we sample from hemisphere at point $p$, this map is called **irradiance map**.
        - $p$ is fixed, so in practice, game engine will put multiple points.
- The .hdr format, RGB each with 1 byte (8 bits), and the E (exponential term) with 1 byte, total 32 bits (compare with RGB = 3 * 32 = 96bits).
    - with stb_image.h we use `stbi_loadf` load the 32bits data back to normal 96bits data.
    - environment map is projected from a sphere onto a flat plane such that we can more easily store the environment into a single image known as an **equirectangular map**.
    - We can directly sample from this map, but that needs some triangle math, which are expensive. So we first transform it into cube map.