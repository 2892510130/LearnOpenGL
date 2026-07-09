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