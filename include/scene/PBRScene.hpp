#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "texture.hpp"

#include <vector>

class PBRScene : public Scene {

public:
    const char* getName() const override { return "PBR Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

    void renderSphere();
    void renderCube();

private:
    std::vector<Shader> m_shader = {};
    Texture m_albedo, m_normal, m_metallic, m_roughness, m_ao;

    glm::vec3 m_lightPositions[4] = {
        glm::vec3(-10.0f,  10.0f, 10.0f),
        glm::vec3( 10.0f,  10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f),
        glm::vec3( 10.0f, -10.0f, 10.0f),
    };
    glm::vec3 m_lightColors[4] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };
    int m_nrRows    = 7;
    int m_nrColumns = 7;
    float m_spacing = 2.5;

    glm::vec3 m_cameraPos;
    glm::mat4 m_view_matrix, m_projection_matrix;

    unsigned int m_cubeVAO = 0, m_cubeVBO = 0;
    unsigned int m_sphereVAO = 0, m_indexCount;
    unsigned int m_hdrTexture;
    unsigned int m_captureFBO;
    unsigned int m_captureRBO;
    unsigned int m_envCubemap;

    // ImGui settings
    int m_shader_number = 0;
    bool m_render_sky_box = true;
};