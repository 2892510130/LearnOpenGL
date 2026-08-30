#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

class NormalMapScene : public Scene {

public:
    const char* getName() const override { return "Normal Map Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderBaseQuad();

private:
    Shader m_shader, m_base_shader;
    Texture m_diffuse_texture, m_normal_texture;

    glm::vec3 m_lightPos = glm::vec3(0.5f, 1.0f, 0.3f), m_cameraPos;
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;

    unsigned int m_quadVAO = 0, m_quadVBO = 0;
    unsigned int m_baseQuadVAO = 0, m_baseQuadVBO = 0;

    // For the ImGui part
    bool m_enable_TBN = false;
};
