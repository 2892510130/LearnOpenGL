#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

class ShadowScene : public Scene {

public:
    const char* getName() const override { return "Shadow Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderCube();
    void renderScene(const Shader &shader, unsigned int planeVAO);

private:
    Shader m_shader, m_basic_shader;
    Texture m_wood_texture;

    glm::vec3 m_lightPos = glm::vec3(-2.0f, 4.0f, -1.0f), m_cameraPos;
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;
    glm::mat4 m_lightProjection, m_lightView, m_lightSpaceMatrix;

    unsigned int m_cubeVAO = 0;
    unsigned int m_cubeVBO = 0;
    unsigned int m_planeVAO = 0;
    unsigned int m_planeVBO = 0;

    unsigned int m_depthMapFBO;
    unsigned int m_depthMap;

    const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    float m_far_plane = 7.5f, m_near_plane = 1.0f;

    // For the ImGui part
    bool m_enable_shadow_offset = false;
    bool m_fix_peter_pin = false;
    bool m_enable_pcf = false;
    bool m_proj_light = false;
};
