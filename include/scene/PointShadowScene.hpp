#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <vector>

class PointShadowScene : public Scene {

public:
    const char* getName() const override { return "Point Shadow Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderQuad();
    void renderCube();
    void renderScene(const Shader &shader);

private:
    unsigned int m_cubeVAO = 0, m_cubeVBO = 0;

    unsigned int m_depthMapFBO;
    unsigned int m_depthMap;

    const int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    float m_far_plane = 25.0f, m_near_plane = 1.0f;

    Shader m_shader, m_depth_shader;
    Texture m_wood_texture;

    glm::vec3 m_lightPos = glm::vec3(0.0f, 0.0f, 0.0f), m_cameraPos;
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;
    std::vector<glm::mat4> m_shadowTransforms;

    // For the ImGui
    bool m_reverse_normal = false;
    bool m_enable_pcf = false;
};
