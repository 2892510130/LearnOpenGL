## Important Things
- glBindVertexArray must after the useProgram

### Critic Points to better Understanding

The pipeline: https://learnopengl-cn.github.io/img/01/04/pipeline.png.
The vertex attribute: https://learnopengl-cn.github.io/img/01/04/vertex_attribute_pointer.png.
The relation of VAO, VBO, EBO: https://learnopengl-cn.github.io/img/01/04/vertex_array_objects_ebo.png.
Texture repeate: https://learnopengl-cn.github.io/img/01/06/texture_wrapping.png.

### Questions
1. Why gl_Position is (x, y, z, w)?
   Becuase we need represent infinite, w -> 1.0 if a point, w -> 0.0 is a direction/vector.
   The real positon is (x/w, y/w, z/w).