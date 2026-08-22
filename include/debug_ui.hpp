#pragma once

#include <imgui.h>

#include "scene/SceneManager.hpp"

class DebugUI {
private:

public:
    DebugUI() = default;
    ~DebugUI() = default;
    void RenderUI(SceneManager &scene_manager);
};