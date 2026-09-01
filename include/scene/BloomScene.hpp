#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

class BloomScene : public Scene {

public:
    const char* getName() const override { return "Bloom Map Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderCube();

private:
    Shader m_shader, m_light_shader, m_blur_shader, m_final_shader;
    Texture m_wood_texture, m_container_texture;

    glm::vec3 m_lightPos = glm::vec3(0.5f, 1.0f, 0.3f), m_cameraPos;
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;
    std::vector<glm::vec3> m_lightPositions, m_lightColors;
    
    unsigned int m_attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    unsigned int m_color_buffers[2];
    unsigned int m_pingpongFBO[2];
    unsigned int m_pingpongColorbuffers[2];
    unsigned int m_hdrFBO = 0, m_rboDepth = 0;
    unsigned int m_quadVAO = 0, m_quadVBO = 0;
    unsigned int m_cubeVAO = 0, m_cubeVBO = 0;

    float m_height_scale = 0.1f;
    float m_max_layer = 32.0f;

    // For the ImGui part
    int m_amount = 10;
    bool m_bloom = true;
    float m_exposure = 0.3;
};
