#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

class HDRScene : public Scene {

public:
    const char* getName() const override { return "HDR Map Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderCube();

private:
    Shader m_shader, m_light_shader;
    Texture m_diffuse_texture;

    glm::vec3 m_cameraPos;
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;
    std::vector<glm::vec3> m_lightPositions, m_lightColors;
    
    unsigned int m_hdrFBO = 0, m_colorBuffer = 0, m_rboDepth = 0;
    unsigned int m_quadVAO = 0, m_quadVBO = 0;
    unsigned int m_cubeVAO = 0, m_cubeVBO = 0;

    float m_height_scale = 0.1f;
    float m_max_layer = 32.0f;

    // For the ImGui part
    bool m_enable_gamma = true;
    int m_hdr = 0;
    float m_exposure = 0.3;
};
