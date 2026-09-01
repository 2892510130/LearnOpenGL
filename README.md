## Important Things
- glBindVertexArray must after the glUseProgram, because glUseProgram will tell 
OpenGL Pipeline from now on, all the draw are using this program  
- Remember that vertex shader works on one vertex, and fragment shader works on one pixel.

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
2. What the glViewport do?
   1. NDC [-1, 1] -> Viewport -> Screen, consider a object that is sizeof 0.2x0.2 in NDC, and viewport is 800x600, than its size is 160x120.
   2. When we resize the window with glViewport to 1000*1000, then the size of the object is 200x200


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