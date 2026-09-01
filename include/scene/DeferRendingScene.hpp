#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "model.hpp"

class DeferRendingScene : public Scene {

public:
    const char* getName() const override { return "Defer Rending Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderCube();

private:
    Shader m_geo_shader, m_light_shader, m_box_shader;
    Model m_model;

    std::vector<glm::vec3> m_objectPositions;

    unsigned int NR_LIGHTS = 32;
    glm::vec3 m_cameraPos;
    glm::mat4 m_view_matrix, m_projection_matrix;
    std::vector<glm::vec3> m_lightPositions, m_lightColors;
    
    unsigned int m_attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    unsigned int m_gBuffer;
    unsigned int m_gPosition, m_gNormal, m_gAlbedoSpec;
    unsigned int m_rboDepth = 0;
    unsigned int m_quadVAO = 0, m_quadVBO = 0;
    unsigned int m_cubeVAO = 0, m_cubeVBO = 0;

    // For the ImGui part
    bool m_copy_depth_buffer = true;
};
