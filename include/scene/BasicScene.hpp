#pragma once

#include "Scene.hpp"
#include "shader.hpp"
#include "model.hpp"

class BasicScene : public Scene {

public:
    const char* getName() const override { return "Basic Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

private:
    Shader m_shader;
    Model m_model;

    glm::vec3 m_lightPos = glm::vec3(0.0f, 0.0f, -2.0f);
    glm::mat4 m_model_matrix, m_view_matrix, m_projection_matrix;
};
