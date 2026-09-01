## How the filter works?
- Two direction of filter the texture to fit the surface
	- GL_TEXTURE_MIN_FILTER (the texture is big, we need shrink it)
	- GL_TEXTURE_MAG_FILTER (the texture is small, we need stretch it)
- Five ways of filter, last 3 needs generate the minimaps (the texture is too large for first two methods)
	- GL_NEAREST, using the texture coordinate to find the closest pxiel.
	- GL_LINEAR, take mean of nearby 4 pixels
	- GL_LINEAR_MIPMAP_NEAREST, choose the closest minimap and use closest pixel
	- GL_LINEAR_MIPMAP_LINEAR, mean of two minimap, each minimap use mean of 4 pixels
	- GL_NEAREST_MIPMAP_NEAREST, all use the closest
	- GL_NEAREST_MIPMAP_LINEAR (not used normally so we do not introduce it, it is bad)

## How the wrap works?
- If the texture coordinate exceed the [0, 1], we need to know how to attach texture on the surface.
	- GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T and GL_TEXTURE_WRAP_R (the depth, for the 3D texture)
	- GL_REPEAT, just repeat, like bricks
	- GL_MIRRORED_REPEAT, mirrored it then repeat, the edge will be smooth
	- GL_CLAMP_TO_EDGE, set the exceed part as the edge
	- GL_CLAMP_TO_BORDER, give a color and set it to this color
		- glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		
## How to understand MRT and Defer Rending textures?
- Remember that vertex shader works on one vertex, and fragment shader works on one pixel.
- In the geometry pass, we render the information to the new buffer's textures.
	- In vertex shader, we get the gl_Postion that is the position of this vertex on the **viewport** (set by glViewport), then after rasterization, we get
fragment(x, y) -> viewport. Then we calculate the gPosition, gNormal and gColor..., it will store it into the corresponding texture position. Here we set
the texture size same as screen size, so it is the position on the screen.
	- Render all the object (with the depth check), we get a screen size of gPosition, gNormal and gColor
- Then in light pass, we just check these pixels, each pixel contains a position, a normal and a color, so we can calculate the light once for all.
