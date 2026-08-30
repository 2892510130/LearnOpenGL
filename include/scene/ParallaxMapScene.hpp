#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

class ParallaxMapScene : public Scene {

public:
    const char* getName() const override { return "Parallax Map Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderBaseQuad();

private:
    Shader m_shader, m_base_shader;
    Texture m_diffuse_texture, m_normal_texture, m_depth_texture;    // for brick
    Texture m_diffuse_texture2, m_normal_texture2, m_depth_texture2; // for toy box

    glm::vec3 m_lightPos = glm::vec3(0.5f, 1.0f, 0.3f), m_cameraPos;
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;

    unsigned int m_quadVAO = 0, m_quadVBO = 0;
    unsigned int m_baseQuadVAO = 0, m_baseQuadVBO = 0;

    float m_height_scale = 0.1f;
    float m_max_layer = 32.0f;

    // For the ImGui part
    bool m_enable_steep = false;
    bool m_use_basic = false;
    bool m_enable_discard = true;
    bool m_enable_rotate = false;
    bool m_divide_z = false;
    bool m_use_toy_box = false;
};
