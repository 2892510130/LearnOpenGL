#pragma once

#include <map>
#include <string>

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

class FontScene : public Scene {

public:
    const char* getName() const override { return "Font Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

private:
    Shader m_shader;
    std::map<GLchar, Character> m_Characters;

    unsigned int m_VAO = 0, m_VBO = 0;
};