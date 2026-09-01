#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "model.hpp"

class SSAOScene : public Scene {

public:
    const char* getName() const override { return "SSAO Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderCube();
    float ourLerp(float left, float right, float alpha);
    void regenerateKernel();
    void regenerateNoiseTexture();

private:
    Shader m_geo_shader, m_light_shader, m_ssao_shader, m_blur_shader;
    Model m_model;

    std::vector<glm::vec3> m_objectPositions;

    unsigned int NR_LIGHTS = 32;
    glm::vec3 m_cameraPos;
    glm::mat4 m_view_matrix, m_projection_matrix;
    std::vector<glm::vec3> m_ssaoKernel;

    // lighting info
    glm::vec3 m_lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 m_lightColor = glm::vec3(0.2, 0.2, 0.7);
    
    unsigned int m_attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    unsigned int m_gBuffer;
    unsigned int m_noiseTexture;
    unsigned int m_gPosition, m_gNormal, m_gAlbedoSpec;
    unsigned int m_ssaoFBO, m_ssaoBlurFBO, m_ssaoColorBuffer, m_ssaoColorBufferBlur;
    unsigned int m_rboDepth = 0;
    unsigned int m_quadVAO = 0, m_quadVBO = 0;
    unsigned int m_cubeVAO = 0, m_cubeVBO = 0;

    // For the ImGui part
    int m_kernelSize = 64;
    int m_noise_size = 4;
    float m_radius = 0.5f;
    float m_bias = 0.025f;
    float m_window_width;
    float m_window_height;
    bool m_enable_ssao = true;
    bool m_enable_blur = true;
};
