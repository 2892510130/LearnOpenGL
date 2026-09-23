#pragma once

#include "Scene.hpp"
#include "animation/animator.h"
#include "animation/anim_model.h"

class AnimationScene : public Scene {

public:
    const char* getName() const override { return "Animation Scene"; }
    void init() override;
    void update(float deltaTime, Camera &camera) override;
    void render() override;
    void cleanup() override;
    void render_ui() override;

private:
    Shader m_shader;
    AnimationModel m_model;
	Animation m_dancing_animation;
	Animator m_animator;

    glm::vec3 m_cameraPos;
    glm::mat4 m_view_matrix, m_projection_matrix;
};
