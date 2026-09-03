#pragma once

#include <imgui.h>

#include "scene/SceneManager.hpp"

class DebugUI {
private:
    float m_smooth_fps = 0.0f;
    float m_alpha = 0.033f;

public:
    DebugUI() = default;
    ~DebugUI() = default;
    void RenderUI(SceneManager &scene_manager);
};
